// src/main.c
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

// A reasonable maximum length for a single line of text.
#define MAX_LINE_LENGTH 4096

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
    int lines_to_show = terminal_rows - 1;
    char line_buffer[MAX_LINE_LENGTH] = {0};
    int lines_printed = 0;

    while (lines_printed < lines_to_show &&
           fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        printf("%s", line_buffer);
        lines_printed++;
    }

    fclose(file);

    return 0;
}
