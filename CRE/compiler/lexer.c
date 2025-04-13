#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Thêm include cho isalpha, isalnum
#include "lexer.h"

#define MAX_TOKEN_LENGTH 256
#define MAX_TOKENS 1024

static const char *source;
static size_t position;
static size_t length;

static Token tokens[MAX_TOKENS];
static size_t token_count;

static char current_char() {
    return position < length ? source[position] : '\0';
}

static void advance() {
    position++;
}

static void skip_whitespace() {
    while (current_char() == ' ' || current_char() == '\n' || current_char() == '\t') {
        advance();
    }
}

static void add_token(TokenType type, const char *lexeme) {
    if (token_count < MAX_TOKENS) {
        tokens[token_count].type = type;
        tokens[token_count].start = strdup(lexeme); // allocate copy of lexeme
        tokens[token_count].length = strlen(lexeme);
        tokens[token_count].line = 1; // dummy value
        token_count++;
    }
}

static void lex_identifier() {
    size_t start = position;
    while (isalnum(current_char()) || current_char() == '_') {
        advance();
    }
    size_t len = position - start;
    char identifier[MAX_TOKEN_LENGTH];
    strncpy(identifier, &source[start], len);
    identifier[len] = '\0';

    // Các từ khóa của Covi 1.0 được đánh dấu là TOKEN_KEYWORD, còn lại là TOKEN_IDENTIFIER
    if (strcmp(identifier, "public") == 0) {
        add_token(TOKEN_KEYWORD, identifier);
    } else if (strcmp(identifier, "private") == 0) {
        add_token(TOKEN_KEYWORD, identifier);
    } else if (strcmp(identifier, "void") == 0) {
        add_token(TOKEN_KEYWORD, identifier);
    } else if (strcmp(identifier, "int") == 0) {
        add_token(TOKEN_KEYWORD, identifier);
    } else if (strcmp(identifier, "return") == 0) {
        add_token(TOKEN_KEYWORD, identifier);
    } else if (strcmp(identifier, "prinrnl") == 0) { // Sửa ở đây
        add_token(TOKEN_KEYWORD, identifier);
    } else {
        add_token(TOKEN_IDENTIFIER, identifier);
    }
}

static void lex_string() {
    advance(); // Skip opening quote
    size_t start = position;
    while (current_char() != '"' && current_char() != '\0') {
        advance();
    }
    size_t str_len = position - start;
    char string_buf[MAX_TOKEN_LENGTH];
    strncpy(string_buf, &source[start], str_len);
    string_buf[str_len] = '\0';
    add_token(TOKEN_LITERAL, string_buf);
    advance(); // Skip closing quote
}

void lex(const char *src) {
    source = src;
    position = 0;
    length = strlen(source);
    token_count = 0;

    while (current_char() != '\0') {
        skip_whitespace();

        if (isalpha(current_char())) {
            lex_identifier();
        } else if (current_char() == '"') {
            lex_string();
        } else if (current_char() == '(') {
            add_token(TOKEN_PUNCTUATION, "(");
            advance();
        } else if (current_char() == ')') {
            add_token(TOKEN_PUNCTUATION, ")");
            advance();
        } else if (current_char() == '{') {
            add_token(TOKEN_PUNCTUATION, "{");
            advance();
        } else if (current_char() == '}') {
            add_token(TOKEN_PUNCTUATION, "}");
            advance();
        } else if (current_char() == ';') {
            add_token(TOKEN_PUNCTUATION, ";");
            advance();
        } else {
            add_token(TOKEN_ERROR, "Unknown token");
            advance();
        }
    }
    add_token(TOKEN_EOF, "EOF");
}