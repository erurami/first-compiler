
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

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


int digitsCount(int num)
{
    int digits_c = 0;
    while (num != 0)
    {
        num /= 10;
        digits_c++;
    }
    return digits_c;
}

void errorAt(char* location, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    if (location == NULL)
    {
        exit(1);
    }

    int line_number = 1;
    int pos_in_line = 0;
    int line_width = 0;
    char* line_beginning = SourceCode;
    char* scanning = SourceCode;
    for (; scanning < location; scanning++)
    {
        switch (*scanning)
        {
            case '\n':
                line_number++;
                pos_in_line = 0;
                line_width = 0;
                line_beginning = scanning + 1;
                break;
            default:
                pos_in_line++;
                line_width++;
                break;
        }
    }
    pos_in_line++;
    while (1)
    {
        scanning++;
        if (*scanning == '\n') break;
        if (*scanning == '\0') break;
        line_width++;
    }

    char* line = malloc(sizeof(char) * (line_width + 1));
    memcpy(line, line_beginning, line_width * sizeof(char));
    line[line_width] = '\0';

    fprintf(stderr, "\n");
    fprintf(stderr, " %d | %s\n", line_number, line);
    fprintf(stderr, " %*c | %*c^\n", digitsCount(line_number), ' ', pos_in_line - 1, ' ');
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

    Program* program = parse();

    // printProgramTree(program);
    // printf("\n\n\n\n");
    genAsm(program);

    return 0;
}

