
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "parse.h"
#include "tokenize.h"


typedef struct FunctionName FunctionName;

struct FunctionName
{
    char* Name;
    int Len;
    FunctionName* Next;
};


struct LValDictStruct
{
    char** Vals;
    int ValsCount;
};

extern struct LValDictStruct LValDict;


// stores function names that appears in the code.
FunctionName* FunctionNames;
FunctionName* FunctionNameCur;


// functions for new node
Node* newNode(NodeType type, Node* lhs, Node* rhs);
Node* newNumNode(int value);
Node* newLvalNode(char* ident, int len);


void initLocalValiablesDict(void);

int IfCount;
int WhileCount;

Node* program(void);
Node* function(void);
Node* arglist(void);
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
Node* lval(void);

Node* parse(void)
{
    IfCount = 1;
    WhileCount = 1;

    Node* node;

    FunctionName fn_head;
    FunctionNameCur = &fn_head;

    node = program();

    FunctionNames = fn_head.Next;

    return node;
}

Node* program(void)
{
    Node* node = function();
    if (atEof()) return node;

    node = newNode(NT_PROGRAM, node, program());
    return node;
}

Node* function(void)
{
    initLocalValiablesDict();

    int function_name_len;
    char* function_name = consumeIdent(&function_name_len);

    expect("(");

    Node* arglist_node = NULL;
    if (!consume(")"))
    {
        arglist_node = arglist();
    }

    Node* node = newNode(NT_FUNCTION_DEF, arglist_node, block());
    node->Str = function_name;
    node->Len = function_name_len;
    node->Value = LValDict.ValsCount;

    FunctionName* func_name = (FunctionName*)calloc(1, sizeof(FunctionName));
    func_name->Name = node->Str;
    func_name->Len  = node->Len;
    FunctionNameCur->Next = func_name;
    FunctionNameCur = FunctionNameCur->Next;

    return node;
}

Node* arglist(void)
{
    Node* node = lval();
    if (consume(")")) return node;
    expect(",");

    node = newNode(NT_FUNCTION_ARGUMENT, node, arglist());
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
        node->Value = IfCount;
        node = newNode(NT_IF, if_cond, node);
        node->Value = IfCount;
        IfCount++;
        return node;
    }
    if (consumeType(TT_WHILE))
    {
        expect("(");
        Node* while_cond = expression();
        expect(")");
        node = newNode(NT_WHILE, while_cond, block());
        node->Value = WhileCount;
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
        return node;
    }
    // TODO : too complex
    if (ident = consumeIdent(&ident_len))
    {
        if (consume("("))
        {
            node = newNode(NT_FUNCTION_CALL, NULL, NULL);
            node->Str = ident;
            node->Len = ident_len;
            node->Value = 0;

            if (consume(")")) return node;

            Node* node_function = node;

            while (1)
            {
                node->Rhs = newNode(NT_FUNCTION_CALL_PARAM, expression(), NULL);
                node = node->Rhs;
                node_function->Value++;
                if (!consume(","))
                {
                    expect(")");
                    break;
                }
            }
            return node_function;
        }
        node = newLvalNode(ident, ident_len);
        return node;
    }
    else
    {
        node = newNumNode(expectNum());
        return node;
    }

    return node;
}

Node* lval(void)
{
    int ident_len;
    char* ident = consumeIdent(&ident_len);

    return newLvalNode(ident, ident_len);
}




// Dictionaly for local variables
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
            printf("%*cvalue id : %d\n", layer * 4, ' ', node->Value);
            break;

        case NT_FUNCTION_CALL:
            printf("%*ctype : FUNCTION_CALL\n", layer * 4, ' ');
            printf("%*cname : %.*s\n", layer * 4, ' ', node->Len, node->Str);
            printf("%*cparam count : %d\n", layer * 4, ' ', node->Value);
            break;
        case NT_FUNCTION_CALL_PARAM:
            printf("%*ctype : FUNCTION_CALL_PARAM\n", layer * 4, ' ');
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
            printf("%*cifid : %d\n", layer * 4, ' ', node->Value);
            break;
        case NT_IF_BLOCK:
            printf("%*ctype : IF_BLOCK\n", layer * 4, ' ');
            printf("%*cifid : %d\n", layer * 4, ' ', node->Value);
            break;

        case NT_WHILE:
            printf("%*ctype : WHILE\n", layer * 4, ' ');
            break;

        case NT_FUNCTION_ARGUMENT:
            printf("%*ctype : FUNCTION_ARGUMENT\n", layer * 4, ' ');
            break;

        case NT_FUNCTION_DEF:
            printf("%*ctype : FUNCTION_DEF\n", layer * 4, ' ');
            printf("%*cfunction name : %.*s\n", layer * 4, ' ', node->Len, node->Str);
            printf("%*cfunction name len : %d\n", layer * 4, ' ', node->Len);
            break;

        case NT_PROGRAM:
            printf("%*ctype : PROGRAM\n", layer * 4, ' ');
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

void printFunctionNames(void)
{
    FunctionName* cur = FunctionNames;

    bool first_node = true;
    while (cur != NULL)
    {
        if (first_node == false) printf(", ");
        printf("%.*s", cur->Len, cur->Name);
        first_node = false;
        cur = cur->Next;
    }

}


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

Node* newLvalNode(char* ident, int len)
{
    Node* node;
    node = newNode(NT_LVAL, NULL, NULL);
    node->Value = getLValId(ident, len);
    node->Str = ident;
    node->Len = len;
    node->HasValue = true;
}

