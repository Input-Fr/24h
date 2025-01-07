#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include <string.h>

void print(struct lexer *lex)
{
    while ((lexer_peek(lex)).type != TOKEN_EOF)
    {
        printf("%s\n", lexer_pop(lex).str);
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
