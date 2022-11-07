
#ifndef _PARSE_H_INCLUDE_
#define _PARSE_H_INCLUDE_

typedef enum
{
    // val and literal
    NT_NUM,
    NT_LVAL,

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

    // flow-controlling
    NT_RETURN,

} NodeType;

typedef struct Node Node;

struct Node
{
    NodeType Type;
    Node* Lhs;
    Node* Rhs;
    int Value;
    int LValOffset;
    char* pLValName;
    int LValNameLen;
};



void parse(void);

void printProgramTree(void);
void printNode(Node* node, int layer);

#endif

