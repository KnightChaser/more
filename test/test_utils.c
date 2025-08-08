// test/test_utils.c
#define _XOPEN_SOURCE 700
#include "../src/consts.h"
#include "../src/utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Create a temporary file with the given content.
 * The file is created in the /tmp directory and is automatically deleted
 * after it is closed.
 *
 * @param content The content to write into the temporary file.
 * @return A pointer to the opened file, or NULL if an error occurred.
 */
FILE *create_temp_file(const char *content) {
    char temp_filename[] = "/tmp/more_test_XXXXXX";
    int fd = mkstemp(temp_filename);
    if (fd == -1) {
        perror("mkstemp");
        return NULL;
    }

    write(fd, content, strlen(content));
    close(fd);

    // Reopen the file for reading
    FILE *file = fopen(temp_filename, "r");
    if (!file) {
        perror("fopen");
        unlink(temp_filename); // Clean up the temporary file anyway
        return NULL;
    }
    unlink(temp_filename);

    return file;
}

/**
 * Redirects the standard output to a file.
 * This is useful for testing purposes to capture output.
 *
 * @param filename The name of the file to redirect stdout to.
 */
void redirect_stdout(const char *filename) {
    fflush(stdout);
    freopen(filename, "w+", stdout);
}

/**
 * Restore the standard output to the terminal.
 * This is useful after redirecting stdout to a file for testing.
 */
void restore_stdout(void) {
    fflush(stdout);
    freopen("/dev/tty", "w+", stdout); // Restore stdout to the terminal
}

/**
 * Test the get_terminal_rows function.
 * It checks if the function returns a reasonable number of rows.
 */
void test_display_page_prints_correct_lines(void) {
    printf("Running test: test_display_page_prints_correct_lines\n");

    const char *file_content = "Line 1\nLine 2\nLine 3\nLine 4\n";
    FILE *test_file = create_temp_file(file_content);
    assert(test_file != NULL);

    const char *capture_filename = "/tmp/more_capture.txt";
    redirect_stdout(capture_filename);

    int lines_printed = display_page(test_file, 2);

    restore_stdout();
    fclose(test_file);

    // Verify the output
    FILE *capture_file = fopen(capture_filename, "r");
    char buffer[MAX_LINE_LENGTH] = {0};
    fread(buffer, 1, sizeof(buffer), capture_file);
    fclose(capture_file);
    unlink(capture_filename);

    const char *expected_output = "Line 1\nLine 2\n";
    assert(strcmp(buffer, expected_output) == 0);
    assert(lines_printed == 2);
    printf("PASSED\n");
}

/**
 * Test the search_forward function.
 * It checks if the function can find a term in the file and display the
 * correct output.
 */
void test_search_forward_finds_term(void) {
    printf("Running test: test_search_forward_finds_term\n");
    const char *file_content = "alpha\nbeta\ngamma\ndelta\n";
    FILE *test_file = create_temp_file(file_content);
    fseek(test_file, 0, SEEK_SET); // Ensure we start from the beginning

    const char *capture_filename = "/tmp/more_capture_search.txt";
    redirect_stdout(capture_filename);

    // We expect search_forward to clear the screen,
    // print the line, and then the rest of the page
    bool found = search_forward(test_file, "gamma", 10);

    restore_stdout();
    fclose(test_file);

    assert(found == true);

    // Verify the output
    FILE *capture_file = fopen(capture_filename, "r");
    char buffer[MAX_LINE_LENGTH] = {0};
    fread(buffer, 1, sizeof(buffer), capture_file);
    fclose(capture_file);
    unlink(capture_filename);

    // Note: The output includes ANSI codes and the rest of the file
    const char *expected_output = "\033[2J\033[Hgamma\ndelta\n";
    assert(strcmp(buffer, expected_output) == 0);
    printf("PASSED\n");
}

int main(void) {
    test_display_page_prints_correct_lines();
    test_search_forward_finds_term();
    return 0;
}
