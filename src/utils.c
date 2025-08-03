// src/utils.c
#include <stdio.h>
#include <sys/ioctl.h>
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
