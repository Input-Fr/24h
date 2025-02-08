#include <stdio.h>
#include <stdlib.h>

#include "evalexpr.h"

// retirer H  1->2->3->4    T  ajouter ici

size_t fifo_size(struct fifo *fifo)
{
    return fifo->size;
}

void fifo_push(struct fifo *fifo, struct token *tok)
{
    struct list *new;
    new = malloc(sizeof(struct list));

    if (fifo->size != 0)
    {
        fifo->tail->next = new;
    }

    if (fifo->size == 0)
    {
        fifo->head = new;
    }
    fifo->tail = new;
    new->data = tok;
    new->next = NULL;

    fifo->size += 1;
}

int fifo_head(struct fifo *fifo)
{
    return fifo->head->data->value;
}

// retirer H  1->2->3->4    T  ajouter ici

void fifo_pop(struct fifo *fifo)
{
    if (fifo->size == 0)
    {
        return;
    }
    if (fifo->size == 1)
    {
        struct list *tmp = fifo->head;
        fifo->tail = NULL;
        fifo->head = NULL;
        fifo->size = 0;
        free(tmp);
    }
    else
    {
        struct list *tmp = fifo->head;
        fifo->head = fifo->head->next;
        fifo->size -= 1;
        free(tmp);
    }
}

/*
void fifo_print(const struct fifo *fifo)
{
    struct list *t = fifo->head;

    while (t != NULL)
    {
        printf("%d -> ", t->data->value);
        t = t->next;
        // si -= 1;
    }
}
*/
