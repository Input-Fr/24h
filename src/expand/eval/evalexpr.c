#include "evalexpr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int eval_arithmetic(char *result, char *input, struct hash_map *h)
{
    struct fifo *f = fifo_init();

    lexing(f, input, h);

    int i = 0;

    struct fifo *output = step2(f);
    i = rpn(output, result);
    free(output);
    free(f);

    if (i == 0)
    {
        return 0;
    }
    else
    {
        return i;
    }
}
