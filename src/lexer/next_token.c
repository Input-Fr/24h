#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static struct token token_reco(struct lexer *lexer);
static struct token continue_word(struct lexer *lexer);
static struct token begin_word(struct lexer *lexer);


static struct token end_of_file(struct lexer *lexer)
{
    // --1
    if (lexer->word || lexer->ope)
    {
        if (lexer->Quoting == SINGLE_QUOTE ||
                lexer->Quoting == DOUBLE_QUOTE)
        {
            exit(2);
        }
        ungetc(EOF, lexer->file);
        if (lexer->current_tok.type == TOKEN_WORD)
        {
            lexer->current_tok.type = reserved_word(lexer);
        }
        return lexer->current_tok;
    }
    else
    {
        lexer->current_tok.type = TOKEN_EOF;
        return lexer->current_tok;
    }
}

static struct token next_quote(struct lexer *lexer)
{
    // --2
    mbt_str_pushc(lexer->current_tok.data, lexer->input);
    return token_reco(lexer);
}

static struct token operator(struct lexer *lexer)
{
    // --3
    ungetc(lexer->input, lexer->file);
    return lexer->current_tok;
}

static void single_quote(struct lexer *lexer)
{
    char c = lexer->input;
    if (lexer->Quoting == NO_QUOTE && c == '\'')
    {
        lexer->word = 1;
        lexer->current_tok.type = TOKEN_WORD;
        lexer->current_tok.data = mbt_str_init();
        mbt_str_pushc(lexer->current_tok.data, '\'');
        lexer->Quoting = SINGLE_QUOTE;
    }
    else if (lexer->Quoting == SINGLE_QUOTE && c == '\'')
    {
        mbt_str_pushc(lexer->current_tok.data, '\'');
        lexer->Quoting = NO_QUOTE;
    }
}

static void double_quote(struct lexer *lexer)
{
    char c = lexer->input;
    if (lexer->Quoting == NO_QUOTE && c == '"')
    {
        lexer->word = 1;
        lexer->current_tok.type = TOKEN_WORD;
        lexer->current_tok.data = mbt_str_init();
        mbt_str_pushc(lexer->current_tok.data, '"');
        lexer->Quoting = DOUBLE_QUOTE;
    }
    else if (lexer->Quoting == DOUBLE_QUOTE && c == '"')
    {
        lexer->Quoting = NO_QUOTE;
        mbt_str_pushc(lexer->current_tok.data, '"');
    }
}

static void backslash_quote(struct lexer *lexer)
{
    char c = lexer->input;
    lexer->input = lexer_file(lexer->file);
    char next_c = lexer->input;
    if (lexer->Quoting == NO_QUOTE && c == '\\')
    {
        if (next_c != '\n')
        {
            if (lexer->word)
            {
                mbt_str_pushc(lexer->current_tok.data, next_c);
            }
            else
            {
                lexer->word = 1;
                lexer->current_tok.data = mbt_str_init();
                mbt_str_pushc(lexer->current_tok.data, next_c);
            }
        }
    }
    else if (lexer->Quoting == DOUBLE_QUOTE)
    {
        if (next_c == '$' || next_c == '`' || next_c == '"' || next_c == '\\')
        {
            mbt_str_pushc(lexer->current_tok.data, next_c);
        }
        else if (next_c != '\n')
        {
            mbt_str_pushc(lexer->current_tok.data, '\\');
            mbt_str_pushc(lexer->current_tok.data, next_c);
        }
    }
    else
    {
        mbt_str_pushc(lexer->current_tok.data, '\\');
        mbt_str_pushc(lexer->current_tok.data, next_c);
    }
}

static struct token quote(struct lexer *lexer)
{
    // --4
    char c = lexer->input;
    if (c == '\'') 
    {
        single_quote(lexer);
    }
    else if (c == '"') 
    {
        double_quote(lexer);
    }
    else if (c == '\\')
    {
        backslash_quote(lexer);
    }

    return token_reco(lexer);
}

static struct token var(struct lexer *lexer)
{
    //--5
    if (!(lexer->word))
    {
        lexer->current_tok.data = mbt_str_init();
        lexer->word = 1;
    }

