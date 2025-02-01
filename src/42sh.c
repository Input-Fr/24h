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

    if (argc > 1 && !strcmp(argv[1], "-l"))
    {
        print_lex(lexer);
        return 0;
    }

    int ret_code = 0;
    enum parser_status status;
    struct ast *ast;
    struct hash_map *h = hash_map_init(7);
    char *ifs = calloc(1, 20);
    ifs[0] = 'I';
    ifs[1] = 'F';
    ifs[2] = 'S';
    ifs[3] = '\0';

    char *ifs2 = calloc(1, 20);
    ifs2[0] = ' ';
    ifs2[1] = '\t';
    ifs2[2] = '\n';
    ifs2[3] = '\0';
    hash_map_insert(h, ifs, ifs2, VARIABLE);
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

static void print(char *string, enum token_type type)
{
    if (type == TOKEN_WORD)
        printf("wd: %s\n", string);
    else if (type == TOKEN_ASSIGNMENT_WORD)
        printf("aw: %s\n", string);
    else if (type == TOKEN_COM)
        printf("co: ...\n");
    else if (type == TOKEN_IF)
        printf("rw: if\n");
    else if (type == TOKEN_THEN)
        printf("rw: then\n");
    else if (type == TOKEN_ELIF)
        printf("rw: elif\n");
    else if (type == TOKEN_ELSE)
        printf("rw: else\n");
    else if (type == TOKEN_FI)
        printf("rw: fi\n");
    else if (type == TOKEN_DO)
        printf("rw: do\n");
    else if (type == TOKEN_DONE)
        printf("rw: done\n");
    else if (type == TOKEN_CASE)
        printf("rw: case\n");
    else if (type == TOKEN_ESAC)
        printf("rw: esac\n");
    else if (type == TOKEN_WHILE)
        printf("rw: while\n");
    else if (type == TOKEN_UNTIL)
        printf("rw: until\n");
    else if (type == TOKEN_FOR)
        printf("rw: for\n");
    else if (type == TOKEN_SEMI)
        printf("se: ;\n");
    else if (type == TOKEN_PIPE)
        printf("rw: |\n");
    else if (type == TOKEN_NEWLINE)
        printf("nl: newline\n");
    else if (type == TOKEN_AND)
        printf("op: &\n");
    else if (type == TOKEN_AND_IF)
        printf("op: &&\n");
    else if (type == TOKEN_OR_IF)
        printf("op: ||\n");
    else if (type == TOKEN_DSEMI)
        printf("op: ;;\n");
    else if (type == TOKEN_LESS)
        printf("op: <\n");
    else if (type == TOKEN_GREAT)
        printf("op: >\n");
    else if (type == TOKEN_DLESS)
        printf("op: <<\n");
    else if (type == TOKEN_DGREAT)
        printf("op: >>\n");
    else if (type == TOKEN_LESSAND)
        printf("op: <&\n");
    else if (type == TOKEN_GREATAND)
        printf("op: >&\n");
    else if (type == TOKEN_LESSGREAT)
        printf("op: <>\n");
    else if (type == TOKEN_DLESSDASH)
        printf("op: <<-\n");
    else if (type == TOKEN_CLOBBER)
        printf("op: >|\n");
    else if (type == TOKEN_LBRACE)
        printf("rw: {\n");
    else if (type == TOKEN_RBRACE)
        printf("rw: }\n");
    else if (type == TOKEN_LPAR)
        printf("rw: (\n");
    else if (type == TOKEN_RPAR)
        printf("rw: )\n");
    else if (type == TOKEN_BANG)
        printf("rw: !\n");
    else if (type == TOKEN_IN)
        printf("rw: in\n");
    else
        printf("?:\n");
}

void print_lex(struct lexer *lexer)
{
    int i = 0;
    while (1)
    {
        struct token tok = lexer_pop(lexer);

        enum token_type type = tok.type;
        char *string = "";
        if (tok.data != NULL)
        {
            string = tok.data->str;
        }
        if (type == TOKEN_EOF)
        {
            printf("EOF\n");
            break;
        }
        print(string, type);
        if (type == TOKEN_WORD)
        {
            free(string);
        }
        i += 1;
    }
    lexer_free(lexer);
}
