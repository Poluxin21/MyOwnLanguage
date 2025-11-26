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
#include "analysers/lex.h"

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

    #ifdef _WIN32
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", currentDir, mainFile);
    #else
        snprintf(fullPath, sizeof(fullPath), "%s/%s", currentDir, mainFile);
    #endif

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
