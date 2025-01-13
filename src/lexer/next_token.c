#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static struct token rule_one(struct lexer *lexer, int *word)
{
    // 1
    if (*word)
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


static void rule_four(struct lexer *lexer)
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
    }
}

static void begin_word(struct lexer *lexer)
{
    lexer->current_tok.data = mbt_str_init();
    mbt_str_pushc(lexer->current_tok.data, lexer->input);
    if (lexer->Quoting == BACKSLASH_QUOTE)
    {
        lexer->Quoting = NO_QUOTE;
    }

}

struct token lexer_next_token(struct lexer *lexer)
{
    clear_current_tok(lexer);
    int word = 0;
    while (1)
    {
        lexer->input = lexer_file(lexer->file);
        char c = lexer->input;

        if (c == EOF || c == '\0') //1
        {
            return rule_one(lexer, &word);
        }
        //else if (lexer->Quoting == NO_QUOTE && //tout les autres operands
        //        lexer->current_tok.type == TOKEN_SEMI)  //test_operand
        //{
        //    if (test_operand(c)) //2
        //    {
        //        //mbt_str_pushc(lexer->current_tok.data, c);
        //    }
        //    else //3
        //    {
        //        //unget(c)
        //        //return lexer->current_tok;
        //    }
        //}
        else if ((c == '\\') || c == '\'' || c == '"') //4    cas \n
        {
            rule_four(lexer);
        }
        else if (lexer->Quoting == NO_QUOTE && c == ';') //6
        {
            if (word)
            {
                return rule_six(lexer);
            }
            else
            {
                //mbt_str_pushc(lexer->current_tok.data, c);
                //lexer->current_tok.type == TOKEN_OPE;
                //test_operand(lexer);
                lexer->current_tok.type = TOKEN_SEMI;
                return lexer->current_tok;
            }
        }
        else if (lexer->Quoting == NO_QUOTE && c == '\n') //7
        {
            return rule_seven(lexer, &word);
        }
        else if ((lexer->Quoting == NO_QUOTE && c == ' ')) //8
        {
            if (word)
            {
                return rule_eight(lexer);
            }
        }
        else if (word) //9
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
