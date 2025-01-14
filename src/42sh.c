#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"
#include "parser/parser.h"

static FILE *gere_usage(int argc, char *argv[])
{
    if (argc == 1)
    {
        return stdin;
    }
    else
    {
        int begin = 1;
        if (!strcmp(argv[1], "-p"))
        {
            if (argc < 3)
            {
                return NULL;
            }
            begin += 1;
        }
        else if (!strcmp(argv[1], "-l"))
        {
            if (argc < 3)
            {
                return NULL;
            }
            begin += 1;
        }
        int second = begin + 1;
        if (!strcmp(argv[begin], "-c"))
        {
            if (argc <= second)
            {
                return NULL;
            }
            return fmemopen(argv[second], (strlen(argv[second])), "r");
        }
        else
        {
            return fopen(argv[begin], "r");
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

    // init lexer
    struct lexer *lexer = lexer_new();
    lexer->file = value;
    int ret_code = 0;

    // launch parser
    enum parser_status status;
    if (argc > 1 && !strcmp(argv[1], "-l"))
    {
        print_lex(lexer);
        return 0;
    }
    struct ast *ast;
    while (lexer->current_tok.type != TOKEN_EOF)
    {
        ast = parse(&status, lexer);
        if (!ast)
        {
            if (status == PARSER_OK && lexer->current_tok.type != TOKEN_EOF)
            {
                continue;
            }
            if (status == PARSER_OK)
            {
                exit(0);
            }
            else
            {
                errx(2, "Wrong grammar");
            }
        }
        if (argc > 1 && !strcmp(argv[1], "-p"))
        {
            pretty_print_ast(ast);
        }
        else
        {
            ret_code = (*ast->ftable->run)(ast);
        }
        (*ast->ftable->free)(ast);
    }

    lexer_free(lexer);

    return ret_code;
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec_ast/exec.h"
#include "lexer/lexer.h"
#include "parser/ast.h"
#include "parser/parser.h"

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
