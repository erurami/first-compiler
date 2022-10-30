
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>


void error(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
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
        error("number expected");
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
    error("operator %c expected", operator);
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

        if (*p == '-' || *p == '+')
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

        error("couldn't tokenize");
    }

    newToken(TT_EOF, cur, NULL);
    return head.Next;
}





int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "unexpected arguments\n");
        return 1;
    }

    token = Tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expectNum());

    while (!atEof())
    {
        if (consume('+'))
        {
            printf("  add rax, %d\n", expectNum());
            continue;
        }
        if (consume('-'))
        {
            printf("  sub rax, %d\n", expectNum());
            continue;
        }
    }

    printf("  ret\n");
    return 0;
}

