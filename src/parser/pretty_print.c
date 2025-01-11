#include <stdio.h>

#include "ast.h"

// list pretty_print
void list_pretty_print(struct ast *ast)
{
    printf("%p", (void *)ast);
    return;
}

// if  pretty_print
void if_pretty_print(struct ast *ast)
{
    printf("%p", (void *)ast);
    return;
}

// cmd pretty_print
void cmd_pretty_print(struct ast *ast)
{
    printf("%p", (void *)ast);
    return;
}
