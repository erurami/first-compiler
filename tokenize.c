
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "tokenize.h"

static Token* token;

int KEYWORDS_COUNT = 12;

char* keywords[] = \
{
    ">=",
    ">",
    "<=",
    "<",
    "==",
    "!=",
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
        // if (isToken(p, "-")  ||
        //     isToken(p, "+")  ||
        //     isToken(p, "*")  ||
        //     isToken(p, "/")  ||
        //     isToken(p, "(")  ||
        //     isToken(p, ")")  ||
        //     isToken(p, "==") ||
        //     isToken(p, "!=")
        //     )
        // {
        //     cur = newToken(TT_RESERVED, cur, p, 1);
        //     p++;
        //     continue;
        // }

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

