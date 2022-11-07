
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "common.h"
#include "tokenize.h"
#include "parse.h"
#include "codegen.h"

void error(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void errorAt(char* location, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fprintf(stderr, "%s\n", SourceCode);
    if (location != NULL)
    {
        fprintf(stderr, "%*c^ \n", (int)(location - SourceCode), ' ');
    }
    exit(1);
}




int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "unexpected arguments\n");
        return 1;
    }

    SourceCode = argv[1];

    Tokenize(argv[1]);

    Node* program_tree = parse();

    genAsm(program_tree);

    return 0;
}

