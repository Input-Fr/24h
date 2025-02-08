#include <stdio.h>

#include "evalexpr.h"

void parenthese(struct token *tok, struct fifo *output, struct stack **s)
{
    if (tok->type == LEFT)
    {
        *s = stack_push(*s, tok);
    }
    if (tok->type == RIGHT)
    {
        struct token *right = tok;
        while ((*s)->data->type != LEFT)
        {
            struct token *val = (*s)->data;
            struct stack *tmp = (*s)->next;
            free(*(s));
            fifo_push(output, val);
            *s = tmp;
        }
        struct token *left = (*s)->data;
        *s = stack_pop(*s);
        free(right);
        free(left);
    }
}

struct fifo *step2(struct fifo *f)
{
    struct stack *s = malloc(sizeof(struct stack));
    s->data = NULL;
    s->next = NULL;

    struct fifo *output = fifo_init();
    while (f->head)
    {
        struct token *tok = f->head->data;
        if (tok->type == INT)
        {
            fifo_push(output, tok);
        }
        if (tok->type != INT && tok->type != LEFT && tok->type != RIGHT)
        {
            if (s->next == NULL)
            {
                s = stack_push(s, tok);
            }
            else
            {
                while (s->next != NULL && s->data->value >= tok->value
                       && s->data->type != LEFT && s->data->type != RIGHT)
                {
                    fifo_push(output, s->data);
                    struct stack *tmp = s->next;
                    free(s);
                    s = tmp;
                }
                s = stack_push(s, tok);
            }
        }
        else
        {
            parenthese(tok, output, &s);
        }
        fifo_pop(f);
    }
    while (s->next != NULL)
    {
        fifo_push(output, s->data);
        struct stack *tmp = s->next;
        free(s);
        s = tmp;
    }

    free(s);
    //    fifo_print(output);
    return output;
}
