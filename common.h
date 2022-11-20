
#include <stdarg.h>
#include "parse.h"

char* SourceCode;

void error(char* fmt, ...);
void errorAt(char* location, char* fmt, ...);

