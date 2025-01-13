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

// negation free
void negation_free(struct ast * ast)
{
    assert(ast && ast->type == AST_NEGATION);
    struct ast_negation * nega = (struct ast_negation *) ast;
    FREE(nega->condition);
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

