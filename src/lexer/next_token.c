#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

struct token rule_one(struct lexer *lexer, int *token)
{
    // 1
    if (*token)
    {
        ungetc(EOF, lexer->file);
        lexer->current_tok.type = reserved_word(lexer);
        return lexer->current_tok;
    }
    else
    {
        lexer->current_tok.type = TOKEN_EOF;
        return lexer->current_tok;
    }
}

struct token rule_three(struct lexer *lexer, int *token)
{
    // 3
    if (*token)
    {
        ungetc(';', lexer->file);
        lexer->current_tok.type = reserved_word(lexer);
        return lexer->current_tok;
    }
    else
    {
        lexer->current_tok.type = TOKEN_SEMI;
        return lexer->current_tok;
    }
}

void rule_four(struct lexer *lexer)
{
    char c = lexer->input;
    if (c == '\'') //(&& lexer->input[index] == ' ' ||
                   // lexer->input[index] == ';')
    {
        if (lexer->Quoting == NO_QUOTE && c == '\'')
        {
            lexer->Quoting = SINGLE_QUOTE;
        }
        else if (lexer->Quoting == SINGLE_QUOTE && c == '\'')
        {
            lexer->current_tok.type = TOKEN_QUOTE;
            lexer->Quoting = NO_QUOTE;
        }
    }

    if (c == '"') //(&& lexer->input[index] == ' ' ||
                  // lexer->input[index] == ';')
    {
        if (lexer->Quoting == NO_QUOTE && c == '"')
        {
            lexer->Quoting = DOUBLE_QUOTE;
        }
        else if (lexer->Quoting == DOUBLE_QUOTE && c == '"')
        {
            lexer->current_tok.type = TOKEN_QUOTE;
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
}

struct token rule_seven(struct lexer *lexer, int *token)
{
    if (*token)
    {
        ungetc('\n', lexer->file);
        lexer->current_tok.type = reserved_word(lexer);
        return lexer->current_tok;
    }
    else
    {
        lexer->current_tok.type = TOKEN_NEWLINE;
        return lexer->current_tok;
    }
}

struct token rule_eight(struct lexer *lexer)
{
    lexer->current_tok.type = reserved_word(lexer);
    return lexer->current_tok;
}

struct token rule_nine(struct lexer *lexer)
{
    mbt_str_pushc(lexer->current_tok.data, lexer->input);
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

struct token lexer_next_token(struct lexer *lexer)
{
    clear_current_tok(lexer);
    int token = 0;
    while (1)
    {
        lexer->input = lexer_file(lexer->file);
        char c  = lexer->input;

        if (c == EOF)
        {
            // 1
            return rule_one(lexer, &token);
        }
        else if (lexer->Quoting == NO_QUOTE && c == ';') // changer
        {
            // 3
            return rule_three(lexer, &token);
        }
        else if ((c == '\\') || c == '\'' || c == '"') // cas \n
        {
            // 4
            rule_four(lexer);
        }
        else if (lexer->Quoting == NO_QUOTE && c == '\n')
        {
            // 7
            return rule_seven(lexer, &token);
        }
        else if ((lexer->Quoting == NO_QUOTE && c == ' ') && token)
        {
            // 8
            return rule_eight(lexer);
        }
        else if (token)
        {
            mbt_str_pushc(lexer->current_tok.data, c);
        }
        else if (lexer->Quoting == NO_QUOTE && c == '#')
        {
            // 9
            return rule_nine(lexer);
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
