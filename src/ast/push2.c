#include <assert.h>
#include <stdlib.h>

#include "ast.h"

#define UNUSED(x) (void)(x)
#define ADD(A, B, C) add_to((A), (B), (C))

static struct ast **add_to(struct ast **ast, struct ast *add, size_t *nbr_elt)
{
    struct ast **asts = NULL;

    asts = realloc(ast, sizeof(struct ast *) * (*nbr_elt + 1));
    if (asts == NULL)
    {
        exit(2);
    }
    else
    {
        asts[*nbr_elt] = add;
        *nbr_elt += 1;
        return asts;
    }
}
void function_push(struct ast *ast, struct ast *add)
{
    assert(ast && ast->type == AST_FUNCTION);
    struct ast_function * fct = (struct ast_function *)ast;
    fct->redirection = add_to(fct->redirection,add,&fct->nbr_redirection);
    return;
}

