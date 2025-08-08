// src/utils.h
#pragma once
#include <stdbool.h>
#include <stdio.h>

int get_terminal_rows(void);
int get_user_command(void);
int display_page(FILE *file, int num_lines);
bool search_forward(FILE *file, const char *term, int terminal_rows);
