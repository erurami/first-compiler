
#include <stdarg.h>
#include "parse.h"

char* SourceCode;


struct LValDictStruct
{
    char** Vals;
    int ValsCount;
};

extern struct LValDictStruct LValDict;

extern int StatementsCount;
extern Node* ProgramBuf[100];

void error(char* fmt, ...);
void errorAt(char* location, char* fmt, ...);

