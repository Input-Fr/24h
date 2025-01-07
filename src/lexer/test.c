#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include <string.h>

int test_if(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'i'
            && lexer->input[index + 1] == 'f')
    {
        return 1;
    }

    return 0;
}

int test_fi(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'f'
            && lexer->input[index + 1] == 'i')
    {
        return 1;
    }
    return 0;
}

int test_then(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 't'
            && lexer->input[index + 1] == 'h'
            && lexer->input[index + 2] == 'e' && lexer->input[index + 3] == 'n')
    {
        return 1;
    }
    return 0;
}

int test_elif(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'e'
            && lexer->input[index + 1] == 'l'
            && lexer->input[index + 2] == 'i' && lexer->input[index + 3] == 'f')
    {
        return 1;
    }
    return 0;

}

int test_else(struct lexer *lexer)
{
    size_t index = lexer->pos;
    if (lexer->input[index] == 'e'
            && lexer->input[index + 1] == 'l'
            && lexer->input[index + 2] == 's' && lexer->input[index + 3] == 'e')
    {
        return 1;
    }
    return 0;
}
