
typedef enum
{
    NT_NUM,
    NT_ADD,
    NT_SUB,
    NT_MUL,
    NT_EQUAL,
    NT_NEQUAL,
    NT_GREATER,
    NT_GREATEREQUAL,
    NT_LESS,
    NT_LESSEQUAL,
    NT_DIV,
} NodeType;

typedef struct Node Node;

struct Node
{
    NodeType Type;
    Node* Lhs;
    Node* Rhs;
    int Value;
};


Node* parse(void);

void printNode(Node* node, int layer);
void genAsm(Node* node);


