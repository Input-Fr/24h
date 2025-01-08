#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include <string.h>

struct token sem(void)
{
    struct token tok;
    tok.type = TOKEN_SEMI;
    tok.str = ";";
    return tok;
}

struct token token_if(void)
{
    struct token tok;
    tok.type = TOKEN_IF;
    tok.str = "if";
    return tok;
}

struct token token_fi(void)
{
    struct token tok;
    tok.type = TOKEN_FI;
    tok.str = "fi";
    return tok;
}

struct token token_then(void)
{
    struct token tok;
    tok.type = TOKEN_THEN;
    tok.str = "then";
    return tok;
}

struct token token_elif(void)
{
    struct token tok;
    tok.type = TOKEN_ELIF;
    tok.str = "elif";
    return tok;
}

struct token token_else(void)
{
    struct token tok;
    tok.type = TOKEN_ELSE;
    tok.str = "else";
    return tok;
}

char *str_word(struct lexer *lexer)
{
    size_t len = 0;
    size_t index = lexer->pos;
    while (index < lexer->end_of_token + 1)
    {
        len += 1;
        index+=1;
    }

    size_t i = 0;
    index = lexer->pos;
    char *string = calloc(sizeof(char), (len + 1));
    while (i < len)
    {
        string[i] = lexer->input[index];
        i += 1;
        index += 1;
    }
    string[i] = '\0';
    //printf("str : %s \n",string);
    return string;
}

struct token token_com(struct lexer *lexer)
{
    struct token tok;
    tok.type = TOKEN_COM;
    lexer->end_of_token -= 1;
    tok.str = str_word(lexer);
    lexer->end_of_token += 1;

    return tok;
}

struct token token_quo(struct lexer *lexer)
{
    struct token tok;
    tok.type = TOKEN_QUO;

    tok.str = str_word(lexer);

    return tok;
}

struct token token_word(struct lexer *lexer)
{
    struct token tok;
    tok.type = TOKEN_WORD;

    tok.str = str_word(lexer);

    return tok;
}

struct token token_semi(void)
{
    struct token tok;
    tok.type = TOKEN_SEMI;
    tok.str = ";";
    return tok;
}

struct token token_EOF(void)
{
    struct token tok;
    tok.type = TOKEN_EOF;
    tok.str = "EOF";
    return tok;
}

struct token token_error(void)
{
    struct token tok;
    tok.type = TOKEN_ERROR;
    tok.str = "ERROR";
    return tok;
}
