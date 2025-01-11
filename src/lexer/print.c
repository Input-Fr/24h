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
    while (i < 2)
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
        struct token tok2 = lexer_peek(lex);
        enum token_type type2 = tok2.type;
        char *string2 = tok2.data->str;

        if (type2 == TOKEN_EOF)
        {
            printf("EOF\n");
        }
        else
        {
            print(string2, type2);
        }


        struct token tok3 = lexer_pop(lex);
        enum token_type type3 = tok3.type;
        char *string3 = tok3.data->str;

        if (type3 == TOKEN_EOF)
        {
            printf("EOF\n");
        }
        else
        {
            print(string3, type3);
        }

        struct token tok4 = lexer_peek(lex);
        enum token_type type4 = tok4.type;
        char *string4 = tok4.data->str;

        if (type4 == TOKEN_EOF)
        {
            printf("EOF\n");
        }
        else
        {
            print(string4, type4);
        }

    mbt_str_free(lex->current_tok.data);
    free(lex);
}
