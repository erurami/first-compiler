
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "common.h"
#include "tokenize.h"
#include "parse.h"

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
    fprintf(stderr, "%s\n", program);
    fprintf(stderr, "%*c^ ", (int)(location - program), ' ');
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}




int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "unexpected arguments\n");
        return 1;
    }

    program = argv[1];

    Tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    genAsm(parse());

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}

