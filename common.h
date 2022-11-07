
#include <stdarg.h>
#include "parse.h"

char* SourceCode;


struct LValDictStruct
{
    char** Vals;
    int ValsCount;
};

extern struct LValDictStruct LValDict;

void error(char* fmt, ...);
void errorAt(char* location, char* fmt, ...);

