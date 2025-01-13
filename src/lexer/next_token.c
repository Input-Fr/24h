#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static struct token rule_one(struct lexer *lexer, int *word, int *operator)
{
    // --1
    if (*word || *operator)
    {
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

static struct token rule_three(struct lexer *lexer)
{
    ungetc(lexer->input, lexer->file);
    return lexer->current_tok;
}


static void rule_four(struct lexer *lexer)
{
    // --4
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

static struct token rule_six(struct lexer *lexer)
{
    ungetc(lexer->input, lexer->file);
    lexer->current_tok.type = reserved_word(lexer);
    return lexer->current_tok;
}

static struct token rule_seven(struct lexer *lexer, int *word)
{
    if (*word)
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

static struct token rule_eight(struct lexer *lexer)
{
    lexer->current_tok.type = reserved_word(lexer);
    return lexer->current_tok;
}

static struct token rule_nine(struct lexer *lexer)
{
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

static void begin_word(struct lexer *lexer)
{
    lexer->current_tok.data = mbt_str_init();
    mbt_str_pushc(lexer->current_tok.data, lexer->input);
    lexer->current_tok.type = TOKEN_WORD;
    if (lexer->Quoting == BACKSLASH_QUOTE)
    {
        lexer->Quoting = NO_QUOTE;
    }

}

static void operator_1(struct lexer *lexer)
{
    char c = lexer->input;
    if (c == ';')
    {
        lexer->current_tok.type = TOKEN_SEMI;
    }
    else if (c == '|')
    {
        lexer->current_tok.type = TOKEN_PIPE;
    }
    else if (c == '&')
    {
        lexer->current_tok.type = TOKEN_AND;
    }
}

static int test_operator_1(struct lexer *lexer)
{
    char c = lexer->input;
    if (c == ';' || c == '|' || c == '&' || c == '>' || c == '<')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


static int test_operator(struct lexer *lexer)
{
    char c = lexer->input;
    size_t length = lexer->current_tok.data->size;
    if (length == 1 && lexer->current_tok.data->str[0] == ';' && c == ';')
    {
        lexer->current_tok.type = TOKEN_DSEMI;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '&' && c == '&')
    {
        lexer->current_tok.type = TOKEN_AND_IF;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '|' && c == '|')
    {
        lexer->current_tok.type = TOKEN_OR_IF;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '>' && c == '>')
    {
        lexer->current_tok.type = TOKEN_DGREAT;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '<' && c == '<')
    {
        lexer->current_tok.type = TOKEN_DLESS;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '>' && c == '&')
    {
        lexer->current_tok.type = TOKEN_GREATAND;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '<' && c == '&')
    {
        lexer->current_tok.type = TOKEN_LESSAND;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '<' && c == '>')
    {
        lexer->current_tok.type = TOKEN_LESSGREAT;
    }
    else if (length == 2 && lexer->current_tok.data->str[0] == '<'
            && lexer->current_tok.data->str[0] == '<' && c == '-')
    {
        lexer->current_tok.type = TOKEN_DLESSDASH;
    }
    else if (length == 1 && lexer->current_tok.data->str[0] == '>' && c == '|')
    {
        lexer->current_tok.type = TOKEN_CLOBBER;
    }
    else
    {
        return 0;
    }
    return 1;
}


struct token lexer_next_token(struct lexer *lexer)
{
    clear_current_tok(lexer);
    int word = 0;
    int operator = 0;

    while (1)
    {
        lexer->input = lexer_file(lexer->file);
        char c = lexer->input;

        if (c == EOF || c == '\0')
        {
            //1
            return rule_one(lexer, &word, &operator);
        }
        else if (lexer->Quoting == NO_QUOTE && operator && test_operator(lexer))
        {
            //2
            mbt_str_pushc(lexer->current_tok.data, c);
        }
        else if (operator && !test_operator(lexer))
        {
            //3
            return rule_three(lexer);
        }
        else if ((c == '\\') || c == '\'' || c == '"') //4    cas \n
        {
            //4
            rule_four(lexer);
        }
        //else if (lexer->Quoting == NO_QUOTE && c == '$' || c == '`') //5
        //{
        //    mbt_str_pushc(lexer->current_tok.data, c);
        //    c = lexer_file(lexer->file);
        //    mbt_str_pushc(lexer->current_tok.data, c);
        //    if (c == '{')
        //    {
        //        //parameter_expansion();
        //        while (1)
        //        {
        //            c = lexer_file(lexer->file);
        //            mbt_str_pushc(lexer->current_tok.data, c);
        //            if (c == ')')
        //            {
        //                c = lexer_file(lexer->file);
        //                mbt_str_pushc(lexer->current_tok.data, c);
        //                if (c == ')')
        //                {
        //                    break;
        //                }
        //            }
        //        }
        //    }
        //    else if (c == '(')
        //    {
        //        c = lexer_file(lexer->file);
        //        mbt_str_pushc(lexer->current_tok.data, c);
        //        if (c == '(')
        //        {
        //            while (1)
        //            {
        //                c = lexer_file(lexer->file);
        //                mbt_str_pushc(lexer->current_tok.data, c);
        //                if (c == ')')
        //                {
        //                    c = lexer_file(lexer->file);
        //                    mbt_str_pushc(lexer->current_tok.data, c);
        //                    if (c == ')')
        //                    {
        //                        break;
        //                    }
        //                }
        //            }
        //            return lexer->current_tok;
        //        }
        //    }
        //}
        else if (lexer->Quoting == NO_QUOTE && test_operator_1(lexer)) //6
        {
            //6
            operator = 1;
            if (word)
            {
                return rule_six(lexer);
            }
            else
            {
                lexer->current_tok.data = mbt_str_init();
                operator_1(lexer);
                mbt_str_pushc(lexer->current_tok.data, c);
            }
        }
        else if (lexer->Quoting == NO_QUOTE && c == '\n')
        {
            return rule_seven(lexer, &word);
        }
        else if ((lexer->Quoting == NO_QUOTE && c == ' '))
        {
            if (word)
            {
                return rule_eight(lexer);
            }
        }
        else if (word)
        {
            mbt_str_pushc(lexer->current_tok.data, c);
        }
        else if (lexer->Quoting == NO_QUOTE && c == '#')
        {
            return rule_nine(lexer);
        }
        else
        {
            word = 1;
            begin_word(lexer);
        }
    }
}
