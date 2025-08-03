// src/main.c
#include <stdio.h>
#include <stdlib.h>

// A reasonable maximum length for a single line of text.
#define MAX_LINE_LENGTH 4096

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

    char line_buffer[MAX_LINE_LENGTH] = {0};
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        printf("%s", line_buffer);
    }

    fclose(file);

    return 0;
}
