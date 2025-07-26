#include "runtime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_VARS 128
#define MAX_NAME_LEN 32
#define MAX_VALUE_LEN 64

typedef struct {
    char name[MAX_NAME_LEN];
    char value[MAX_VALUE_LEN];
    int scope_depth;
    int is_number;
} Var;

static Var vars[MAX_VARS];
static int var_count = 0;
static int scope_depth = 0;

void enter_scope() {
    scope_depth++;
}

void exit_scope() {
    for (int i = var_count - 1; i >= 0; i--) {
        if (vars[i].scope_depth == scope_depth) {
            var_count--;
        } else {
            break;
        }
    }
    scope_depth--;
}

void set_var(const char* name, const char* value) {
    for (int i = var_count - 1; i >= 0; i--) {
        if (strcmp(vars[i].name, name) == 0) {
            strncpy(vars[i].value, value, MAX_VALUE_LEN - 1);
            vars[i].value[MAX_VALUE_LEN - 1] = '\0';
            vars[i].is_number = 0;
            return;
        }
    }

    if (var_count < MAX_VARS) {
        strncpy(vars[var_count].name, name, MAX_NAME_LEN - 1);
        vars[var_count].name[MAX_NAME_LEN - 1] = '\0';
        strncpy(vars[var_count].value, value, MAX_VALUE_LEN - 1);
        vars[var_count].value[MAX_VALUE_LEN - 1] = '\0';
        vars[var_count].is_number = 0;
        vars[var_count].scope_depth = scope_depth;
        var_count++;
    }
}

void set_var_int(const char* name, int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);

    for (int i = var_count - 1; i >= 0; i--) {
        if (strcmp(vars[i].name, name) == 0) {
            strncpy(vars[i].value, buf, MAX_VALUE_LEN - 1);
            vars[i].value[MAX_VALUE_LEN - 1] = '\0';
            vars[i].is_number = 1;
            return;
        }
    }

    if (var_count < MAX_VARS) {
        strncpy(vars[var_count].name, name, MAX_NAME_LEN - 1);
        vars[var_count].name[MAX_NAME_LEN - 1] = '\0';
        strncpy(vars[var_count].value, buf, MAX_VALUE_LEN - 1);
        vars[var_count].value[MAX_VALUE_LEN - 1] = '\0';
        vars[var_count].is_number = 0;
        vars[var_count].scope_depth = scope_depth;
        var_count++;
    }
}

void set_var_bool(const char* name, int bool_value) {
    const char* val = bool_value ? "true" : "false";

    for (int i = var_count - 1; i >= 0; i--) {
        if (strcmp(vars[i].name, name) == 0) {
            strncpy(vars[i].value, val, MAX_VALUE_LEN - 1);
            vars[i].value[MAX_VALUE_LEN - 1] = '\0';
            vars[i].is_number = 1;
            return;
        }
    }

    if (var_count < MAX_VARS) {
        strncpy(vars[var_count].name, name, MAX_NAME_LEN - 1);
        vars[var_count].name[MAX_NAME_LEN - 1] = '\0';
        strncpy(vars[var_count].value, val, MAX_VALUE_LEN - 1);
        vars[var_count].value[MAX_VALUE_LEN - 1] = '\0';
        vars[var_count].is_number = 0;
        vars[var_count].scope_depth = scope_depth;
        var_count++;
    }
}

char* get_var(const char* name) {
    for (int i = var_count - 1; i >= 0; i--) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].value;
        }
    }
    return "";
}

int get_var_int(const char* name) {
    return atoi(get_var(name));
}

int get_var_bool(const char* name) {
    const char* val = get_var(name);
    if (strcmp(val, "true") == 0) return 1;
    if (strcmp(val, "false") == 0) return 0;
    return -1;
}

void cprint(const char* value) {
    printf("%s\n", value);
}

void cprint_bool(int val) {
    if (val == 1)
        printf("true\n");
    else if (val == 0)
        printf("false\n");
    else
        printf("undefined\n");
}

void read_input(const char* name) {
    char buffer[MAX_VALUE_LEN];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        set_var(name, buffer);
    } else {
        set_var(name, "");
    }
}
