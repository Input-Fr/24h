#include <assert.h>
#include <stdio.h>
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
        fprintf(stderr, "error\n");
        exit(2);
    }
    else
    {
        asts[*nbr_elt] = add;
        *nbr_elt += 1;
        return asts;
    }
}

void list_push(struct ast *ast, struct ast *add)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    list->cmd = ADD(list->cmd, add, &list->nbr_cmd);
}

void for_push(struct ast *ast, struct ast *add)
{
    assert(ast && ast->type == AST_FOR);
    struct ast_for *boucle = (struct ast_for *)ast;
    boucle->do_body = add;
    return;
}

void for_push_Word(struct ast *ast, char *add)
{
    assert(ast && ast->type == AST_FOR);
    struct ast_for *boucle = (struct ast_for *)ast;

    char **words = NULL;

    words = realloc(boucle->list, sizeof(struct ast *) * (boucle->nbr_elt + 1));
    if (words == NULL)
    {
        exit(2);
    }
    else
    {
        boucle->list = words;
        words[boucle->nbr_elt] = add;
        boucle->nbr_elt += 1;
    }
}
