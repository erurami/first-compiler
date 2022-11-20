
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "common.h"
#include "tokenize.h"

static Token* token;

int OPERATORS_COUNT = 17;

char* operators[] = \
{
    ">=",
    ">",
    "<=",
    "<",
    "==",
    "!=",
    ",",
    "{",
    "}",
    "=",
    ";",
    "-",
    "+",
    "*",
    "/",
    "(",
    ")",
};


bool isIdentChar(char chara);

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

bool consume(char* operator)
{
    if (token->Type != TT_RESERVED)
    {
        return false;
    }
    int len = strlen(operator);

    if (token->Len != len) return false;
    if (memcmp(token->Str, operator, len) != 0) return false;

    token = token->Next;
    return true;
}

void expect(char* operator)
{
    if (consume(operator)) return;
    errorAt(token->Str, "operator %s expected", operator);
}

char* consumeIdent(int* len)
{
    if (token->Type != TT_IDENT)
    {
        return 0;
    }

    *len = token->Len;
    char* string = token->Str;

    token = token->Next;

    return string;
}

bool consumeType(TokenType type)
{
    if (token->Type != type)
    {
        return false;
    }

    token = token->Next;

    return true;
}

bool atEof(void)
{
    if (token->Type == TT_EOF) return true;
    return false;
}


Token* newToken(TokenType type, Token* cur, char* str, int len)
{
    Token* token = calloc(1, sizeof(Token));
    token->Type = type;
    token->Str = str;
    token->Len = len;
    cur->Next = token;
    return token;
}

// is (p) token (token) ?  -> true/false
bool isOperator(char* p, char* op, int len)
{
    if (memcmp(p, op, len) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#define IS_NEXT_IDENT(string, len) \
    (strncmp(p, string, len) == 0) && (isIdentChar(*(p + len)) == false)

void Tokenize(char* p)
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

        if (IS_NEXT_IDENT("return", 6))
        {
            cur = newToken(TT_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        if (IS_NEXT_IDENT("if", 2))
        {
            cur = newToken(TT_IF, cur, p, 2);
            p += 2;
            continue;
        }

        if (IS_NEXT_IDENT("else", 4))
        {
            cur = newToken(TT_ELSE, cur, p, 4);
            p += 4;
            continue;
        }

        if (IS_NEXT_IDENT("while", 5))
        {
            cur = newToken(TT_WHILE, cur, p, 5);
            p += 5;
            continue;
        }

        bool operator_found = false;
        for (int i = 0; i < OPERATORS_COUNT; i++)
        {
            int len = strlen(operators[i]);
            if (isOperator(p, operators[i], len))
            {
                cur = newToken(TT_RESERVED, cur, p, len);
                p += len;
                operator_found = true;
                break;
            }
        }
        if (operator_found) continue;

        if (isdigit(*p))
        {
            char* num_p = p;
            int   value = strtol(p, &p, 10);
            cur = newToken(TT_NUM, cur, num_p, p - num_p);
            cur->Value = value;
            continue;
        }

        if (isIdentChar(*p))
        {
            char* str = p;
            while (isIdentChar(*p)) p++;
            cur = newToken(TT_IDENT, cur, str, p - str);
            continue;
        }

        errorAt(p, "couldn't tokenize");
    }

    newToken(TT_EOF, cur, NULL, 0);
    token = head.Next;
}

void printTokens(void)
{
    Token* cur = token;

    while (cur->Type != TT_EOF)
    {
        switch(cur->Type)
        {
            case TT_RESERVED:
                printf("type : RESERVED\n");
                printf("String : %.*s\n", cur->Len, cur->Str);
                printf("Len : %d\n", cur->Len);
                break;
            case TT_IDENT:
                printf("type : IDENT\n");
                printf("String : %.*s\n", cur->Len, cur->Str);
                printf("Len : %d\n", cur->Len);
                break;
            case TT_NUM:
                printf("type : NUM\n");
                printf("Num : %d", cur->Value);
                break;
            case TT_RETURN:
                printf("type : RETURN\n");
                break;
            case TT_IF:
                printf("type : IF\n");
                break;
            case TT_ELSE:
                printf("type : ELSE\n");
                break;
            case TT_WHILE:
                printf("type : WHILE\n");
                break;
        }
        cur = cur->Next;
    }
}

bool isIdentChar(char chara)
{
    if (('a' <= chara && chara <= 'z') ||
        ('A' <= chara && chara <= 'Z') ||
        ('0' <= chara && chara <= '9') ||
        (chara == '_') ||
        (chara == '?'))
    {
        return true;
    }
    return false;
}

