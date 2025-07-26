// error.c
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void throw_syntax(const char* message) {
    fprintf(stderr, "[Syntax Error] %s\n", message);
    exit(1);
}

void throw_unexpected(const char* got, const char* expected) {
    fprintf(stderr, "[Syntax Error] Unexpected '%s', expected '%s'\n", got, expected);
    exit(1);
}

void throw_runtime(const char* message) {
    fprintf(stderr, "[Runtime Error] %s\n", message);
    exit(1);
}
