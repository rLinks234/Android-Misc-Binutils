#pragma once

#include <string>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static const string v_str = ANSI_COLOR_BLUE;
static const string w_str = ANSI_COLOR_YELLOW;
static const string e_str = ANSI_COLOR_RED;

static void print_verboseln(const char* pFormat, ...) {

	va_list vl;
	va_start(vl, pFormat);

	string fmt = v_str + pFormat;
	fmt += ANSI_COLOR_RESET;
	fmt += '\n';

	vprintf(fmt.c_str(), vl);

	va_end(vl);

}

static void print_warnln(const char* pFormat, ...) {

	va_list vl;
	va_start(vl, pFormat);

	string fmt = w_str + pFormat;
	fmt += ANSI_COLOR_RESET;
	fmt += '\n';

	vfprintf(stderr, fmt.c_str(), vl);

	va_end(vl);

}

static void print_errorln(const char* pFormat, ...) {

	va_list vl;
	va_start(vl, pFormat);

	string fmt = e_str + pFormat;
	fmt += ANSI_COLOR_RESET;
	fmt += '\n';

	vfprintf(stderr, fmt.c_str(), vl);

	va_end(vl);

}