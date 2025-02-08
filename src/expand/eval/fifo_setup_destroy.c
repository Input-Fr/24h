#include <stdio.h>
#include <stdlib.h>

#include "evalexpr.h"

struct fifo *fifo_init(void)
{
    struct fifo *q;
    q = malloc(sizeof(struct fifo));
    q->head = NULL;
    q->tail = NULL; //  (*q).tail
    q->size = 0;
    return q;
}

void fifo_clear(struct fifo *fifo)
{
    while (fifo->tail != NULL)
    {
        free(fifo->head->data);
        fifo_pop(fifo);
    }
    fifo->size = 0;
}

void fifo_destroy(struct fifo *fifo)
{
    fifo_clear(fifo);
    free(fifo);
}
