#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

void print(char *string, enum token_type type)
{
    if (type == TOKEN_WORD)
    {
        printf("wd : %s\n", string);
    }
    else if (type == TOKEN_COM)
    {
        printf("co : %s\n", string);
    }
    else if (type == TOKEN_IF || type == TOKEN_THEN || type == TOKEN_ELIF
             || type == TOKEN_ELSE || type == TOKEN_FI)
    {
        printf("rw : %s\n", string);
    }
    else if (type == TOKEN_SEMI)
    {
        printf("se : ;\n");
    }
    else if (type == TOKEN_NEWLINE)
    {
        printf("nl : newline\n");
    }
    else
    {
        printf("? : %s\n", string);
    }
}

void print_lex(struct lexer *lex)
{
    int i = 0;
    while (1)
    {
        struct token tok1 = lexer_pop(lex);
        enum token_type type1 = tok1.type;
        char *string1 = tok1.data->str;

        if (type1 == TOKEN_EOF)
        {
            printf("EOF\n");
            break;
        }
        print(string1, type1);
        i += 1;
    }
}
