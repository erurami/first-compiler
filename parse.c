
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common.h"
#include "parse.h"
#include "tokenize.h"


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


// comp = expr( '==' expr | '!-' expr)*
// expr = mul ( '+' mul | '-' mul)*
// mul = primary ( '*' primary | '/' primary )*
// primary = ('+' | '-')? (num | '(' comp ')')


Node* comp(void);
Node* expr(void);
Node* primary(void);
Node* mul(void);

Node* parse(void)
{
    return comp();
}

Node* comp(void)
{
    Node* lhs = expr();

    while (1)
    {
        if (consume("=="))
        {
            lhs = newNode(NT_EQUAL, lhs, expr());
            continue;
        }
        if (consume("!="))
        {
            lhs = newNode(NT_NEQUAL, lhs, expr());
            continue;
        }
        return lhs;
    }
}

Node* expr(void)
{
    Node* lhs = mul();

    while (1)
    {
        if (consume("+"))
        {
            lhs = newNode(NT_ADD, lhs, mul());
            continue;
        }
        if (consume("-"))
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
        if (consume("*"))
        {
            lhs = newNode(NT_MUL, lhs, primary());
            continue;
        }
        if (consume("/"))
        {
            lhs = newNode(NT_DIV, lhs, primary());
            continue;
        }
        return lhs;
    }
}

Node* primary(void)
{
    bool unary = true;
    Node* node;

    if (consume("-"))
    {
        unary = false;
    }
    consume("+");

    if (consume("("))
    {
        node = comp();
        expect(")");
    }
    else
    {
        node = newNumNode(expectNum());
    }

    if (unary == false)
    {
        node = newNode(NT_SUB, newNumNode(0), node);
    }

    return node;
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
        case NT_EQUAL:
            printf("%*ctype : EQUAL\n", layer * 4, ' ');
            break;
        case NT_NEQUAL:
            printf("%*ctype : NEQUAL\n", layer * 4, ' ');
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

void genAsm(Node* node)
{
    if (node->Type == NT_NUM)
    {
        printf("  push %d\n", node->Value);
        return;
    }

    genAsm(node->Lhs);
    genAsm(node->Rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->Type)
    {
        case NT_ADD:
            printf("  add rax, rdi\n");
            break;
        case NT_SUB:
            printf("  sub rax, rdi\n");
            break;
        case NT_MUL:
            printf("  imul rax, rdi\n");
            break;
        case NT_DIV:
            printf("  cqo\n");
            printf("  idiv rax, rdi\n");
            break;
        case NT_EQUAL:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case NT_NEQUAL:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");
}

