
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "parse.h"
#include "tokenize.h"


int StatementsCount = 0;
Node* ProgramBuf[100];

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

Node* newIdentNode(char* ident, int len);

void initLocalValiablesDict(void);

// program = statement*
// statement = assign ';'
// assign = equality ('=' assign)?
// equality = comp ( '==' comp | '!-' comp)*
// comp = expr ( '>' expr | '>=' expr | '<' expr | '<=' expr)*
// expr = mul ( '+' mul | '-' mul)*
// mul = primary ( '*' primary | '/' primary )*
// unary = ('+' | '-')? primary
// primary = num | ident | '(' assign ')'


Node** program(void);
Node* statement(void);
Node* assign(void);
Node* equality(void);
Node* comp(void);
Node* expr(void);
Node* mul(void);
Node* unary(void);
Node* primary(void);

void parse(void)
{
    initLocalValiablesDict();
    program();
}

Node** program(void)
{
    while (1)
    {
        ProgramBuf[StatementsCount++] = statement();
        if (atEof()) break;
    }
    return ProgramBuf;
}

Node* statement(void)
{
    Node* node = assign();
    expect(";");
    return node;
}

Node* assign(void)
{
    Node* node = equality();
    if (consume("="))
    {
        node = newNode(NT_ASSIGN, node, assign());
    }
    return node;
}

Node* equality(void)
{
    Node* lhs = comp();

    while (1)
    {
        if (consume("=="))
        {
            lhs = newNode(NT_EQUAL, lhs, comp());
            continue;
        }
        if (consume("!="))
        {
            lhs = newNode(NT_NEQUAL, lhs, comp());
            continue;
        }
        return lhs;
    }
}

Node* comp(void)
{
    Node* lhs = expr();

    while (1)
    {
        if (consume(">"))
        {
            lhs = newNode(NT_GREATER     , lhs, expr());
            continue;
        }
        if (consume(">="))
        {
            lhs = newNode(NT_GREATEREQUAL, lhs, expr());
            continue;
        }
        if (consume("<"))
        {
            lhs = newNode(NT_LESS        , lhs, expr());
            continue;
        }
        if (consume("<="))
        {
            lhs = newNode(NT_LESSEQUAL   , lhs, expr());
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
    Node* lhs = unary();

    while (1)
    {
        if (consume("*"))
        {
            lhs = newNode(NT_MUL, lhs, unary());
            continue;
        }
        if (consume("/"))
        {
            lhs = newNode(NT_DIV, lhs, unary());
            continue;
        }
        return lhs;
    }
}

Node* unary(void)
{
    bool plus = true;
    if (consume("-"))
    {
        plus = false;
    }
    else
    {
        consume("+");
    }

    Node* node = primary();

    if (plus == false)
    {
        node = newNode(NT_SUB, newNumNode(0), node);
    }

    return node;
}

Node* primary(void)
{
    Node* node;
    char* ident;
    int ident_len;

    if (consume("("))
    {
        node = equality();
        expect(")");
    }
    else if (ident = consumeIdent(&ident_len))
    {
        node = newIdentNode(ident, ident_len);
    }
    else
    {
        node = newNumNode(expectNum());
    }

    return node;
}



struct LValDictStruct LValDict;

void initLocalValiablesDict(void)
{
    LValDict.Vals = NULL;
    LValDict.ValsCount = 0;
}

int getLValId(char* ident, int len)
{
    for (int i = 0; i < LValDict.ValsCount; i++)
    {
        if (strlen(LValDict.Vals[i]) != len) continue;
        if (memcmp(LValDict.Vals[i], ident, len) == 0) return i + 1;
    }

    char** new_vals_buf;
    new_vals_buf = (char**)malloc(sizeof(char*) * (LValDict.ValsCount + 1));

    for (int i = 0; i < LValDict.ValsCount; i++)
    {
        new_vals_buf[i] = LValDict.Vals[i];
    }

    free(LValDict.Vals);
    LValDict.Vals = new_vals_buf;

    LValDict.Vals[LValDict.ValsCount] = (char*)calloc((len + 1), sizeof(char));
    memcpy(LValDict.Vals[LValDict.ValsCount], ident, len);

    return ++LValDict.ValsCount;
}


Node* newIdentNode(char* ident, int len)
{
    Node* node;
    node = newNode(NT_LVAL, NULL, NULL);
    node->LValOffset = getLValId(ident, len) * 8;
    node->pLValName = ident;
    node->LValNameLen = len;
}




void printProgramTree(void)
{
    for (int i = 0; i < StatementsCount; i++)
    {
        printf("%d th statement\n", i + 1);
        printf("\n");
        printNode(ProgramBuf[i], 0);
        printf("================================\n");
    }
}

void printNode(Node* node, int layer)
{
    switch (node->Type)
    {
        case NT_NUM:
            printf("%*ctype : NUM\n", layer * 4, ' ');
            printf("%*cvalue : %d\n", layer * 4, ' ', node->Value);
            break;
        case NT_LVAL:
            printf("%*ctype : LVAL\n", layer * 4, ' ');
            printf("%*cvalue id : %d\n", layer * 4, ' ', node->LValOffset / 8);
            break;

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
        case NT_GREATER:
            printf("%*ctype : GREATER\n", layer * 4, ' ');
            break;
        case NT_GREATEREQUAL:
            printf("%*ctype : GREATEREQUAL\n", layer * 4, ' ');
            break;
        case NT_LESS:
            printf("%*ctype : LESS\n", layer * 4, ' ');
            break;
        case NT_LESSEQUAL:
            printf("%*ctype : LESSEQUAL\n", layer * 4, ' ');
            break;

        case NT_ASSIGN:
            printf("%*ctype : ASSIGN\n", layer * 4, ' ');
            break;
    }

    if (node->Type != NT_NUM &&
        node->Type != NT_LVAL)
    {
        printNode(node->Lhs, layer + 1);
        printNode(node->Rhs, layer + 1);
    }
}
