
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "common.h"
#include "tokenize.h"

static Token* token;

int KEYWORDS_COUNT = 14;

char* keywords[] = \
{
    ">=",
    ">",
    "<=",
    "<",
    "==",
    "!=",
    "=",
    ";",
    "-",
    "+",
    "*",
    "/",
    "(",
    ")",
};


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
bool isToken(char* p, char* token, int len)
{
    if (memcmp(p, token, len) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

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

        bool token_found = false;
        for (int i = 0; i < KEYWORDS_COUNT; i++)
        {
            int len = strlen(keywords[i]);
            if (isToken(p, keywords[i], len))
            {
                cur = newToken(TT_RESERVED, cur, p, len);
                p += len;
                token_found = true;
                break;
            }
        }
        if (token_found) continue;

        if ('a' <= *p && *p <= 'z')
        {
            cur = newToken(TT_IDENT, cur, p, 1);
            p++;
            continue;
        }

        if (isdigit(*p))
        {
            char* num_p = p;
            int   value = strtol(p, &p, 10);
            cur = newToken(TT_NUM, cur, num_p, p - num_p);
            cur->Value = value;
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
            case TT_NULL:
                printf("type : NULL\n");
                break;
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
        }
        cur = cur->Next;
    }
}
