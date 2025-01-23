#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/lexer.h"
#include "ast.h"
#include "parser.h"

static FILE *gere_usage(int argc, char *argv[])
{
    if (argc == 1)
    {
        return stdin;
    }
    else
    {
        if (!strcmp(argv[1], "-c"))
        {
            if (argc <= 2)
            {
                return NULL;
            }
            return fmemopen(argv[2], (strlen(argv[2])), "r");
        }
        else
        {
            return fopen(argv[1], "r");
        }
    }
}

char lexer_file(FILE *hd)
{
    return fgetc(hd);
}

int main(int argc, char *argv[])
{
    FILE *value = gere_usage(argc, argv);
    if (!value)
    {
        return -1;
    }

    struct lexer *lexer = lexer_new();
    enum parser_status status;
    struct ast *ast = parse(&status, lexer);
    (*ast->ftable->free)(ast);
    /*
    lexer->file = value;
    print_lex(lexer);
    // get the sh code

    if (value == NULL)
    {
        return -1;
    }
    // start of the process of lexing
    //lexer_file(value);
    */
    RETURN 0;
}
