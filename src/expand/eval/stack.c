#include <stdio.h>
#include <stdlib.h>

#include "evalexpr.h"

struct stack *stack_push(struct stack *s, struct token *tok)
{
    struct stack *pu = malloc(sizeof(struct stack));
    pu->next = s;
    pu->data = tok;

    return pu;
}

struct stack *stack_pop(struct stack *s)
{
    if (s == NULL)
    {
        return NULL;
    }

    struct stack *tmp = s->next;
    // free(s->data);
    free(s);
    return tmp;
}

int stack_peek(struct stack *s)
{
    if (s == NULL)
    {
        return 0;
    }
    return s->data->value;
}
