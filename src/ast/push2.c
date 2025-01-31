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

void function_push(struct ast *ast, struct ast *add)
{
    assert(ast && ast->type == AST_FUNCTION);
    struct ast_function *fct = (struct ast_function *)ast;
    fct->redirection = add_to(fct->redirection, add, &fct->nbr_redirection);
    return;
}

void element_push(struct ast *ast, struct ast *add)
{
    UNUSED(ast);
    UNUSED(add);
    return;
}

void simple_cmd_push(struct ast *ast, struct ast *add)
{
    assert(ast && ast->type == AST_SIMPLE_CMD);
    struct ast_simp_cmd *cmd = (struct ast_simp_cmd *)ast;
    if (add->type == AST_ELEMENT)
    {
        cmd->element = ADD(cmd->element, add, &cmd->nbr_element);
    }
    else if (add->type == AST_REDIRECTION || add->type == AST_VARIABLE)
    {
        cmd->prefix = ADD(cmd->prefix, add, &cmd->nbr_prefix);
    }
}

void shell_cmd_push(struct ast *ast, struct ast *add)
{
    assert(ast && ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd *cmd = (struct ast_shell_cmd *)ast;
    cmd->redirection = ADD(cmd->redirection, add, &cmd->nbr_redirection);
}

void pipeline_push(struct ast *ast, struct ast *add)
{
    assert(ast && ast->type == AST_PIPELINE);
    struct ast_pipeline *pipe = (struct ast_pipeline *)ast;
    pipe->cmd = ADD(pipe->cmd, add, &pipe->nbr_cmd);
}

void subshell_push(struct ast *ast, struct ast *add)
{
    UNUSED(ast);
    UNUSED(add);
}
