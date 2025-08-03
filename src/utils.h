// src/utils.h
#pragma once
#include <stdio.h>

int get_terminal_rows(void);
int get_user_command(void);
int display_page(FILE *file, int num_lines);
int search_forward(FILE *file, const char *term, int terminal_rows);
