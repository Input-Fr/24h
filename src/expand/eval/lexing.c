#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evalexpr.h"
#include "expand/expand.h"

// int error(char val)
//{
//     if ((val >= '0' && val <= '9') || val == ' ' || val == '+' || val == '-'
//         || val == '/' || val == ' ' || val == '%' || val == '^' || val == ')'
//         || val == '(' || val == '*')
//     {
//         return 0;
//     }
//     return 1;
// }

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

void expand_ariii(char *string, size_t *i, struct hash_map *h, struct fifo *f)
{
    char *str = calloc(1, 64);
    str[0] = string[*i];
    *i += 1;
    int j = 1;
    while (isalnum(string[*i]) || string[*i] == '_' || string[*i] == '"'
           || string[*i] == '}' || string[*i] == '{' || string[*i] == '$')
    {
        str[j] = string[*i];
        *i += 1;
        j += 1;
    }
    struct token *tok = malloc(sizeof(struct token));
    char *val;
    int a = 0;
    if (str[0] == '$' || str[0] == '"')
    {
        val = expand(h, str);
        a = 1;
    }
    else
    {
        struct ast * save = NULL;
        val = hash_map_get(h, str,&save);
        (void)save;
    }
    if (strlen(val) != 0)
        tok->value = my_atoi(val);
    else
        tok->value = 0;

    tok->type = INT;
    fifo_push(f, tok);
    free(str);
    if (a)
        free(val);
}

int tri2(char *string, size_t len, struct fifo *f, struct hash_map *h)
{
    for (size_t i = 0; i < len; i += 1)
    {
        // if (error(string[i]))
        //{
        //     free(string);
        //     return 1;
        // }

        if (isalpha(string[i]) || string[i] == '_' || string[i] == '$'
            || string[i] == '"')
        {
            expand_ariii(string, &i, h, f);
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
    // if (f->size == 0)
    //{
    //     return 0;
    // }
    // return -1;
    return 0;
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
