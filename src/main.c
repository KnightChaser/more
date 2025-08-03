// src/main.c
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    printf("--- Preparing to show file: %s ---\n", argv[1]);

    return 0;
}
