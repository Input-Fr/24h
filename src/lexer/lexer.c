#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lexer *lexer_new(void)
{
    struct lexer *lex = malloc(sizeof(struct lexer));
    lex->input = '\0';
    struct token tok;
    tok.type = NO_TOKEN;
    tok.data = NULL;
    lex->current_tok = tok;
    lex->Quoting = NO_QUOTE;
    lex->peek = 0;
    return lex;
}

void lexer_free(struct lexer *lexer)
{
    if (lexer->current_tok.data != NULL)
    {
        free(lexer->current_tok.data); // free the mbt_str
    }
    free(lexer); // free the lexer
}

struct token lexer_peek(struct lexer *lexer)
{
    if (!lexer->peek)
    {
        lexer->peek = 1;
        return lexer_next_token(lexer);
    }
    else
    {
        return lexer->current_tok;
    }
}

struct token lexer_pop(struct lexer *lexer)
{
    if (!lexer->peek)
    {
        return lexer_next_token(lexer);
    }
    else
    {
        lexer->peek = 0;
        return lexer->current_tok;
    }
}
