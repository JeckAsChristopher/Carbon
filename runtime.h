#ifndef RUNTIME_H
#define RUNTIME_H

void set_var(const char* name, const char* value);
char* get_var(const char* name);
int get_var_int(const char* name);
void set_var_int(const char* name, int value);
void cprint(const char* value);
void read_input(const char* name);
void set_var_bool(const char* name, int bool_value);
int get_var_bool(const char* name);
void cprint_bool(int val);

void enter_scope();
void exit_scope();

#endif
