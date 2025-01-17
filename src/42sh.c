#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/hash_map/hash_map.h"

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
        else if (!strcmp(argv[1], "-h"))
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

int main(int argc, char **argv)
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
    ast = parse(&status, lexer);

    struct hash_map *h = hash_map_init(7);
    h->ret = ret_code;

    hash_map_insert(h, "abc", "1234");
    h->nb_args = argc - 1;
    h->all_args = argv;
    if (ast != NULL)
    {
        ret_code = (*ast->ftable->run)(ast,h);
        h->ret = ret_code;
    }

    if (argc > 1 && !strcmp(argv[1], "-h"))
    {
        hash_map_print(h);
    }

    (*ast->ftable->free)(ast);

    lexer_free(lexer);
    hash_map_free(h);

    return ret_code;
}
