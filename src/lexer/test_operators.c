#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

void operator_1(struct lexer *lexer)
{
    char c = lexer->input;
    if (c == ';')
    {
        lexer->current_tok.type = TOKEN_SEMI;
    }
    else if (c == '|')
    {
        lexer->current_tok.type = TOKEN_PIPE;
    }
    else if (c == '&')
    {
        lexer->current_tok.type = TOKEN_AND;
    }
    else if (c == '<')
    {
        lexer->current_tok.type = TOKEN_LESS;
    }
    else if (c == '>')
    {
        lexer->current_tok.type = TOKEN_GREAT;
    }
}

int test_operator_1(struct lexer *lexer)
{
    char c = lexer->input;
    if (c == ';' || c == '|' || c == '&' || c == '>' || c == '<')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int test_operator(struct lexer *lexer)
{
    char c = lexer->input;
    size_t length = lexer->current_tok.data->size;
    if (length == 1 && lexer->current_tok.data->str[0] == ';' && c == ';')
    {
        lexer->current_tok.type = TOKEN_DSEMI;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '&' && c == '&')
    {
        lexer->current_tok.type = TOKEN_AND_IF;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '|' && c == '|')
    {
        lexer->current_tok.type = TOKEN_OR_IF;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '>' && c == '>')
    {
        lexer->current_tok.type = TOKEN_DGREAT;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '<' && c == '<')
    {
        lexer->current_tok.type = TOKEN_DLESS;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '>' && c == '&')
    {
        lexer->current_tok.type = TOKEN_GREATAND;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '<' && c == '&')
    {
        lexer->current_tok.type = TOKEN_LESSAND;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '<' && c == '>')
    {
        lexer->current_tok.type = TOKEN_LESSGREAT;
    }
    else if (length == 2 && lexer->current_tok.data->str[0] == '<'
            && lexer->current_tok.data->str[0] == '<' && c == '-')
    {
        lexer->current_tok.type = TOKEN_DLESSDASH;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '>' && c == '|')
    {
        lexer->current_tok.type = TOKEN_CLOBBER;
    }
    else
    {
        return 0;
    }
    return 1;
}
