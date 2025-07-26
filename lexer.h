#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_EOF,
    TOKEN_INPUT,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_LET,
    TOKEN_CPRINT,
    TOKEN_EQUALS,
    TOKEN_RBRACE,
    TOKEN_ELSE,
    TOKEN_LBRACE,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_STRING_TYPE,
    TOKEN_ASSIGN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_IF,
    TOKEN_SEMICOLON,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_FUNC,
    TOKEN_BOOL,
    TOKEN_NOT,
    TOKEN_COLON,
    TOKEN_CEND,
} TokenType;

typedef struct {
    TokenType type;
    char* text;
} Token;

void init_lexer(char* input);
Token next_token();
Token peek_token();

#endif
