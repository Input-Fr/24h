#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evalexpr.h"

int error(char val)
{
    if ((val >= '0' && val <= '9') || val == ' ' || val == '+' || val == '-'
        || val == '/' || val == ' ' || val == '%' || val == '^' || val == ')'
        || val == '(' || val == '*')
    {
        return 0;
    }
    return 1;
}

void tritoken(struct token *tok, char val)
{
    if (val == '+')
    {
        tok->type = ADD;
        tok->value = 1;
    }
    if (val == '-')
    {
        tok->type = SUB;
        tok->value = 1;
    }
    if (val == '*')
    {
        tok->type = MULT;
        tok->value = 2;
    }
    if (val == '/')
    {
        tok->type = DIV;
        tok->value = 2;
    }
    if (val == '%')
    {
        tok->type = MOD;
        tok->value = 2;
    }
    if (val == '^')
    {
        tok->type = EXP;
        tok->value = 3;
    }
    if (val == '(')
    {
        tok->type = LEFT;
        tok->value = 5;
    }
    if (val == ')')
    {
        tok->type = RIGHT;
        tok->value = 5;
    }
}

int tri2(char *string, size_t len, struct fifo *f, struct hash_map *h)
{
    if (h == NULL)
    {
        return 0;
    }
    for (size_t i = 0; i < len; i += 1)
    {
        if (error(string[i]))
        {
            free(string);
            return 1;
        }

        if (string[i] >= '0' && string[i] <= '9')
        {
            size_t j = 0;
            char str[15] = { 0 };
            struct token *tok;
            tok = malloc(sizeof(struct token));

            while (i < len && string[i] >= '0' && string[i] <= '9')
            {
                str[j] = string[i];
                i += 1;
                j += 1;
            }

            int number = my_atoi(str);
            tok->value = number;
            tok->type = INT;
            fifo_push(f, tok);
        }

        if (i < len && (string[i] > '0' || string[i] < '9') && string[i] != ' ')
        {
            struct token *tok = malloc(sizeof(struct token));

            tritoken(tok, string[i]);
            fifo_push(f, tok);
        }
    }
    if (f->size == 0)
    {
        return 0;
    }
    return -1;
}

int lexing(struct fifo *f, char *string, struct hash_map *h)
{
    if (strlen(string) == 0)
    {
        return 0;
    }

    int i = tri2(string, strlen(string), f, h);
    if (i == -1)
    {
        return 0;
    }
    else
    {
        return i;
    }
}
