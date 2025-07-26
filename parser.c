#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
#include "runtime.h"
#include "error.h"

void enter_scope();
void exit_scope();
static void parse_print();
static void parse_assignment(Token first);
static void parse_string_decl();
static void parse_if();
static void parse_input();
static void parse_bool_decl();
static int eval_condition();
void skip_block();
void parse_cend();

void parse_line() {
    Token tok = peek_token();

    switch (tok.type) {
        case TOKEN_CPRINT:
            next_token();
            parse_print();
            break;
        case TOKEN_LET:
        case TOKEN_IDENTIFIER:
            next_token();
            parse_assignment(tok);
            break;
        case TOKEN_STRING_TYPE:
            next_token();
            parse_string_decl();
            break;
        case TOKEN_IF:
            next_token();
            parse_if();
            break;
        case TOKEN_INPUT:
            next_token();
            parse_input();
            break;
        case TOKEN_ELSE:
        case TOKEN_EOF:
            break;
        case TOKEN_FALSE:
        case TOKEN_TRUE:
            break;
        case TOKEN_BOOL:
            next_token();
            parse_bool_decl();
            break;
        case TOKEN_CEND:
            next_token();
            parse_cend();
            break;
        default:
            throw_syntax("Unknown statement.");
    }
}

static void parse_print() {
    Token next = next_token();
    int negate = 0;

    if (next.type == TOKEN_IDENTIFIER && strcmp(next.text, "!") == 0) {
        negate = 1;
        next = next_token();
    }

    const char* left = NULL;
    char* result = NULL;

    if (next.type == TOKEN_LPAREN) {
        Token val = next_token();

        if (val.type == TOKEN_STRING) {
            left = val.text;
        } else if (val.type == TOKEN_IDENTIFIER) {
            left = get_var(val.text);
            if (!left) throw_runtime("Undefined variable in cprint");
        } else if (val.type == TOKEN_TRUE || val.type == TOKEN_FALSE) {
            int bval = (val.type == TOKEN_TRUE) ? 1 : 0;
            if (negate) bval = !bval;
            left = bval ? "true" : "false";
        } else {
            throw_syntax("Expected string, boolean, or variable in cprint()");
}

        Token maybe_add = peek_token();
        if (maybe_add.type == TOKEN_ADD) {
            next_token();
            Token right = next_token();
            const char* right_val = NULL;

            if (right.type == TOKEN_STRING) {
                right_val = right.text;
            } else if (right.type == TOKEN_IDENTIFIER) {
                right_val = get_var(right.text);
                if (!right_val) throw_runtime("Undefined variable in cprint");
            } else {
                throw_syntax("Expected string or variable after 'add'");
            }

            size_t total_len = strlen(left) + strlen(right_val) + 1;
            result = malloc(total_len);
            strcpy(result, left);
            strcat(result, right_val);
        } else {
            result = strdup(left);
        }

        Token closing = next_token();
        if (closing.type != TOKEN_RPAREN)
            throw_unexpected(closing.text, ")");

        cprint(result);
        free(result);

    } else if (next.type == TOKEN_IDENTIFIER) {
        const char* val = get_var(next.text);
        if (!val) throw_runtime("Undefined variable in cprint");

        if (negate) {
            if (strcmp(val, "true") == 0)
                cprint("false");
            else if (strcmp(val, "false") == 0)
                cprint("true");
            else
                throw_runtime("Cannot negate non-boolean variable in cprint");
        } else {
            cprint(val);
        }

    } else if (next.type == TOKEN_TRUE || next.type == TOKEN_FALSE) {
        int bval = (next.type == TOKEN_TRUE) ? 1 : 0;
        if (negate) bval = !bval;
        cprint(bval ? "true" : "false");

    } else {
        throw_syntax("Expected string, variable, or boolean in cprint");
    }

    Token semi = next_token();
    if (semi.type != TOKEN_SEMICOLON && semi.type != TOKEN_EOF)
        throw_unexpected(semi.text, ";");
}

int match_token(TokenType type) {
    if (peek_token().type == type) {
        next_token();
        return 1;
    }
    return 0;
}

void expect_token(TokenType type) {
    Token tok = next_token();
    if (tok.type != type) {
        throw_syntax("Unexpected token");
    }
}

