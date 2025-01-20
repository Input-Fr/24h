#include <assert.h>
#include <stddef.h>

#include "ast.h"
#include "build_graphivz.h"

#define PRINT(AST, C) (*(AST)->ftable->pretty_print)((AST), (C))
#define PRINT_LIST(A,NB,ACT) handle_print_list((A),(NB),(ACT))


// handle ast with a list of ast in variable
static int handle_print_list(struct ast ** asts,size_t nbr, int actual)
{
    int next_node = actual + 1;
    for (size_t i = 0; i < list->nbr_cmd; i++)
    {
        link(&actual,&next_node,NULL,NULL,NULL);
        next_node = PRINT(list->cmd[i], t);
    }
    return next_node;
}

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

int for_pretty_print(struct ast * ast, int actual)
{
        (void)(ast);
        return actual + 1;
}

