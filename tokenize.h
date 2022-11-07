
#include <stdbool.h>

typedef enum
{
    TT_NULL,
    TT_RESERVED,
    TT_IDENT,
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
    int Len;
};



int expectNum(void);

bool consume(char* op);
void expect(char* op);

char* consumeIdent(int* len);

bool atEof(void);

void Tokenize(char* p);

void printTokens(void);

