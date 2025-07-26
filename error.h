// error.h
#ifndef ERROR_H
#define ERROR_H

void throw_syntax(const char* message);
void throw_unexpected(const char* got, const char* expected);
void throw_runtime(const char* message);

#endif
