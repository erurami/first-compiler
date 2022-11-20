
#include <stdbool.h>

typedef enum
{
    TT_IDENT,
    TT_NUM,

    TT_RESERVED,

    TT_RETURN,
    TT_IF,
    TT_ELSE,
    TT_WHILE,

    TT_EOF,
} TokenType;


typedef struct Token Token;

struct Token
{
    TokenType Type;
    Token*    Next;

    int   Value;
    char* Str;
    int   Len;
};



// expect TT_NUMBER and returns the value if it's number.
int expectNum(void);

// returns false if the next token is not TT_RESERVED.TT_RESERVED
// receives string and compares to the next token's string.
// If the strings are the same, returns true.
bool consume(char* op);

// exits the program if the next token is not TT_RESERVED.
// receives string and compares to the next token's string.
// If the strings are not the same, exits program with error msg.
void expect(char* op);

// returns NULL is the next token is not TT_IDENT.
// returns pointer to the string in the program.
// sets the length of the string to the received pointer to int.
char* consumeIdent(int* len);

// returns true if the Type of next token is the same as the type given.
bool consumeType(TokenType type);

// returns true if the next token is TT_EOF
bool atEof(void);



// receives pointer to the source code and tokenizes the code.
// The token array is stored in global variable that can only be accessed from tokenize.c
void Tokenize(char* p);

// print Tokens array to the stdout
void printTokens(void);

