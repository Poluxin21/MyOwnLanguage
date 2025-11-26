#include <ctype.h>
#include <stdio.h>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef enum {
    VAR_INT,
    VAR_FLOAT,
    VAR_STRING,
    VAR_BOOL,
    VAR_CHAR,
    TOKEN_GENERIC,
    ASSIGN,
    INDENTIFIER,
    NUMBER,
    SEMICOLON,
    TOKEN_EOF,
    TOKEN_NUMBER,
    TOKEN_DOUBLE_QUOT,
    TOKEN_QUOT,
    TOKEN_VAR_VALUE,
} tokenType;

typedef struct {
    tokenType type;
    char text[128];
} Token;

char* src;
int pos = 0;

char peek() {
    return src[pos];
}

char advance() {
    return src[pos++];
}

Token make_token(tokenType type, const char* text) {
    Token t;
    t.type = type;
    strcpy(t.text, text);
    return t;
}

Token next_token() {
    while (isspace(peek())) advance();
    char c = peek();
    if (c == '\0')
        return make_token(TOKEN_EOF, "");

    if (c == '"') {
        advance();
        char buffer[128];
        int i = 0;

        while (peek() != '"' && peek() != '\0') {
            buffer[i++] = advance();
        }

        if (peek() == '"') {
            advance();
        }

        buffer[i] = '\0';
        return make_token(TOKEN_VAR_VALUE, buffer);
    }

    if (c == '\'') {
        advance();
        char buffer[128];
        int i = 0;

        while (peek() != '\'' && peek() != '\0') {
            buffer[i++] = advance();
        }

        if (peek() == '\'') {
            advance();
        }

        buffer[i] = '\0';
        return make_token(TOKEN_VAR_VALUE, buffer);
    }

    if (isalpha(c)) {
        char buffer[128];
        int i = 0;

        while (isalnum(peek())) {
            buffer[i++] = advance();
        }

        buffer[i] = '\0';

        if (strcmp(buffer, "INT") == 0)
            return make_token(VAR_INT, buffer);
        if (strcmp(buffer, "FLOAT") == 0)
            return make_token(VAR_FLOAT, buffer);
        if (strcmp(buffer, "STRING") == 0)
            return make_token(VAR_STRING, buffer);
        if (strcmp(buffer, "BOOL") == 0)
            return make_token(VAR_BOOL, buffer);
        if (strcmp(buffer, "CHAR") == 0)
            return make_token(VAR_CHAR, buffer);

        return make_token(INDENTIFIER, buffer);
    }
    if (isdigit(c)) {
        char buffer[128];
        int i = 0;

        while (isdigit(peek()) || peek() == '.') {
            buffer[i++] = advance();
        }
        buffer[i] = '\0';

        return make_token(TOKEN_NUMBER, buffer);
    }

    if (c == '=') {
        advance();
        return make_token(ASSIGN, "=");
    }

    if (c == ';') {
        advance();
        return make_token(SEMICOLON, ";");
    }

    advance();
    return make_token(TOKEN_GENERIC, "?");
}
