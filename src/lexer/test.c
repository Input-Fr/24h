#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

int test_if(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'i' && lexer->input[index + 1] == 'f')
    {
        return 1;
    }

    return 0;
}

int test_fi(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'f' && lexer->input[index + 1] == 'i')
    {
        return 1;
    }
    return 0;
}

int test_then(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 't' && lexer->input[index + 1] == 'h'
        && lexer->input[index + 2] == 'e' && lexer->input[index + 3] == 'n')
    {
        return 1;
    }
    return 0;
}

int test_elif(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'e' && lexer->input[index + 1] == 'l'
        && lexer->input[index + 2] == 'i' && lexer->input[index + 3] == 'f')
    {
        return 1;
    }
    return 0;
}

int test_else(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'e' && lexer->input[index + 1] == 'l'
        && lexer->input[index + 2] == 's' && lexer->input[index + 3] == 'e')
    {
        return 1;
    }
    return 0;
}

int test_quo(struct lexer *lexer)
{
    if (lexer->input[lexer->pos] == '\''
        && lexer->input[lexer->end_of_token] == '\'')
    {
        return 1;
    }
    return 0;
}

int test_com(struct lexer *lexer)
{
    if (lexer->input[lexer->pos] == '#')
    {
        return 1;
    }
    return 0;
}

int test_EOF(struct lexer *lexer)
{
    if (lexer->input[lexer->pos] == '\0')
    {
        return 1;
    }
    return 0;
}

struct token word(struct lexer *lexer)
{
    if (test_if(lexer))
    {
        return token_if();
    }
    else if (test_fi(lexer))
    {
        return token_fi();
    }
    else if (test_elif(lexer))
    {
        return token_elif();
    }
    else if (test_else(lexer))
    {
        return token_else();
    }
    else if (test_then(lexer))
    {
        return token_then();
    }
    else if (test_quo(lexer))
    {
        return token_quo(lexer);
    }
    else
    {
        return token_word(lexer);
    }
}
