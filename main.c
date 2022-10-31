
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

char* program;

void error(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void errorAt(char* location, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "%s\n", program);
    fprintf(stderr, "%*c^ ", (int)(location - program), ' ');
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}



typedef enum
{
    TT_NULL,
    TT_RESERVED,
    TT_NUM,
    TT_EOF,
} TokenType;

typedef struct Token Token;

struct Token
{
    TokenType Type;
    Token* Next;

    int Value;
    char* Str;
};


Token* token;


int expectNum(void)
{
    if (token->Type != TT_NUM)
    {
        errorAt(token->Str, "number expected");
    }
    int value = token->Value;
    token = token->Next;
    return value;
}

bool consume(char operator)
{
    if (token->Type != TT_RESERVED)
    {
        return false;
    }
    if (token->Str[0] != operator)
    {
        return false;
    }
    token = token->Next;
    return true;
}

void expect(char operator)
{
    if (consume(operator)) return;
    errorAt(token->Str, "operator %c expected", operator);
}

bool atEof(void)
{
    if (token->Type == TT_EOF) return true;
    return false;
}


Token* newToken(TokenType type, Token* cur, char* str)
{
    Token* token = calloc(1, sizeof(Token));
    token->Type = type;
    token->Str = str;
    cur->Next = token;
    return token;
}

Token* Tokenize(char* p)
{
    Token head;
    Token* cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (*p == '-' ||
            *p == '+' ||
            *p == '*' ||
            *p == '/' ||
            *p == '(' ||
            *p == ')')
        {
            cur = newToken(TT_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p))
        {
            cur = newToken(TT_NUM, cur, p);
            cur->Value = strtol(p, &p, 10);
            continue;
        }

        errorAt(p, "couldn't tokenize");
    }

    newToken(TT_EOF, cur, NULL);
    return head.Next;
}


// parse

// expr = mul ( '+' mul | '-' mul)*
// mul = primary ( '*' primary | '/' primary )*
// primary = num | '(' expr ')'


typedef enum
{
    NT_NUM,
    NT_ADD,
    NT_SUB,
    NT_MUL,
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

Node* newNode(NodeType type, Node* lhs, Node* rhs)
{
    Node* node = calloc(1, sizeof(Node));
    node->Type = type;
    node->Lhs = lhs;
    node->Rhs = rhs;
    return node;
}

Node* newNumNode(int value)
{
    Node* node = calloc(1, sizeof(Node));
    node->Type = NT_NUM;
    node->Value = value;
    return node;
}

Node* expr(void);
Node* primary(void);
Node* mul(void);

Node* expr(void)
{
    Node* lhs = mul();

    while (1)
    {
        if (consume('+'))
        {
            lhs = newNode(NT_ADD, lhs, mul());
            continue;
        }
        if (consume('-'))
        {
            lhs = newNode(NT_SUB, lhs, mul());
            continue;
        }
        return lhs;
    }
}

Node* mul(void)
{
    Node* lhs = primary();

    while (1)
    {
        if (consume('*'))
        {
            lhs = newNode(NT_MUL, lhs, primary());
            continue;
        }
        if (consume('/'))
        {
            lhs = newNode(NT_DIV, lhs, primary());
            continue;
        }
        return lhs;
    }
}

Node* primary(void)
{
    if (consume('('))
    {
        Node* node = expr();
        expect(')');
        return node;
    }

    return newNumNode(expectNum());
}

void printNode(Node* node, int layer)
{
    switch (node->Type)
    {
        case NT_ADD:
            printf("%*ctype : ADD\n", layer * 4, ' ');
            break;
        case NT_SUB:
            printf("%*ctype : SUB\n", layer * 4, ' ');
            break;
        case NT_MUL:
            printf("%*ctype : MUL\n", layer * 4, ' ');
            break;
        case NT_DIV:
            printf("%*ctype : DIV\n", layer * 4, ' ');
            break;
        case NT_NUM:
            printf("%*ctype : NUM\n", layer * 4, ' ');
            printf("%*cvalue : %d\n", layer * 4, ' ', node->Value);
            break;
    }

    if (node->Type != NT_NUM)
    {
        printNode(node->Lhs, layer + 1);
        printNode(node->Rhs, layer + 1);
    }
}





int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "unexpected arguments\n");
        return 1;
    }

    program = argv[1];

    token = Tokenize(argv[1]);

    // printf(".intel_syntax noprefix\n");
    // printf(".global main\n");
    // printf("main:\n");

    printNode(expr(), 1);

    // printf("  mov rax, %d\n", expectNum());

    // while (!atEof())
    // {
    //     if (consume('+'))
    //     {
    //         printf("  add rax, %d\n", expectNum());
    //         continue;
    //     }
    //     if (consume('-'))
    //     {
    //         printf("  sub rax, %d\n", expectNum());
    //         continue;
    //     }

    //     expect(' ');
    // }

    // printf("  ret\n");
    return 0;
}

