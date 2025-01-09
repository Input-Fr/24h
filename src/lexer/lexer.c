#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

struct lexer *lexer_new(const char *input)
{
    struct lexer *lex = malloc(sizeof(struct lexer));
    lex->input = input;
    lex->pos = 0;
    lex->end_of_token = 0;
    lex->length = strlen(input);
    return lex;
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

struct token token_reco(struct lexer *lexer)
{
    size_t index = lexer->pos;
    short isQuoting = 0;
    int token = 0;
    while (1)
    {
        if (lexer->input[index] == '\0')
        {
            // 1
            return uno(lexer, &token, &index);
        }
        else if (!isQuoting && lexer->input[index] == ';')
        {
            // 3
            return tres(lexer, &token, &index);
        }
        else if ((lexer->input[index] == '\\' || lexer->input[index] == '\''))
        {
            // 4
            isQuoting = isQuoting != 0 ? 0 : 1;
            lexer->end_of_token += 1;
            index++;
        }
        else if (!isQuoting && lexer->input[index] == '\n')
        {
            // 7
            return siete(lexer, &token, &index);
        }
        else if (!isQuoting && lexer->input[index] == ' ')
        {
            // 8
            if (token)
            {
                return ocho(lexer, &index);
            }
            else
            {
                lexer->pos += 1;
                lexer->end_of_token += 1;
                index += 1;
            }
        }
        else if (token)
        {
            index += 1;
        }
        else if (!isQuoting && lexer->input[index] == '#')
        {
            // 9
            return nueve(lexer, &index);
        }
        else
        {
            lexer->end_of_token += 1;
            index += 1;
            token = 1;
        }
    }
}

struct token lexer_next_token(struct lexer *lexer)
{
    struct token tok;
    tok = token_reco(lexer);
    lexer->pos = lexer->end_of_token + 1;
    lexer->end_of_token = lexer->pos;
    return tok;
}

struct token lexer_peek(struct lexer *lexer)
{
    size_t tmp = lexer->pos;
    size_t tmp2 = lexer->end_of_token;
    struct token tok = lexer_next_token(lexer);
    lexer->pos = tmp;
    lexer->end_of_token = tmp2;
    return tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    struct token tok = lexer_next_token(lexer);
    return tok;
}
