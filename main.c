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

void cacheFile(char* line_buffer) {
    FILE *file = fopen("compile.bin", "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "%s", line_buffer);
    fclose(file);
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

    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
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

    FILE* file = fopen(fullPath, "r");

    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    verifySintaxe(file);

    return 0;
}
