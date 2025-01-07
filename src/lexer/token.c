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

struct token token_error(void)
{
    struct token tok;
    tok.type = TOKEN_ERROR;
    tok.str = "ERROR";
    return tok;
}



