// src/main.c
#define _XOPEN_SOURCE 700
#include "consts.h"
#include "utils.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t g_resize_flag = 0;

void handle_sigwinch(int sig) {
    (void)sig;         // Suppress unused parameter warning
    g_resize_flag = 1; // Set the flag to indicate a resize event
}

int main(int argc, char *argv[]) {
    // Register the signal handler for terminal window size changes
    struct sigaction sa;
    sa.sa_handler = handle_sigwinch; // Set handler function
    sigemptyset(&sa.sa_mask);        // Don't block any other signals
    sa.sa_flags = SA_RESTART;        // Restart interrupted system calls
    if (sigaction(SIGWINCH, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    FILE *file = NULL;
    long file_size = -1;

    // Decide the input source: a file path argument or a pipe(stdin)
    if (argc > 1) {
        // A file path is provided as an argument
        const char *filename = argv[1];
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }

        // Get the ile size for seekable files
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);
    } else if (!isatty(STDIN_FILENO)) {
        // No file argument, but input is coming from a pipe
        file = stdin;
    } else {
        // No file and no pipe. Print usage.
        fprintf(stderr, "Usage: %s <filename\n", argv[0]);
        return 1;
    }

    // Continues as long as the user doesn't quit
    int terminal_rows = get_terminal_rows();
    long page_start_pos = 0; // Where the current page starts

    int lines_this_page = display_page(file, terminal_rows - 1);
    if (feof(file)) {
        fclose(file);
        return 0;
    }

    while (true) {
        if (g_resize_flag) {
            g_resize_flag = 0;
            terminal_rows = get_terminal_rows();

            // Reposition to the start of the current page
            fseek(file, page_start_pos, SEEK_SET);

            // Redraw the screen from that position with the new dimensions
            printf("%s%s", ANSI_CLEAR_SCREEN, ANSI_CURSOR_HOME);
            fflush(stdout);
            display_page(file, terminal_rows - 1);

            // If the redraw finished the file, exit
            if (feof(file)) {
                break;
            }
        }

        if (file_size > 0) {
            long current_pos = ftell(file);
            int percent_done =
                (file_size > 0) ? (current_pos * 100 / file_size) : 0;

            // Display the --More-- prompt and get the next command.
            printf("%s--More-- (%d%%)%s", ANSI_INVERSE_VIDEO, percent_done,
                   ANSI_RESET);
        } else {
            // No percentage if input is from a pipe
            printf("%s--More--%s", ANSI_INVERSE_VIDEO, ANSI_RESET);
        }
        fflush(stdout);

        int reply = get_user_command();
        printf("\r%s", ANSI_CLEAR_LINE_FROM_CURSOR);

        if (reply == 'q') {
            // 'q' to exit the loop and quit the program immediately
            break;
        } else if (reply == ' ') {
            // ' '(space) to show the next page
            lines_this_page = display_page(file, terminal_rows - 1);
            if (lines_this_page < terminal_rows - 1) {
                // End of file
                break;
            }
        } else if (reply == '\n') {
            // '\n' to show the next line
            lines_this_page = display_page(file, 1);
            if (lines_this_page < 1) {
                // End of file
                break;
            }
        } else if (reply == '/') {
            // '/' to search for a substring in the file
            char search_term[MAX_LINE_LENGTH] = {0};
            long pre_search_pos = ftell(file); // save current position

            printf("/");
            fflush(stdout);
            if (fgets(search_term, sizeof(search_term), stdin) == NULL) {
                // Input failed, continue to the next iteration
                continue;
            }

            // Remove the trailing newline that fgets() captures
            search_term[strcspn(search_term, "\n")] = 0;

            if (search_forward(file, search_term, terminal_rows)) {
                // search success, search_forward() allready displayed the new
                // page. Just continue the loop to show the next prompt.
                if (feof(file)) {
                    // Hit the end of the file
                    break;
                }
            } else {
                // If the search is not successful, restore the file position
                // after notifying the user
                printf("%sPattern not found: %s%s", ANSI_INVERSE_VIDEO,
                       search_term, ANSI_RESET);
                fflush(stdout);
                sleep(2); // Pause so the user can read the message

                fseek(file, pre_search_pos, SEEK_SET);

                // Clear the "Pattern not found" message
                printf("\r                                          \r");
            }
        }
    }

    fclose(file);
    printf("\n"); // Move to a new line after exiting...
    return 0;
}
