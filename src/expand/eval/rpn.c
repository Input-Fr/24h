#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "evalexpr.h"

/*
void print(struct stack *s)
{
    while (s->next)
    {
        int dat = s->data->value;
        if (s->data->type == INT)
        {
            printf(" %d \n", dat);
        }
        if (s->data->type == ADD)
        {
            printf(" ERROR \n");
            printf(" + \n");
        }
        if (s->data->type == SUB)
        {
            printf(" ERROR \n");
            printf(" - \n");
        }
        if (s->data->type == MULT)
        {
            printf(" ERROR \n");
            printf(" * \n");
        }
        if (s->data->type == DIV)
        {
            printf(" ERROR \n");
            printf(" / \n");
        }
        if (s->data->type == MOD)
        {
            printf(" ERROR \n");
        }

        s = s->next;
    }
    printf("\n");
}
*/
struct token *tri(int val1, int val2, enum token_type type, int *error)
{
    struct token *t = malloc(sizeof(struct token));
    int result = 0;

    if (type == ADD)
    {
        result = val2 + val1;
    }
    if (type == SUB)
    {
        result = val2 - val1;
    }
    if (type == MULT)
    {
        result = val2 * val1;
    }
    if (type == DIV)
    {
        if (val1 == 0)
        {
            *error = 0;
        }
        else
        {
            result = val2 / val1;
        }
    }
    if (type == MOD)
    {
        if (val1 == 0)
        {
            *error = 0;
        }
        else
        {
            result = val2 % val1;
        }
    }
    if (type == EXP)
    {
        result = 1;
        if (val1 < 0)
        {
            *error = 0;
        }
        while (val1 > 0)
        {
            result *= val2;
            val1 -= 1;
        }
    }

    t->value = result;
    t->type = INT;

    return t;
}

int rpn(struct fifo *f, char *result)
{
    struct stack *s = malloc(sizeof(struct stack));
    s->data = 0;
    s->next = NULL;

    int error = 1;
    while (f->head)
    {
        int type = f->head->data->type;
        if (type == INT)
        {
            s = stack_push(s, f->head->data);
        }
        else
        {
            int val1 = s->data->value;
            free(s->data);
            s = stack_pop(s);
            int val2 = s->data->value;
            free(s->data);
            s = stack_pop(s);
            struct token *t = tri(val1, val2, type, &error);
            s = stack_push(s, t);
            free(f->head->data);
        }
        fifo_pop(f);
    }

    int res = stack_peek(s);
    free(s->data);
    s = stack_pop(s);
    free(s);
    if (error == 0)
    {
        return 3;
    }
    else
    {
        snprintf(result, 3, "%d", res);
        return 0;
    }
}
