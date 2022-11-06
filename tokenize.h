
#include <stdbool.h>

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
    int Len;
};



int expectNum(void);

bool consume(char* op);
void expect(char* op);

bool atEof(void);

void Tokenize(char* p);