    char c = lexer->input;
    mbt_str_pushc(lexer->current_tok.data, c);
    c = lexer_file(lexer->file);
    mbt_str_pushc(lexer->current_tok.data, c);
    size_t count = 0;
    if (c == '{')
    {
        while (1)
        {
            c = lexer_file(lexer->file);
            mbt_str_pushc(lexer->current_tok.data, c);
            if (c == '{')
            {
                count += 1;
            }
            if (c == '}' && count == 0)
            {
                break;
            }
            else if (c == '}')
            {
                count -= 1;
            }
        }
    }
    if (c == '(')
    {
        while (1)
        {
            c = lexer_file(lexer->file);
            mbt_str_pushc(lexer->current_tok.data, c);
            if (c == '(')
            {
                count += 1;
            }
            if (c == ')' && count == 0)
            {
                break;
            }
            else if (c == ')')
            {
                count -= 1;
            }
        }
    }
    return token_reco(lexer);
}

static struct token begin_ope(struct lexer *lexer)
{
    // --6
    if (lexer->word)
    {
        ungetc(lexer->input, lexer->file);
        lexer->current_tok.type = reserved_word(lexer);
        return lexer->current_tok;
    }
    else
    {
        lexer->ope = 1;
        lexer->current_tok.data = mbt_str_init();
        operator_1(lexer);
        mbt_str_pushc(lexer->current_tok.data, lexer->input);
        return token_reco(lexer);
    }
}

static struct token new_line(struct lexer *lexer)
{
    // --7

    if (lexer->word)
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

static struct token blank(struct lexer *lexer)
{
    // --8
    if (lexer->word)
    {
        lexer->current_tok.type = reserved_word(lexer);
        return lexer->current_tok;
    }
    else
    {
        return token_reco(lexer);
    }
}

static struct token continue_word(struct lexer *lexer)
{
    // --9
    mbt_str_pushc(lexer->current_tok.data, lexer->input);
    return token_reco(lexer);
}

static struct token com(struct lexer *lexer)
{
    // --10
    lexer->current_tok.type = TOKEN_COM;
    while (1)
    {
        char co = fgetc(lexer->file);
        if (co == '\0' || co == EOF)
        {
            ungetc(EOF, lexer->file);
            return lexer->current_tok;
        }
        else if (co == '\n')
        {
            ungetc('\n', lexer->file);
            return lexer->current_tok;
        }
    }
}

static struct token begin_word(struct lexer *lexer)
{
    // --11
    lexer->word = 1;
    lexer->current_tok.data = mbt_str_init();
    mbt_str_pushc(lexer->current_tok.data, lexer->input);
    lexer->current_tok.type = TOKEN_WORD;
    return token_reco(lexer);
}

static struct token token_reco(struct lexer *lexer)
{
    lexer->input = lexer_file(lexer->file);
    char c = lexer->input;
    //printf("c : %c\n",c);
    if (c == EOF || c == '\0')
    {
        return end_of_file(lexer);   // 1
    }
    else if ((lexer->Quoting == NO_QUOTE
                && lexer->ope && test_operator(lexer)))
    {
        return next_quote(lexer);    // 2
    }
    else if (lexer->ope && !test_operator(lexer))
    {
        return operator(lexer);      // 3
    }
    else if ((c == '\\') || c == '\'' || c == '"')
    {
        return quote(lexer);         // 4
    }
    else if (lexer->Quoting == NO_QUOTE && c == '$') // || c == '`')
    {
        return var(lexer);           // 5
    }
    else if (lexer->Quoting == NO_QUOTE && test_operator_1(lexer)) // 6
    {
        return begin_ope(lexer);     // 6
    }
    else if (lexer->Quoting != SINGLE_QUOTE && lexer->Quoting != DOUBLE_QUOTE
            && c == '\n')
    {
        return new_line(lexer);      // 7
    }
    else if ((lexer->Quoting == NO_QUOTE && (c == ' ' || c == '\t')))
    {
        return blank(lexer);         // 8
    }
    else if (lexer->word)
    {
        return continue_word(lexer); // 9
    }
    else if (lexer->Quoting == NO_QUOTE && c == '#')
    {
        return com(lexer);           // 10
    }
    else
    {
        return begin_word(lexer);    // 11
    }
}

struct token lexer_next_token(struct lexer *lexer)
{
    clear_current_tok(lexer);
    lexer->word = 0;
    lexer->ope = 0;
    return token_reco(lexer);
}
