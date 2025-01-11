#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

void clear_current_tok(struct lexer *lex)
{
    mbt_str_free(lex->current_tok.data);
    lex->current_tok.data = mbt_str_init();
}

void mbt_str_free(struct mbt_str *str)
{
    // free(str->str);
    free(str);
}

struct mbt_str *mbt_str_init(void)
{
    struct mbt_str *str = malloc(sizeof(struct mbt_str));
    str->str = calloc(1, 6);
    str->capacity = 5;
    str->size = 0;
    return str;
}

void mbt_str_pushc(struct mbt_str *str, char c)
{
    if (str->size >= str->capacity)
    {
        str->capacity += 5;
        str->str = realloc(str->str, str->capacity + 1);
        (str->str)[str->size] = c;
        (str->str)[str->size + 1] = '\0';
    }
    else
    {
        (str->str)[str->size] = c;
        (str->str)[str->size + 1] = '\0';
    }

    str->size += 1;
}
