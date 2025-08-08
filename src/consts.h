// src/consts.h
#pragma once

// A reasonable maximum length for a single line of text.
#define MAX_LINE_LENGTH 4096

// Default number of rows in the terminal if it can't detect the terminal
#define DEFAULT_ROWS 24

// ANSI Escape Codes for terminal manipulation
#define ANSI_INVERSE_VIDEO "\033[7m"
#define ANSI_RESET "\033[m"
#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_CURSOR_HOME "\033[H"
#define ANSI_CLEAR_LINE_FROM_CURSOR "\033[K"
