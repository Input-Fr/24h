#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"


void print_lex(struct lexer *lex)
{
    int i = 0;
    while (1)
    {
        struct token tok = lexer_pop(lex);
        enum token_type type = tok.type;
        if (type == TOKEN_EOF)
        {
            printf("EOF\n");
            break;
        }
        char *string = tok.data->str;
        if (type == TOKEN_WORD)
        {
            printf("wd : %s\n", string);
        }
        else if (type == TOKEN_COM)
        {
            printf("co : %s\n", string);
        }
        else if (type == TOKEN_IF || type == TOKEN_THEN || type == TOKEN_ELIF
                 || type == TOKEN_ELSE || type == TOKEN_FI)
        {
            printf("rw : %s\n", string);
        }
        else if (type == TOKEN_SEMI)
        {
            printf("se : ;\n");
        }
        else if (type == TOKEN_NEWLINE)
        {
            printf("nl : newline\n");
        }
        else
        {
            printf("? : %s\n", string);
        }
        mbt_str_free(lex->current_tok.data);
        lex->current_tok.data = mbt_str_init();
        i += 1;
    }
    mbt_str_free(lex->current_tok.data);
    free(lex);
}

void print_lex_peek3(struct lexer *lex)
{
    int i = 0;
    while (i < 4)
    {

        struct token tok = lexer_peek(lex);
        enum token_type type = tok.type;
        if (type == TOKEN_EOF)
        {
            printf("EOF\n");
            break;
        }
        char *string = tok.data->str;
        if (type == TOKEN_WORD)
        {
            printf("wd : %s\n", string);
        }
        else if (type == TOKEN_COM)
        {
            printf("co : %s\n", string);
        }
        else if (type == TOKEN_IF || type == TOKEN_THEN || type == TOKEN_ELIF
                 || type == TOKEN_ELSE || type == TOKEN_FI)
        {
            printf("rw : %s\n", string);
        }
        else if (type == TOKEN_SEMI)
        {
            printf("se : ;\n");
        }
        else if (type == TOKEN_NEWLINE)
        {
            printf("nl : newline\n");
        }
        else
        {
            printf("? : %s\n", string);
        }
        //mbt_str_free(lex->current_tok.data);
        //lex->current_tok.data = mbt_str_init();
        i += 1;
    }
    mbt_str_free(lex->current_tok.data);
    free(lex);
}

void print_lex_pop3(struct lexer *lex)
{
    int i = 0;
    while (i < 4)
    {

        struct token tok = lexer_pop(lex);
        enum token_type type = tok.type;
        if (type == TOKEN_EOF)
        {
            printf("EOF\n");
            break;
        }
        char *string = tok.data->str;
        if (type == TOKEN_WORD)
        {
            printf("wd : %s\n", string);
        }
        else if (type == TOKEN_COM)
        {
            printf("co : %s\n", string);
        }
        else if (type == TOKEN_IF || type == TOKEN_THEN || type == TOKEN_ELIF
                 || type == TOKEN_ELSE || type == TOKEN_FI)
        {
            printf("rw : %s\n", string);
        }
        else if (type == TOKEN_SEMI)
        {
            printf("se : ;\n");
        }
        else if (type == TOKEN_NEWLINE)
        {
            printf("nl : newline\n");
        }
        else
        {
            printf("? : %s\n", string);
        }
        //mbt_str_free(lex->current_tok.data);
        //lex->current_tok.data = mbt_str_init();
        i += 1;
    }
    mbt_str_free(lex->current_tok.data);
    free(lex);
}
