
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

char* readSourceFile(char* filename)
{

    FILE* p_file = fopen(filename, "r");
    if (p_file == NULL) error("error opening file");

    unsigned long filesize;
    fseek(p_file, 0L, SEEK_END);
    filesize = ftell(p_file);

    fseek(p_file, 0L, SEEK_SET);

    char* content = (char*)malloc(filesize);
    for (unsigned long i = 0; i < filesize; i++)
    {
        content[i] = fgetc(p_file);
    }
    fclose(p_file);
    content[filesize - 1] = '\0';

    return content;
}




int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "unexpected arguments\n");
        return 1;
    }

    SourceCode = readSourceFile(argv[1]);

    Tokenize(SourceCode);

    // printTokens();

    Node* program_tree = parse();

    // printProgramTree(program_tree);
    // printFunctionNames();
    // printf("\n\n\n\n");
    genAsm(program_tree);

    return 0;
}

