#include <assert.h>
#include <err.h>
#include <stdlib.h>

#include "ast.h"

#define FREE(AST) (*(AST)->ftable->free)((AST))

// commande free
void cmd_free(struct ast *ast)
{
    assert(ast && ast->type == AST_COMMAND);
    struct ast_cmd *cmd = (struct ast_cmd *)ast;
    if (cmd->words)
    {
        for (size_t i = 0; cmd->words[i] != NULL; i++)
        {
            free(cmd->words[i]);
        }
    }
    free(cmd->words);
    free(ast);
}

// list free
void list_free(struct ast *ast)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    for (size_t i = 0; i < list->nbr_cmd; i++)
    {
        FREE(((list->cmd)[i]));
    }
    free(list->cmd);
    free(ast);
}

struct operation
{
    enum op op;
    struct ast *left;
    struct ast *right;
};

union content
{
    struct ast *pipeline;
    struct operation *op;
};

struct ast_and_or
{
    struct ast base;
    enum and_or_type t;
    union content c;
};

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
void boucle_free(struct ast * ast)
{
    assert(ast && ast->type == AST_BOUCLE);
    struct ast_boucle * boucle = (struct ast_boucle *) ast;
    FREE(boucle->condition);
    FREE(boucle->do_body);
    free(ast);
}

// redirection
void redirection_free(struct ast * ast)
{
    assert(ast && ast->type == AST_REDIRECTION);
    struct ast_redirection * redir = (struct ast_redirection *)ast;
    // tu dois free le word et le type de redirection
    free(word);
    //free(redir_op); //plus la peine j'ai mis des enum à la place
    free(ast);
}


// element
void element_free(struct ast * ast)
{
    assert(ast && ast->type == AST_ELEMENT);
    struct ast_element * elt = (struct ast_element *) ast;
    if (elt->type != WORD)
    {
        FREE(elt->elt->redirection);
    }
}

void shell_cmd_free(struct ast * ast)
{
    assert(ast && ast->type == AST_SHELL_CMD);
    struct ast_cmd_shell * cmd = (struct ast_cmd_shell *) ast;
    for (int i = 0; i < cmd->nbr_redirection; i++)
    {
        FREE(cmd->redirection[i]);
    }
    FREE(cmd->rule);
    free(ast);
}

void pipeline_free(struct ast * ast)
{
    assert(ast && ast->type == AST_PIPELINE);
    struct ast_pipeline *ast_pipe = (struct ast_pipeline *)ast;
    for(int i = 0; i < ast_pipe->nbr_cmd; i++)
    {
        FREE(ast_pipe->cmd[i]);
    }
    free(ast);
}
