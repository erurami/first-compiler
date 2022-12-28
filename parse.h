
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

    NT_DEREFERENCE,
    NT_ADDRESS,

    // val and literal
    NT_NUM,
    NT_LVAL,
} NodeType;

typedef struct Node Node;

struct Node
{
    NodeType Type;
    Node* Lhs;
    Node* Rhs;

    int   Value;
    char* Str;
    int   Len;

    bool HasValue;
    bool IsAssignable;
    bool IsOnRam;
};

typedef enum
{
    ET_FUNCTION,
} ElementType;

typedef struct ProgramElem ProgramElem;

struct ProgramElem
{
    Node* Node;

    ElementType Type;
    char* Str;
    int   Len;

    ProgramElem* Next;
};

typedef struct GlobalIdent GlobalIdent;

typedef struct
{
    ProgramElem* FirstElem;
} Program;



// Parses the code just tokenized
// Returns the root node of the tree of the program
Program* parse(void);

void printProgramTree(Program* program);
void printNode(Node* node, int layer);

#endif

