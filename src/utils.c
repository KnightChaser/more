// src/utils.c
#include "consts.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/**
 * Get the number of rows in the terminal.
 * It uses ioctl() to query the terminal's window size.
 *
 * @return The number of rows in the terminal.
 *         If the ioctl call fails, it returns a default value of 24.
 */
int get_terminal_rows(void) {
    struct winsize w;

    /**
     * ioctl() queries the terminal driver for its window size.
     * STDOUT_FILENO is to specify the standard output terminal.
     * TIOCGWINSZ is the specific request to get the window size.
     * The result is placed in the winsize structure.
     */
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("ioctl");
        return 24;
    }

    return w.ws_row;
}

/**
 * Get a single character input from the user without waiting for Enter key.
 * This function sets the terminal to raw mode to read a single character.
 *
 * @return The character read from the user.
 */
int get_user_command(void) {
    struct termios original_settings, new_settings;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &original_settings);
    new_settings = original_settings;

    // Change settings for the raw mode
    new_settings.c_lflag &= ~(ICANON | ECHO);

    // Apply the new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    // Flushes input data that has been received by the system but not read by
    // an application (a pending input).
    tcflush(STDIN_FILENO, TCIFLUSH);

    // Wait for a single character
    int ch = getchar();

    // NOTE: restore the original terminal settings after reading input
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings);

    return ch;
}

/**
 * Display a page of text from a file.
 * It reads and prints a specified number of lines from the file.
 *
 * @param file The file pointer to read from.
 * @param num_lines The number of lines to display.
 * @return The number of lines actually printed.
 */
int display_page(FILE *file, int num_lines) {
    char line_buffer[MAX_LINE_LENGTH] = {0};
    int lines_printed = 0;

    while (lines_printed < num_lines &&
           fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        printf("%s", line_buffer);
        lines_printed++;
    }

    return lines_printed;
}

/**
 * Search for a term in the file from the current position.
 * If the term is found, it clears the screen and displays the line containing
 * the term, then continues displaying the rest of the page.
 *
 * @param file The file pointer to search in.
 * @param term The search term to look for.
 * @param terminal_rows The number of rows in the terminal to display.
 * @return true if the term was found and displayed, false otherwise.
 */
bool search_forward(FILE *file, const char *term, int terminal_rows) {
    char line_buffer[MAX_LINE_LENGTH] = {0};

    // Read line-by-lline from the current file position and search for the term
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        if (strstr(line_buffer, term) != NULL) {
            // Clear the entire screen and move the cursor to the top-left
            // (home)
            // and flush to ensure screen clears immediately
            printf("%s%s", ANSI_CLEAR_SCREEN, ANSI_CURSOR_HOME);
            fflush(stdout);

            printf("%s", line_buffer);

            // Continue displaying rest of the page
            display_page(file, terminal_rows - 2);

            return true;
        }
    }
    return false;
}
