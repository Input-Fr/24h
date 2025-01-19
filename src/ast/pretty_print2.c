#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "ast.h"

int and_or_pretty_print(struct ast *ast, int actual)
{
    (void)ast;
    return actual + 1;
}
int pipeline_pretty_print(struct ast *ast, int actual)
{
    (void)ast;
    return actual + 1;
}
