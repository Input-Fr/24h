#include "ast.h"

#include <err.h>
#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (!new)
        return NULL;
    new->type = type;
    return new;
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
        return;

    ast_free(ast->left);
    ast->left = NULL;

    ast_free(ast->right);
    ast->right = NULL;
    while (ast->cList)
    {
        struct ast *save = ast->cList->cList;
        free(ast->cList);
        ast->cList = save;
    }
    free(ast);
}
