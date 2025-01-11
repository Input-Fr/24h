#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

int test_if(struct lexer *lexer)
{
    char *str = lexer->current_tok.data->str;
    if (lexer->current_tok.data->size == 2 && str[0] == 'i' && str[1] == 'f')
    {
        return 1;
    }
    return 0;
}

int test_fi(struct lexer *lexer)
{
    char *str = lexer->current_tok.data->str;
    if (lexer->current_tok.data->size == 2 && str[0] == 'f' && str[1] == 'i')
    {
        return 1;
    }
    return 0;
}

int test_then(struct lexer *lexer)
{
    char *str = lexer->current_tok.data->str;
    if (lexer->current_tok.data->size == 4 && str[0] == 't' && str[1] == 'h'
        && str[2] == 'e' && str[3] == 'n')
    {
        return 1;
    }
    return 0;
}

int test_elif(struct lexer *lexer)
{
    char *str = lexer->current_tok.data->str;
    if (lexer->current_tok.data->size == 4 && str[0] == 'e' && str[1] == 'l'
        && str[2] == 'i' && str[3] == 'f')
    {
        return 1;
    }
    return 0;
}

int test_else(struct lexer *lexer)
{
    char *str = lexer->current_tok.data->str;
    if (lexer->current_tok.data->size == 4 && str[0] == 'e' && str[1] == 'l'
        && str[2] == 's' && str[3] == 'e')
    {
        return 1;
    }
    return 0;
}

enum token_type reserved_word(struct lexer *lexer)
{
    if (lexer->current_tok.type == TOKEN_QUOTE)
    {
        return TOKEN_WORD;
    }
    else
    {
        if (test_if(lexer))
        {
            return TOKEN_IF;
        }
        else if (test_fi(lexer))
        {
            return TOKEN_FI;
        }
        else if (test_elif(lexer))
        {
            return TOKEN_ELIF;
        }
        else if (test_else(lexer))
        {
            return TOKEN_ELSE;
        }
        else if (test_then(lexer))
        {
            return TOKEN_THEN;
        }
    }
    return TOKEN_WORD;
}
