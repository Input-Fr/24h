#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../expand/expand.h"
#include "../parser/parser.h"
#include "lexer.h"

static struct token token_reco(struct lexer *lexer);
static struct token continue_word(struct lexer *lexer);
static struct token begin_word(struct lexer *lexer);

static struct token end_of_file(struct lexer *lexer)
{
    // --1
    if (lexer->word || lexer->ope)
    {
        if (lexer->Quoting == SINGLE_QUOTE || lexer->Quoting == DOUBLE_QUOTE)
            errx(2, "missing quote");

        if (lexer->current_tok.type == TOKEN_WORD)
            lexer->current_tok.type = reserved_word(lexer);

        ungetc(EOF, lexer->file);
        return lexer->current_tok;
    }
    else
    {
        lexer->current_tok.type = TOKEN_EOF;
        return lexer->current_tok;
    }
}

static struct token next_ope(struct lexer *lexer)
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
        if (!lexer->word)
        {
            lexer->word = 1;
            lexer->current_tok.data = mbt_str_init();
        }
        mbt_str_pushc(lexer->current_tok.data, '\'');
        lexer->Quoting = SINGLE_QUOTE;
        lexer->current_tok.type = TOKEN_WORD;
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
        if (!lexer->word)
        {
            lexer->word = 1;
            lexer->current_tok.data = mbt_str_init();
        }
        mbt_str_pushc(lexer->current_tok.data, '"');
        lexer->Quoting = DOUBLE_QUOTE;
        lexer->current_tok.type = TOKEN_WORD;
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
    if (next_c != '\n')
    {
        if (lexer->word)
        {
            mbt_str_pushc(lexer->current_tok.data, c);
        }
        else
        {
            lexer->word = 1;
            lexer->current_tok.data = mbt_str_init();
            mbt_str_pushc(lexer->current_tok.data, c);
            lexer->current_tok.type = TOKEN_WORD;
        }
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
    else if (c == '\\' && lexer->Quoting != SINGLE_QUOTE)
    {
        backslash_quote(lexer);
    }

    return token_reco(lexer);
}

static void set_doublep(char c, int *doublep);
static struct token var(struct lexer *lexer)
{
    //--5
    if (!(lexer->word))
    {
        lexer->word = 1;
        lexer->current_tok.type = TOKEN_WORD;
        lexer->current_tok.data = mbt_str_init();
    }

    char c = lexer->input;
    mbt_str_pushc(lexer->current_tok.data, c);
    c = lexer_file(lexer->file);
    if (c == '{')
    {
        mbt_str_pushc(lexer->current_tok.data, c);
        while (c != EOF && c != '}')
        {
            c = lexer_file(lexer->file);
            mbt_str_pushc(lexer->current_tok.data, c);
        }
        if (c == EOF)
            errx(1, "missing }");
    }
    else if (c == '(')
    {
        mbt_str_pushc(lexer->current_tok.data, c);
        int doublep = 0;
        int context = 0;
        c = lexer_file(lexer->file);
        mbt_str_pushc(lexer->current_tok.data, c);
        set_doublep(c, &doublep);
        while (c != EOF && context != -1)
        {
            c = lexer_file(lexer->file);
            if (c == '(')
                context++;
            if (c == ')')
                context--;
            mbt_str_pushc(lexer->current_tok.data, c);
        }

        if (!doublep && c == EOF)
            errx(1, "missing )");
        c = lexer_file(lexer->file);
        if (doublep && c == ')')
            mbt_str_pushc(lexer->current_tok.data, c);
        if (!doublep)
        {
            ungetc(c, lexer->file);
        }
        if (doublep && c != ')')
        {
            errx(2, "opened 2 '(' but not closed\n");
        }
    }
    else
    {
        ungetc(c, lexer->file);
    }
    return token_reco(lexer);
}
static void set_doublep(char c, int *doublep)
{
    if (c == '(')
    {
        *doublep = 1;
    }
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
    lexer->current_tok.type = TOKEN_WORD;
    lexer->current_tok.data = mbt_str_init();
    mbt_str_pushc(lexer->current_tok.data, lexer->input);
    return token_reco(lexer);
}

static struct token token_reco(struct lexer *lexer)
{
    lexer->input = lexer_file(lexer->file);
    char c = lexer->input;
    if (c == EOF || c == '\0')
    {
        return end_of_file(lexer); // 1
    }
    else if ((lexer->Quoting == NO_QUOTE && lexer->ope && test_operator(lexer)))
    {
        return next_ope(lexer); // 2
    }
    else if (lexer->ope && !test_operator(lexer))
    {
        return operator(lexer); // 3
    }
    else if ((c == '\\' && lexer->Quoting != SINGLE_QUOTE)
             || (c == '\'' && lexer->Quoting != DOUBLE_QUOTE)
             || (c == '"' && lexer->Quoting != SINGLE_QUOTE))
    {
        return quote(lexer); // 4
    }
    else if (lexer->Quoting == NO_QUOTE && c == '$') // || c == '`')
    {
        return var(lexer); // 5
    }
    else if (lexer->Quoting == NO_QUOTE && test_operator_1(lexer)) // 6
    {
        return begin_ope(lexer); // 6
    }
    else if (lexer->Quoting == NO_QUOTE && c == '\n')
    {
        return new_line(lexer); // 7
    }
    else if ((lexer->Quoting == NO_QUOTE && (c == ' ' || c == '\t')))
    {
        return blank(lexer); // 8
    }
    else if (lexer->word)
    {
        return continue_word(lexer); // 9
    }
    else if (lexer->Quoting == NO_QUOTE && c == '#')
    {
        return com(lexer); // 10
    }
    else
    {
        return begin_word(lexer); // 11
    }
}

struct token lexer_next_token(struct lexer *lexer)
{
    clear_current_tok(lexer);
    lexer->word = 0;
    lexer->ope = 0;
    struct token tok = token_reco(lexer);

    if (tok.type == TOKEN_ALIAS)
    {
        struct token tok = lexer_next_token(lexer);
        char *aw = tok.data->str;
        char *name = calloc(1, strlen(aw));
        char *val = calloc(1, strlen(aw));
        separator_equal(name, val, aw);
        delete_quote(val);
        setenv(name, val, 1);
        tok = lexer_next_token(lexer);
        free(name);
        free(val);
        free(aw);
        return tok;
    }
    else if (tok.type == TOKEN_ALIAS)
    {
        lexer_next_token(lexer);
        char *name = lexer->current_tok.data->str;
        unsetenv(name);
        free(name);
        lexer_next_token(lexer);
    }

    if (tok.type == TOKEN_COM)
        lexer_next_token(lexer);

    return tok;
}
