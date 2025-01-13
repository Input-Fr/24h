#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

void print_reserved_word(enum token_type type)
{
    if (type == TOKEN_IF)
        printf("rw: if\n");
    if (type == TOKEN_THEN)
        printf("rw: then\n");
    if (type == TOKEN_ELIF)
        printf("rw: elif\n");
    if (type == TOKEN_ELSE)
        printf("rw: else\n");
    if (type == TOKEN_FI)
        printf("rw: fi\n");
}

void print(char *string, enum token_type type)
{
    if (type == TOKEN_WORD)
    {
        printf("wd: %s\n", string);
    }
    else if (type == TOKEN_COM)
    {
        printf("co: ...\n");
    }
    else if (type == TOKEN_IF || type == TOKEN_THEN || type == TOKEN_ELIF
             || type == TOKEN_ELSE || type == TOKEN_FI)
    {
        print_reserved_word(type);
    }
    else if (type == TOKEN_SEMI)
    {
        printf("se: ;\n");
    }
    else if (type == TOKEN_NEWLINE)
    {
        printf("nl: newline\n");
    }
    else
    {
        printf("?:");
    }
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
