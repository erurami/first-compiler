
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
    switch (type)
    {
        case NT_ADD:
        case NT_SUB:
        case NT_MUL:
        case NT_DIV:
        case NT_EQUAL:
        case NT_NEQUAL:
        case NT_GREATER:
        case NT_GREATEREQUAL:
        case NT_LESS:
        case NT_LESSEQUAL:
        case NT_ASSIGN:
            node->HasValue = true;
            break;

        case NT_IF:
        case NT_IF_BLOCK:
        case NT_WHILE:
        case NT_RETURN:
            node->HasValue = false;
            break;
    }
    return node;
}

Node* newNumNode(int value)
{
    Node* node = calloc(1, sizeof(Node));
    node->Type = NT_NUM;
    node->Value = value;
    node->HasValue = true;
    return node;
}

Node* newIdentNode(char* ident, int len);

void initLocalValiablesDict(void);

// block = '{' statements
// statements = statement (statements | '}')?
// statement = expression ';'
//           | 'return' expression ';'
//           | 'if' '(' expression ')' block ('else' block)?
//           | 'while' '(' expression ')' block
// expression = assign
// assign = equality ('=' assign)?
// equality = comp ( '==' comp | '!-' comp)*
// comp = add ( '>' add | '>=' add | '<' add | '<=' add)*
// add = mul ( '+' mul | '-' mul)*
// mul = primary ( '*' primary | '/' primary )*
// unary = ('+' | '-')? primary
// primary = num | ident | '(' assign ')'

int IfCount;
int WhileCount;

Node* block(void);
Node* statements(void);
Node* statement(void);
Node* expression(void);
Node* assign(void);
Node* equality(void);
Node* comp(void);
Node* add(void);
Node* mul(void);
Node* unary(void);
Node* primary(void);

Node* parse(void)
{
    initLocalValiablesDict();
    IfCount = 1;
    WhileCount = 1;
    Node* node = block();
    if (!atEof())
    {
        error("Eof expected");
    }
    return node;
}

Node* block(void)
{
    expect("{");
    Node* node = statements();
    return node;
}

Node* statements(void)
{
    Node* node = statement();
    if (consume("}")) return node;

    node = newNode(NT_STATEMENT, node, statements());
    return node;
}

Node* statement(void)
{
    Node* node;
    if (consumeType(TT_RETURN))
    {
        node = expression();
        expect(";");
        return newNode(NT_RETURN, node, NULL);
    }
    if (consumeType(TT_IF))
    {
        expect("(");
        Node* if_cond = expression();
        expect(")");
        node = newNode(NT_IF_BLOCK, block(), NULL);
        if (consumeType(TT_ELSE))
        {
            node->Rhs = block();
        }
        node->IfId = IfCount;
        node = newNode(NT_IF, if_cond, node);
        node->IfId = IfCount;
        IfCount++;
        return node;
    }
    if (consumeType(TT_WHILE))
    {
        expect("(");
        Node* while_cond = expression();
        expect(")");
        node = newNode(NT_WHILE, while_cond, block());
        node->WhileId = WhileCount;
        WhileCount++;
        return node;
    }

    node = expression();
    expect(";");
    return node;
}

Node* expression(void)
{
    return assign();
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
    Node* lhs = add();

    while (1)
    {
        if (consume(">"))
        {
            lhs = newNode(NT_GREATER     , lhs, add());
            continue;
        }
        if (consume(">="))
        {
            lhs = newNode(NT_GREATEREQUAL, lhs, add());
            continue;
        }
        if (consume("<"))
        {
            lhs = newNode(NT_LESS        , lhs, add());
            continue;
        }
        if (consume("<="))
        {
            lhs = newNode(NT_LESSEQUAL   , lhs, add());
            continue;
        }
        return lhs;
    }
}

Node* add(void)
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




void printProgramTree(Node* node)
{
    printNode(node, 0);
}

void printNode(Node* node, int layer)
{
    if (node == NULL)
    {
        return;
    }
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

        case NT_RETURN:
            printf("%*ctype : RETURN\n", layer * 4, ' ');
            break;
        case NT_STATEMENT:
            printf("%*ctype : STATEMENT\n", layer * 4, ' ');
            break;

        case NT_IF:
            printf("%*ctype : IF\n", layer * 4, ' ');
            printf("%*cifid : %d\n", layer * 4, ' ', node->IfId);
            break;
        case NT_IF_BLOCK:
            printf("%*ctype : IF_BLOCK\n", layer * 4, ' ');
            printf("%*cifid : %d\n", layer * 4, ' ', node->IfId);
            break;

        case NT_WHILE:
            printf("%*ctype : WHILE\n", layer * 4, ' ');
            break;
    }

    if (node->Type == NT_RETURN)
    {
        printNode(node->Lhs, layer + 1);
        return;
    }

    if (node->Type != NT_NUM &&
        node->Type != NT_LVAL)
    {
        printNode(node->Lhs, layer + 1);
        printNode(node->Rhs, layer + 1);
    }
}
