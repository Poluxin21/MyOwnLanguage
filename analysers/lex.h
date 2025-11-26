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
#include <stdlib.h>

typedef enum {
    DT_UNKNOWN = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_CHAR
} DataType;

typedef enum {
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
    TOKEN_TYPE
} tokenType;

typedef struct {
    tokenType type;
    char* text;
    DataType dataType;
} Token;

typedef struct {
    const char* name;
    DataType type;
} Keyword;

Keyword keywords[] = {
    {"INT", TYPE_INT},
    {"FLOAT", TYPE_FLOAT},
    {"STRING", TYPE_STRING},
    {"BOOL", TYPE_BOOL},
    {"CHAR", TYPE_CHAR},
};

char* src;
int pos = 0;

char peek() {
    return src[pos];
}

char advance() {
    return src[pos++];
}

DataType lookup_keyword(const char* text) {
    for (int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
        if (strcmp(text, keywords[i].name) == 0)
            return keywords[i].type;
    }
    return DT_UNKNOWN;
}

Token make_token(tokenType type, const char* text) {
    Token t;
    t.type = type;
    t.dataType = DT_UNKNOWN;

    t.text = (char*)malloc(strlen(text) + 1);
    strcpy(t.text, text);

    return t;
}

Token make_type_token(DataType dt) {
    Token t;
    t.type = TOKEN_TYPE;
    t.dataType = dt;

    t.text = (char*)(malloc(16));
    strcpy(t.text, "<type>");

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

        // if (strcmp(buffer, "INT") == 0)
        //     return make_token(VAR_INT, buffer);
        // if (strcmp(buffer, "FLOAT") == 0)
        //     return make_token(VAR_FLOAT, buffer);
        // if (strcmp(buffer, "STRING") == 0)
        //     return make_token(VAR_STRING, buffer);
        // if (strcmp(buffer, "BOOL") == 0)
        //     return make_token(VAR_BOOL, buffer);
        // if (strcmp(buffer, "CHAR") == 0)
        //     return make_token(VAR_CHAR, buffer);

        DataType dt = lookup_keyword(buffer);
        if (dt != DT_UNKNOWN)
            return make_type_token(dt);
        else
            return make_token(INDENTIFIER, buffer);


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
