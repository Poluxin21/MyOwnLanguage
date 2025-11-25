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
#include "FileMap.h"

// void cacheFile(char* line_buffer) {
//     FILE *file = fopen("compile.bin", "w");
//     if (file == NULL) {
//         perror("Error opening file");
//         return;
//     }

//     fprintf(file, "%s", line_buffer);
//     fclose(file);
// }

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

bool validadeVariables(const char *variables[], char line_buffer[1024], int num_variables, int *foundType, char *equals_sign) {
    for (int i = 0; i < num_variables ; i++) {
        if (strstr(line_buffer, variables[i])) {
            *foundType = 1;
            const char* currentVar = variables[i];
            char* value = equals_sign + 1;
            while (isspace((unsigned char)*value)) value++;

            if (strcmp(currentVar, "INT") == 0) {
                if (!isdigit(*value)) {
                    fprintf(stderr, "Error: Invalid Sintaxe, you need a number at line\n");
                    *value = EXIT_FAILURE;
                    return true;
                }
            }

            if (strcmp(currentVar, "FLOAT") == 0) {
                if (!(isdigit(*value) || *value == '.')) {
                    fprintf(stderr, "Error: Invalid Sintaxe, you need a float number at line\n");
                    *value = EXIT_FAILURE;
                    return true;
                }
            }

            if (strcmp(currentVar, "STRING") == 0) {
                if (*value != '"') {
                    fprintf(stderr, "Error: Invalid Sintaxe, you need quotes at line\n");
                    *value = EXIT_FAILURE;
                    return true;
                }
            }

            if (strcmp(currentVar, "BOOL") == 0) {
                if (strncmp(value, "true", 4) != 0 && strncmp(value, "false", 5) != 0) {
                    fprintf(stderr, "Error: Invalid Sintaxe, you need 'true' or 'false' at line\n");
                    *value = EXIT_FAILURE;
                    return true;
                }
            }

            if (strcmp(currentVar, "CHAR") == 0) {
                if (*value != '\'') {
                    fprintf(stderr, "Error: Invalid Sintaxe, you need single quotes at line\n");
                    *value = EXIT_FAILURE;
                    return true;
                }
            }
        }
    }
    return false;
}

int verifySintaxe(FILE* file) {
    const char* variables[] = {"INT", "FLOAT", "STRING", "BOOL", "CHAR"};
    char line_buffer[1024];
    size_t lineLen;

    int num_variables = sizeof(variables) / sizeof(variables[0]);

    while (fgets(line_buffer, sizeof(line_buffer), file ) != NULL) {
        printf("%s", line_buffer); // RETIRAR DEPOIS
        lineLen = strlen(line_buffer);

        if (lineLen > 0 && line_buffer[lineLen - 1] == '\n') {
            line_buffer[--lineLen] = '\0';
        }

        if (lineLen > 0) {

            int foundType = 0;
            char* equals_sign = strchr(line_buffer, '=');
            if (equals_sign != NULL) {

                validadeVariables(variables, line_buffer, num_variables, &foundType, equals_sign);
                if (foundType == 0) {
                    fprintf(stderr, "Error: Invalid Sintaxe, variable type not found at line\n");
                    return EXIT_FAILURE;
                }

            }

            char* baseFunc = strchr(line_buffer, '(');
            if (baseFunc != NULL) {
                char* value = baseFunc + 1;
                while (isspace((unsigned char)*value)) value++;
            }



            if (line_buffer[lineLen - 1] != ';') {
                fprintf(stderr, "Error: Invalid Sintaxe, you need the ';' at line\n");
                return EXIT_FAILURE;
            }
        }
        return 0;
    }
}


int main(void) {
    char* mainFile = "main.atlx";

    #ifdef _WIN32
        char* currentDir = _getcwd(NULL, 0);
    #else
        char* currentDir = getcwd(NULL, 0);
    #endif

    if (currentDir == NULL) {
        perror("_getcwd");
        return EXIT_FAILURE;
    }
    printf("%s\n", currentDir);

    // APENAS DESENVOLVIMENTO //

    char* lastBackslash = strrchr(currentDir, '\\');
    if (lastBackslash != NULL && lastBackslash != currentDir) {
        *lastBackslash = '\0';
    }

    #ifdef _WIN32
        if (_chdir(currentDir) != 0) {
            perror("chdir");
        } else {
            printf("Mudou para: %s\n", _getcwd(NULL, 0));
        }
    #else
        if (chdir(currentDir) != 0) {
            perror("chdir");
        } else {
            printf("Mudou para: %s\n", getcwd(NULL, 0));
        }
    #endif

    /// //// ///

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s\\%s", currentDir, mainFile);

    if (strstr(fullPath, mainFile) == NULL) {
        printf("Main file not found, you need to have a %s to start", mainFile);
        return EXIT_FAILURE;
    }

    FileMap fm;

    if (!mapFile(fullPath, &fm)) {
        return EXIT_FAILURE;
	}

    src = fm.data;
    Token token;
    do {
        token = next_token();
        printf("Token: Type=%d, Text='%s'\n", token.type, token.text);
    } while (token.type != TOKEN_EOF);

	unmapFile(&fm);
    return 0;
}
