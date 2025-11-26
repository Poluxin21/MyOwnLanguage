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
    NODE_INT_LITERAL,
    NODE_STRING_LITERAL,
    NODE_BOOL_LITERAL,

    NODE_VAR_DECLR,
    NODE_VAR_ASSIGN,
    NODE_VAR_ACESS,

    NODE_BINARY_OP,
    NODE_UNARY_OP,

    NODE_IF,
    NODE_WHILE,
    NODE_FOR,

    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
    NODE_RETURN,

    NODE_BLOCK,
    NODE_PROGRAM
} NodeType;

typedef struct AST {
    NodeType nodeType;

    union {
    struct {
        int value;
    } int_literal;

    struct {
        char* value;
    } string_literal;

    struct {
        bool value;
    } bool_literal;

    struct {

    }

    };
} AST;
