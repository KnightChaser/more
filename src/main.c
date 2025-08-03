// src/main.c
#include "utils.h"
#include <stdio.h>

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

    int terminal_rows = get_terminal_rows();
    int reply = 0;
    int lines_this_page = 0;

    // Continues as long as the user doesn't quit
    do {
        int lines_to_show = terminal_rows - 1;
        lines_this_page = display_page(file, lines_to_show);

        if (lines_this_page < lines_to_show) {
            break;
        }

        // Display the --More-- prompt and get the next command.
        printf("\033[7m--More--\033[m"); // \033[7m is inverse video, \033[m
                                         // resets it

        reply = get_user_command();
        printf("\r        \r"); // Clear the prompt line

        // 'q' to quit, ' ' for next page, Enter for next line.
        if (reply == ' ') {
            // User wants a full page, do nothing, the loop will handle it.
        } else if (reply == '\n') {
            // User wants one more line.
            display_page(file, 1);
        }
    } while (reply != 'q');

    fclose(file);
    return 0;
}
