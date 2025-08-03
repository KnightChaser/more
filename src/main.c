// src/main.c
#include "consts.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Continues as long as the user doesn't quit
    int terminal_rows = get_terminal_rows();
    int lines_this_page = display_page(file, terminal_rows - 1);
    if (lines_this_page < terminal_rows - 1) {
        // If the file is shorter than a single page,
        // display the entire content and exit.
        fclose(file);
        return 0;
    }

    while (true) {
        long current_pos = ftell(file);
        int percent_done =
            (file_size > 0) ? (current_pos * 100 / file_size) : 0;

        // Display the --More-- prompt and get the next command.
        // \033[7m is inverse video,
        // \033[m resets it
        printf("\033[7m--More-- (%d%%)\033[m", percent_done);
        fflush(stdout);

        int reply = get_user_command();
        printf("\r               \r");

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
                printf("\033[7mPattern not found: %s\033[m", search_term);
                fflush(stdout);
                sleep(2); // Pause so the user can read the message

                fseek(file, pre_search_pos, SEEK_SET);

                // Clear the "Pattern not found" message
                printf("\r                                          \r");
            }
        }
    }

    fclose(file);
    return 0;
}
