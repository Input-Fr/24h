#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include <string.h>

void print(struct lexer *lex)
{
    while (1)
    {
        struct token tok = lexer_pop(lex);
        enum token_type type = tok.type;
        if (type == TOKEN_EOF)
        {
            printf("EOF\n");
            break;
        }
        char *string = tok.str;
        if (type == TOKEN_WORD)
        {
            printf("wd : %s\n", string);
            free(tok.str);
        }
        else if (type == TOKEN_COM)
        {
            printf("co : %s\n", string);
            free(tok.str);
        }
        else if (type == TOKEN_QUO)
        {
            printf("qo : %s\n", string);
            free(tok.str);
        }
        else if (type == TOKEN_IF || type == TOKEN_THEN || type == TOKEN_ELIF
                || type == TOKEN_ELSE || type == TOKEN_FI)
        {
            printf("rw : %s\n", string);
        }
        else if (type == TOKEN_SEMI)
        {
            printf("se: %s\n", string);
        }
        else
        {
            printf("? : %s\n", string);
        }

    }

}

void pretty_print(struct lexer *lex)
{
    while ((lexer_peek(lex)).type != TOKEN_EOF)
    {
        struct token tokpeek = lexer_pop(lex);
        if (tokpeek.type == TOKEN_IF)
        {
            printf("%s {", tokpeek.str);
            //lexer_pop(lex);
            while (lexer_pop(lex).type != TOKEN_SEMI && lexer_peek(lex).type != TOKEN_EOF)
            {
                if (tokpeek.type == TOKEN_SEMI)
                {
               //     printf(" %s ", lexer_peek(lex).str);
                }
            }
            printf(" }; ");
        }

        if (tokpeek.type == TOKEN_THEN)
        {
            printf("%s {", tokpeek.str);
            lexer_pop(lex);
            while (lexer_pop(lex).type != TOKEN_FI && lexer_peek(lex).type != TOKEN_EOF)
            {
                if (tokpeek.type == TOKEN_SEMI)
                {
                //    printf(" %s ", lexer_peek(lex).str);
                }
            }
            printf(" }; %s ", lexer_peek(lex).str);
        }
    }

    printf(" }; %s \n", lexer_peek(lex).str);
}
