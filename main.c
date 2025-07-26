#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "runtime.h"
#include "error.h"
#include "parser.h"

#define MAX_SOURCE_SIZE 8192

void run_repl() {
    char line[512];
    char full_input[MAX_SOURCE_SIZE] = {0};

    printf("Carbon REPL (BARE-C Version: 1.2-prerelease type 'exit' to exit\n");
    printf("This programming language has a lot more bugs, lack of syntaxes, please use this as an experiment-temporary only.\n");

    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit;") == 0 || strcmp(line, "exit") == 0)
            break;

        strcat(full_input, line);
        strcat(full_input, "\n");

        if (strchr(line, ';')) {
            init_lexer(full_input);
            while (peek_token().type != TOKEN_EOF) {
                parse_line();
            }
            full_input[0] = '\0';
        }
    }
}

void run_file(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext || strcmp(ext, ".ca") != 0) {
        throw_syntax("Invalid file extension. Use .ca");
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (length >= MAX_SOURCE_SIZE) {
        fclose(file);
        throw_syntax("Source file too large.");
    }

    char* source = malloc(length + 1);
    if (!source) {
        fclose(file);
        throw_syntax("Memory allocation failed.");
    }

    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);

    init_lexer(source);
    while (peek_token().type != TOKEN_EOF) {
        parse_line();
    }

    free(source);
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        run_file(argv[1]);
    } else {
        run_repl();
    }
    return 0;
}
