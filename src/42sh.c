#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast.h"
#include "hash_map/hash_map.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

void print_lex(struct lexer *lexer);

static FILE *gere_usage(int argc, char *argv[])
{
    if (argc == 1)
    {
        return stdin;
    }
    else
    {
        int begin = 1;
        if (!strcmp(argv[1], "-l"))
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

static char *ifs1(void)
{
    char *ifs = calloc(1, 20);
    ifs[0] = 'I';
    ifs[1] = 'F';
    ifs[2] = 'S';
    ifs[3] = '\0';
    return ifs;
}

static char *ifs2(void)
{
    char *ifs2 = calloc(1, 20);
    ifs2[0] = ' ';
    ifs2[1] = '\t';
    ifs2[2] = '\n';
    ifs2[3] = '\0';
    return ifs2;
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
    enum parser_status status;
    struct ast *ast;
    struct hash_map *h = hash_map_init(7);
    hash_map_insert(h, ifs1(), ifs2(), VARIABLE);
    char *bufferpwd = malloc(1024 * sizeof(char));
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
                lexer_free(lexer);
                hash_map_free(h);
                free(bufferpwd);
                exit(ret_code);
            }
            else
            {
                errx(2, "Wrong grammar");
            }
        }
        h->ret = ret_code;
        h->nb_args = argc - 1;
        h->all_args = argv;
        h->old_pwd = getcwd(bufferpwd, 1024);
        ret_code = (*ast->ftable->run)(ast, h);

        (*ast->ftable->free)(ast);
    }

    lexer_free(lexer);
    hash_map_free(h);
    free(bufferpwd);

    return ret_code;
}
