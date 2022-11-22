
#ifndef _PARSE_H_INCLUDE_
#define _PARSE_H_INCLUDE_

#include <stdbool.h>

typedef enum
{
    NT_PROGRAM,

    // global
    NT_FUNCTION_DEF,
    NT_FUNCTION_ARGUMENT,

    // flow-controlling
    NT_STATEMENT,
    NT_RETURN,

    NT_IF,
    NT_IF_BLOCK,

    NT_WHILE,

    // function call
    NT_FUNCTION_CALL,
    NT_FUNCTION_CALL_PARAM,

    // operators
    NT_ADD,
    NT_SUB,
    NT_MUL,
    NT_DIV,

    NT_EQUAL,
    NT_NEQUAL,
    NT_GREATER,
    NT_GREATEREQUAL,
    NT_LESS,
    NT_LESSEQUAL,

    NT_ASSIGN,

    // val and literal
    NT_NUM,
    NT_LVAL,
} NodeType;

typedef struct Node Node;

// TODO: make general perpose member e.g. Str to store string.
struct Node
{
    NodeType Type;
    Node* Lhs;
    Node* Rhs;

    int   Value;
    char* Str;
    int   Len;

    bool HasValue;
};



// Parses the code just tokenized
// Returns the root node of the tree of the program
Node* parse(void);

void printProgramTree(Node* node);
void printNode(Node* node, int layer);

void printFunctionNames(void);

#endif

