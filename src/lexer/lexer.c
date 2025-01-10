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
    tok.data = mbt_str_init();
    lex->current_tok = tok;
    lex->Quoting = NO_QUOTE;
    lex->peek = 0;
    return lex;
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

struct token lexer_next_token(struct lexer *lexer)
{
    clear_current_tok(lexer);
    int token = 0;
    while (1)
    {
        char c = lexer_file(lexer->file);
        if (c == '\0')
        {
            // 1
            if (token)
            {
                lexer->current_tok.type = TOKEN_WORD;
                ungetc('\0', lexer->file);
                return lexer->current_tok;
            }
            else
            {
                lexer->current_tok.type = TOKEN_EOF;
                return lexer->current_tok;
            }

        }
        else if (lexer->Quoting == NO_QUOTE && c == ';') //changer
        {
            // 3
            if (token)
            {
                lexer->current_tok.type = TOKEN_WORD;
                ungetc(';', lexer->file);
                return lexer->current_tok;
            }
            else
            {
                lexer->current_tok.type = TOKEN_SEMI;
                return lexer->current_tok;
            }
        }
        else if ((c == '\\') || c == '\'' || c == '"') //cas \n
        {
            // 4
            if (c == '\'')  //(&& lexer->input[index] == ' ' || lexer->input[index] == ';')
            {
                if (lexer->Quoting == NO_QUOTE && c == '\'')
                {
                    lexer->Quoting = SINGLE_QUOTE;
                }
                else if (lexer->Quoting == SINGLE_QUOTE && c == '\'')
                {
                    lexer->Quoting = NO_QUOTE;
                }
            }

            if (c == '"')  //(&& lexer->input[index] == ' ' || lexer->input[index] == ';')
            {
                if (lexer->Quoting == NO_QUOTE && c == '"')
                {
                    lexer->Quoting = DOUBLE_QUOTE;
                }
                else if (lexer->Quoting == DOUBLE_QUOTE && c == '"')
                {
                    lexer->Quoting = NO_QUOTE;
                }
            }

            if (c == '\\')
            {
                if (lexer->Quoting == NO_QUOTE && c == '\\')
                {
                    lexer->Quoting = BACKSLASH_QUOTE;
                }
            }

            //lexer->current_tok->str = lexer->input;
        }
        else if (lexer->Quoting == NO_QUOTE && c == '\n')
        {
            // 7
            if (token)
            {
                lexer->current_tok.type = TOKEN_WORD;
                ungetc('\n', lexer->file);
                return lexer->current_tok;
            }
            else
            {
                lexer->current_tok.type = TOKEN_NEWLINE;
                return lexer->current_tok;
            }
        }
        else if ((lexer->Quoting == NO_QUOTE && c == ' '))
        {
            // 8
            if (token)
            {
                lexer->current_tok.type = TOKEN_WORD;
                return lexer->current_tok;
            }
        }
        else if (token)
        {
            mbt_str_pushc(lexer->current_tok.data,c);
        }
        else if (lexer->Quoting == NO_QUOTE && c == '#')
        {
            // 9
            mbt_str_pushc(lexer->current_tok.data, c);
            lexer->current_tok.type = TOKEN_COM;
            while (1)
            {
                char co = fgetc(lexer->file);
                if (co == '\0')
                {
                    ungetc('\0', lexer->file);
                    return lexer->current_tok;
                }
                else if (co == '\n')
                {
                    ungetc('\n', lexer->file);
                    return lexer->current_tok;
                }
                mbt_str_pushc(lexer->current_tok.data, co);
            }

        }
        else
        {
            mbt_str_pushc(lexer->current_tok.data, c);
            token = 1;
            if (lexer->Quoting == BACKSLASH_QUOTE)
            {
                lexer->Quoting = NO_QUOTE;
            }
        }
    }
}

void reserved_word(struct lexer *lexer)
{
    if (lexer->current_tok.type == TOKEN_WORD)
    {
        if (test_if(lexer))
        {
            lexer->current_tok.type = TOKEN_IF;
        }
        else if (test_fi(lexer))
        {
            lexer->current_tok.type = TOKEN_FI;
        }
        else if (test_elif(lexer))
        {
            lexer->current_tok.type = TOKEN_ELIF;
        }
        else if (test_else(lexer))
        {
            lexer->current_tok.type = TOKEN_ELSE;
        }
        else if (test_then(lexer))
        {
            lexer->current_tok.type = TOKEN_THEN;
        }
    }
}



struct token lexer_peek(struct lexer *lexer)
{
    reserved_word(lexer);
    if (!lexer->peek)
    {
        lexer->peek = 1;
        lexer_next_token(lexer);
        return lexer->current_tok;
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
        lexer_next_token(lexer);
        reserved_word(lexer);
        return lexer->current_tok;
    }
    else
    {
        lexer->peek = 0;
        return lexer->current_tok;
    }
}