void parse_if() {
    int negate = 0;
    if (peek_token().type == TOKEN_NOT) {
        next_token();
        negate = 1;
    }

    Token var = next_token();
    if (var.type != TOKEN_IDENTIFIER) {
        throw_syntax("Expected variable after 'if'");
    }

    Token next = peek_token();
    int has_equals = 0;
    Token expected_val;

    if (next.type == TOKEN_EQUALS) {
        next_token();
        has_equals = 1;

        expected_val = next_token();
        if (expected_val.type != TOKEN_NUMBER && expected_val.type != TOKEN_STRING &&
            expected_val.type != TOKEN_TRUE && expected_val.type != TOKEN_FALSE &&
            expected_val.type != TOKEN_IDENTIFIER) {
            throw_syntax("Expected number, string, boolean, or variable after 'equals'");
        }
    }

    Token open = next_token();
    if (open.type != TOKEN_LBRACE) {
        throw_syntax("Expected '{' after condition");
    }

    int matched = 0;

    if (has_equals) {
        const char* var_val = get_var(var.text);
        const char* expected = NULL;

        if (!var_val)
            throw_runtime("Undefined variable in if condition");

        if (expected_val.type == TOKEN_IDENTIFIER) {
            expected = get_var(expected_val.text);
            if (!expected)
                throw_runtime("Undefined variable in equals");
        } else if (expected_val.type == TOKEN_TRUE) {
            expected = "true";
        } else if (expected_val.type == TOKEN_FALSE) {
            expected = "false";
        } else {
            expected = expected_val.text;
        }

        matched = strcmp(var_val, expected) == 0;
    } else {
        const char* val = get_var(var.text);
        if (!val) throw_runtime("Undefined bool variable");
        matched = strcmp(val, "true") == 0;
    }

    if (negate) matched = !matched;

    if (matched) {
        while (peek_token().type != TOKEN_RBRACE && peek_token().type != TOKEN_EOF)
            parse_line();
        expect_token(TOKEN_RBRACE);

        if (match_token(TOKEN_ELSE)) {
            if (match_token(TOKEN_IF)) {
                parse_if();
            } else {
                expect_token(TOKEN_LBRACE);
                skip_block();
            }
        }
    } else {
        skip_block();

        if (match_token(TOKEN_ELSE)) {
            if (match_token(TOKEN_IF)) {
                parse_if();
            } else {
                expect_token(TOKEN_LBRACE);
                while (peek_token().type != TOKEN_RBRACE && peek_token().type != TOKEN_EOF)
                    parse_line();
  		    next_token();
		    parse_cend();
                expect_token(TOKEN_RBRACE);
            }
        }
    }
}

void skip_block() {
    int depth = 0;
    while (1) {
        Token tok = next_token();
        if (tok.type == TOKEN_LBRACE) depth++;
        else if (tok.type == TOKEN_RBRACE) {
            if (depth == 0) break;
            depth--;
        } else if (tok.type == TOKEN_EOF) break;
    }
}

static void parse_string_decl() {
    Token name = next_token();
    if (name.type != TOKEN_IDENTIFIER)
        throw_syntax("Expected identifier after 'string'");

    Token eq = next_token();
    if (eq.type != TOKEN_ASSIGN)
        throw_unexpected(eq.text, "=");

    Token val = next_token();
    if (val.type != TOKEN_STRING)
        throw_syntax("Expected string after '='");

    set_var(name.text, val.text);

    Token semi = next_token();
    if (semi.type != TOKEN_SEMICOLON && semi.type != TOKEN_EOF)
        throw_unexpected(semi.text, ";");
}

static void parse_assignment(Token first) {
    Token varname = (first.type == TOKEN_LET) ? next_token() : first;
    if (varname.type != TOKEN_IDENTIFIER)
        throw_syntax("Expected identifier");

    Token assign = next_token();
    if (assign.type != TOKEN_ASSIGN)
        throw_unexpected(assign.text, "=");

    Token val = next_token();

    if (val.type == TOKEN_STRING) {
        set_var(varname.text, val.text);
    } else if (val.type == TOKEN_NUMBER) {
        int left = atoi(val.text);
        Token op = peek_token();

        if (op.type == TOKEN_ADD || op.type == TOKEN_SUB ||
            op.type == TOKEN_MUL || op.type == TOKEN_DIV) {

            next_token();
            Token right = next_token();
            if (right.type != TOKEN_NUMBER)
                throw_syntax("Expected number after operator");

            int rightVal = atoi(right.text);
            switch (op.type) {
                case TOKEN_ADD: left += rightVal; break;
                case TOKEN_SUB: left -= rightVal; break;
                case TOKEN_MUL: left *= rightVal; break;
                case TOKEN_DIV:
                    if (rightVal == 0) throw_syntax("Division by zero.");
                    left /= rightVal;
                    break;
                default: break;
            }
        }

        set_var_int(varname.text, left);
    } else if (val.type == TOKEN_IDENTIFIER) {
        set_var(varname.text, get_var(val.text));
    } else if (val.type == TOKEN_TRUE || val.type == TOKEN_FALSE) {
        set_var_int(varname.text, val.type == TOKEN_TRUE ? 1 : 0);
    } else {
        throw_syntax("Invalid assignment value.");
    }

    Token semi = next_token();
    if (semi.type != TOKEN_SEMICOLON && semi.type != TOKEN_EOF)
        throw_unexpected(semi.text, ";");
}

