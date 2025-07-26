#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static char* src;
static int pos;

static Token peeked_token;
static int has_peeked = 0;

static char peek() { return src[pos]; }
static char advance() { return src[pos++]; }

void init_lexer(char* input) {
    src = input;
    pos = 0;
    has_peeked = 0;
}

static Token make_token(TokenType type, const char* text) {
    return (Token){ type, strdup(text) };
}

static void skip_whitespace() {
    while (isspace(peek())) advance();
}

Token next_token() {
    if (has_peeked) {
        has_peeked = 0;
        return peeked_token;
    }

    skip_whitespace();

    if (peek() == '\0') return make_token(TOKEN_EOF, "");

    char c = peek();

    // Identifiers and keywords
    if (isalpha(c)) {
        int start = pos;
        while (isalnum(peek())) advance();
        char* word = strndup(src + start, pos - start);

        if (strcmp(word, "let") == 0) return make_token(TOKEN_LET, word);
        if (strcmp(word, "cprint") == 0) return make_token(TOKEN_CPRINT, word);
        if (strcmp(word, "add") == 0) return make_token(TOKEN_ADD, word);
        if (strcmp(word, "sub") == 0) return make_token(TOKEN_SUB, word);
        if (strcmp(word, "mul") == 0) return make_token(TOKEN_MUL, word);
        if (strcmp(word, "div") == 0) return make_token(TOKEN_DIV, word);
        if (strcmp(word, "if") == 0) return make_token(TOKEN_IF, word);
        if (strcmp(word, "else") == 0) return make_token(TOKEN_ELSE, word);
        if (strcmp(word, "equals") == 0) return make_token(TOKEN_EQUALS, word);
        if (strcmp(word, "string") == 0) return make_token(TOKEN_STRING_TYPE, word);
        if (strcmp(word, "input") == 0) return make_token(TOKEN_INPUT, word);
        if (strcmp(word, "true") == 0) return make_token(TOKEN_TRUE, word);
        if (strcmp(word, "false") == 0) return make_token(TOKEN_FALSE, word);
        if (strcmp(word, "bool") == 0) return make_token(TOKEN_BOOL, word);
        if (strcmp(word, "func") == 0) return make_token(TOKEN_FUNC, word);
        if (strcmp(word, "cend") == 0) return make_token(TOKEN_CEND, word);

        return make_token(TOKEN_IDENTIFIER, word);
    }

    // String literals
    if (c == '"') {
        advance();
        int start = pos;
        while (peek() != '"' && peek() != '\0') advance();
        if (peek() == '\0') return make_token(TOKEN_EOF, ""); // unterminated string
        char* str = strndup(src + start, pos - start);
        advance();
        return make_token(TOKEN_STRING, str);
    }

    // Numbers
    if (isdigit(c)) {
        int start = pos;
        while (isdigit(peek())) advance();
        char* num = strndup(src + start, pos - start);
        return make_token(TOKEN_NUMBER, num);
    }

    // Symbols
    switch (advance()) {
        case '=': return make_token(TOKEN_ASSIGN, "=");
        case '(': return make_token(TOKEN_LPAREN, "(");
        case ')': return make_token(TOKEN_RPAREN, ")");
        case '{': return make_token(TOKEN_LBRACE, "{");
        case '}': return make_token(TOKEN_RBRACE, "}");
        case ';': return make_token(TOKEN_SEMICOLON, ";");
	case '!': return make_token(TOKEN_NOT, "!");
        case ':': return make_token(TOKEN_COLON, ":");
        default: break;
    }

    return make_token(TOKEN_EOF, "");
}

Token peek_token() {
    if (!has_peeked) {
        peeked_token = next_token();
        has_peeked = 1;
    }
    return peeked_token;
}
