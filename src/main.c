// src/main.c
#include "consts.h"
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
