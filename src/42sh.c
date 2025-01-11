#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/ast.h"


static FILE * gere_usage(int argc, char *argv[])
{
    if (argc == 1)
    {
        return stdin;
    }
    else
    {
        if (!strcmp(argv[1],"-c"))
        {
            if (argc <= 2)
            {
                return NULL;
            }
            return fmemopen(argv[2], (strlen(argv[2]) + 1),"r");
        }
        else
        {
            return fopen(argv[1],"r");
        }
    }

}

char lexer_file (FILE * hd)
{
    return fgetc(hd);
}

int main(int argc, char *argv[])
{
    FILE * value = gere_usage(argc,argv);
    if (!value)
    {
        return -1;
    }

    // init lexer
    struct lexer *lexer = lexer_new();
    lexer->file = value;

    // launch parser
    enum parser_status status;
    struct ast *ast = parse(&status, lexer);
    (*ast->ftable->free)(ast);

    return 0;
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/ast.h"
#include "exec_ast/exec.h"

void given_string(char *input)
{
    if (input)
    {
        return;
    }
}

int main(int argc, char *argv[])
{
    if (argc == 3 && !strcmp("-c", argv[1]))
    {
        given_string(argv[2]);
    }

    // TEST DE PARSER
    //
    //const char* string = "if t; then t fi";
    //const char* string = "if true; then t else echo caca fi";
    //const char* string = "ls";
    const char* string = "if echo toto then if echo tata then echo toto fi fi";
    //
    struct lexer *lex = lexer_new(string);
    struct ast *ast;
    enum parser_status status;
    ast = parse(&status, lex);
    ast_free(ast);

    // TEST DE LEXER 
    //
    //const char* string = "#if world \n then hello";
    //const char* string = "if 'world then hello'";
    //const char* string = "if world ; then hello";

    //const char* string = "if 'world' then hello";
    //const char* string = "'if' world 'then hello'";
    //const char* string = "if world 'then' hello";
    //const char* string = "if world then 'hello'";

    //const char* string = "if world;\nthen hello";
    //const char* string = "if world then hello";
    //const char* string = "if #world then\nhello";
    //const char* string = "if #world then hello";
    //const char* string = "if world then hello";

    struct lexer *lex = lexer_new(string);
    if (argc > 1)
    {
        if (strcmp(argv[1],"--print") == 0)
        {
            print(lex);
        }

        if (strcmp(argv[1],"--pretty") == 0)
        {
            pretty_print(lex);
        }
    }

    free(lex);

    return 0;
}
*/