static void parse_input() {
    Token next = peek_token();

    Token var;
    if (next.type == TOKEN_IDENTIFIER) {
        var = next_token();
    } else if (next.type == TOKEN_ASSIGN) {
        next_token();
        var = next_token();
        if (var.type != TOKEN_IDENTIFIER) {
            throw_syntax("Expected identifier after 'input ='");
        }
    } else {
        throw_syntax("Expected identifier or '=' after 'input'");
    }

    Token semi = next_token();
    if (semi.type != TOKEN_SEMICOLON && semi.type != TOKEN_EOF)
        throw_unexpected(semi.text, ";");

    char buffer[256];
    printf("> ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        set_var(var.text, buffer);
    } else {
        set_var(var.text, "");
    }
}

static void parse_bool_decl() {
    Token name = next_token();
    if (name.type != TOKEN_IDENTIFIER)
        throw_syntax("Expected identifier after 'bool'");

    Token next = peek_token();
    const char* val = NULL;

    if (next.type == TOKEN_COLON) {
        next_token();

        Token open = next_token();
        if (open.type != TOKEN_LBRACE)
            throw_syntax("Expected '{' after ':' in bool declaration");

        enter_scope();

        const char* result_val = NULL;

        while (1) {
            Token tok = peek_token();

            if (tok.type == TOKEN_CEND)
                break;

            if (tok.type == TOKEN_TRUE) {
                next_token();
                result_val = "true";
            } else if (tok.type == TOKEN_FALSE) {
                next_token();
                result_val = "false";
            } else if (tok.type == TOKEN_IDENTIFIER) {
                next_token();
                result_val = get_var(tok.text);
                if (!result_val)
                    throw_runtime("Undefined variable in bool block");
            } else {
                parse_line();
            }

            if (peek_token().type == TOKEN_SEMICOLON)
                next_token();
        }

	Token maybe_cend = next_token();
        if (maybe_cend.type != TOKEN_CEND)
            throw_syntax("Expected 'cend' at the end of block");

        Token close = next_token();
        if (close.type != TOKEN_RBRACE)
            throw_syntax("Expected '}' after bool block");

        exit_scope();

        if (!result_val)
            throw_syntax("Final statement in bool block must be true/false or variable");

        val = result_val;
        set_var(name.text, val);
        return;
    }

    Token assign = next_token();
    if (assign.type != TOKEN_ASSIGN)
        throw_unexpected(assign.text, "=");

    Token value = next_token();

    if (value.type == TOKEN_TRUE)
        val = "true";
    else if (value.type == TOKEN_FALSE)
        val = "false";
    else if (value.type == TOKEN_IDENTIFIER) {
        val = get_var(value.text);
        if (!val)
            throw_runtime("Undefined variable in bool declaration");
    } else {
        throw_syntax("Expected true/false or bool variable");
    }

    set_var(name.text, val);

    Token semi = next_token();
    if (semi.type != TOKEN_SEMICOLON && semi.type != TOKEN_EOF)
        throw_unexpected(semi.text, ";");
}

static int eval_condition() {
    Token tok = next_token();

    int negate = 0;

    if (tok.type == TOKEN_NOT) {
        negate = 1;
        tok = next_token();
    }

    if (tok.type != TOKEN_IDENTIFIER)
        throw_syntax("Expected boolean variable");

    const char* val = get_var(tok.text);
    if (!val) throw_runtime("Undefined bool variable");

    int result = (strcmp(val, "1") == 0);

    return negate ? !result : result;
}

void parse_cend() {
    Token tok = peek_token();

    printf("DEBUG: peeked token in parse_cend -> %s (type: %d)\n", tok.text, tok.type);

    if (tok.type != TOKEN_CEND)
        throw_syntax("Expected 'cend' at the end of block");

    Token consumed = next_token();
    printf("DEBUG: consumed token in parse_cend -> %s (type: %d)\n", consumed.text, consumed.type);
}
