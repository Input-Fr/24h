#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

struct token uno(struct lexer *lexer, int *token, size_t *index)
{
    if (*token)
    {
        lexer->end_of_token = *index - 1;
        return word(lexer);
    }
    else
    {
        lexer->end_of_token = *index;
        return token_EOF();
    }
}

struct token tres(struct lexer *lexer, int *token, size_t *index)
{
    if (*token)
    {
        lexer->end_of_token = *index - 1;
        return word(lexer);
    }
    else
    {
        lexer->end_of_token = *index;
        return token_semi();
    }
}

struct token siete(struct lexer *lexer, int *token, size_t *index)
{
    if (*token)
    {
        lexer->end_of_token = *index - 1;
        return word(lexer);
    }
    else
    {
        lexer->end_of_token = *index;
        return token_newline();
    }
}

struct token ocho(struct lexer *lexer, size_t *index)
{
    lexer->end_of_token = *index - 1;
    return word(lexer);
}

struct token nueve(struct lexer *lexer, size_t *index)
{
    while (lexer->input[*index] != '\0' && lexer->input[*index] != '\n')
    {
        lexer->end_of_token += 1;
        *index += 1;
    }

    if (lexer->input[*index] == '\n')
    {
        lexer->end_of_token = *index;
        return token_com(lexer);
    }
    else
    {
        // errx(2, "Syntax error");
    }
    return token_com(lexer);
}
