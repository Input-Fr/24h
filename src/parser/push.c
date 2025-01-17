#include <assert.h>
#include "ast.h"
#include <stdlib.h>

#define UNUSED(x) (void)(x)
#define ADD(A,B,C) add_to((A),(B),&(C))

static struct ast ** add_to(struct ast **ast,struct ast * add,size_t * nbr_elt)
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

void list_push(struct ast *ast, struct ast * add)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    list->cmd  = ADD(list->cmd,add,list->nbr_cmd);
}

void if_push(struct ast * ast, struct ast * add)
{
    UNUSED(ast);
    UNUSED(add);
    return;
}

void boucle_push(struct ast * ast, struct ast * add)
{
    UNUSED(ast);
    UNUSED(add);
    return;
}

void redirection_push(struct ast * ast, struct ast * add)
{
    UNUSED(ast);
    UNUSED(add);
    return;
}


void variable_push(struct ast * ast, struct ast * add)
{
    UNUSED(ast);
    UNUSED(add);
    return;
}

void and_or_push(struct ast * ast, struct ast * add)
{
    UNUSED(ast);
    UNUSED(add);
    return;
}

void element_push(struct ast * ast, struct ast * add)
{
    UNUSED(ast);
    UNUSED(add);
    return;
}


void simple_cmd_push(struct ast * ast, struct ast * add)
{
    assert(ast && ast->type == AST_SIMPLE_CMD);
    struct ast_simp_cmd * cmd = (struct ast_simp_cmd *)ast;
    if (add->type == AST_ELEMENT)
    {
        cmd->element = ADD(cmd->element,add,cmd->nbr_element);
    }
    else if (add->type == AST_REDIRECTION)
    {
        cmd->prefix = ADD(cmd->prefix,add,cmd->nbr_prefix);
    }
}

void shell_cmd_push(struct ast * ast, struct ast * add)
{
    assert(ast && ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd * cmd = (struct ast_shell_cmd *) ast;
    cmd->redirection = ADD(cmd->redirection,add,cmd->nbr_redirection);
}

void pipeline_push(struct ast * ast, struct ast * add)
{
    assert(ast && ast->type == AST_PIPELINE);
    struct ast_pipeline * pipe = (struct ast_pipeline *)ast;
    pipe->cmd = ADD(pipe->cmd,add,pipe->nbr_cmd);
}

