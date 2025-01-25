#include <assert.h>
#include <err.h>
#include <stdlib.h>

#include "ast.h"

#define FREE(AST)                                                              \
    (*(AST)->ftable->free)((AST)) // Permet d'appeler la methode free d'un ast
#define FREE_LIST(ASTS, NBR)                                                   \
    handle_free_astlist((ASTS), &(NBR)) // free toute une liste d'ast

// and_or free
void and_or_free(struct ast *ast)
{
    assert(ast && ast->type == AST_AND_OR);
    struct ast_and_or *and_or_ast = (struct ast_and_or *)ast;
    if (and_or_ast->t == NODE_PIPELINE)
    {
        FREE(and_or_ast->c.pipeline);
    }
    else
    {
        FREE(and_or_ast->c.op->left);
        FREE(and_or_ast->c.op->right);
        free(and_or_ast->c.op);
    }
    free(ast);
}

// free une liste d'ast
static void handle_free_astlist(struct ast **asts, size_t *nbr_elt)
{
    for (size_t i = 0; i < *nbr_elt; i++)
    {
        FREE(asts[i]);
    }
    free(asts);
}

// list free
void list_free(struct ast *ast)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    FREE_LIST(list->cmd, list->nbr_cmd);
    free(ast);
}

// if free
void if_free(struct ast *ast)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if *if_ast = (struct ast_if *)ast;
    FREE(if_ast->condition);
    FREE(if_ast->then_body);
    if (if_ast->else_body)
    {
        FREE(if_ast->else_body);
    }
    free(ast);
}

// boucle (until and while) free
void boucle_free(struct ast *ast)
{
    assert(ast && ast->type == AST_BOUCLE);
    struct ast_boucle *boucle = (struct ast_boucle *)ast;
    FREE(boucle->condition);
    FREE(boucle->do_body);
    free(ast);
}

// redirection
void redirection_free(struct ast *ast)
{
    assert(ast && ast->type == AST_REDIRECTION);
    struct ast_redirection *redir = (struct ast_redirection *)ast;
    free(redir->word);
    free(ast);
}

// element
void element_free(struct ast *ast)
{
    assert(ast && ast->type == AST_ELEMENT);
    struct ast_element *elt = (struct ast_element *)ast;
    if (elt->type != WORD)
    {
        FREE(elt->elt.redirection);
    }
    else
    {
        free(elt->elt.word);
    }
    free(ast);
}

void shell_cmd_free(struct ast *ast)
{
    assert(ast && ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd *cmd = (struct ast_shell_cmd *)ast;
    FREE_LIST(cmd->redirection, cmd->nbr_redirection);
    FREE(cmd->rule);
    free(ast);
}

void simple_cmd_free(struct ast *ast)
{
    assert(ast && ast->type == AST_SIMPLE_CMD);
    struct ast_simp_cmd *cmd = (struct ast_simp_cmd *)ast;
    FREE_LIST(cmd->prefix, cmd->nbr_prefix);
    FREE_LIST(cmd->element, cmd->nbr_element);
    free(cmd->word);
    free(ast);
}

void pipeline_free(struct ast *ast)
{
    assert(ast && ast->type == AST_PIPELINE);
    struct ast_pipeline *ast_pipe = (struct ast_pipeline *)ast;
    FREE_LIST(ast_pipe->cmd, ast_pipe->nbr_cmd);
    free(ast);
}

void for_free(struct ast *ast)
{
    assert(ast && ast->type == AST_FOR);
    struct ast_for *boucle = (struct ast_for *)ast;
    FREE(boucle->do_body);
    for (size_t i = 0; i < boucle->nbr_elt; i++)
    {
        free(boucle->list[i]);
    }
    free(boucle->list);
    free(ast);
}
