#include "ast.h"
#include <err.h>
#include <stdli.h>

// commande free
void cmd_free(struct ast *ast)
{
    assert(ast && ast->type == AST_COMMANDE);
    struct ast_cmd *cmd = (struct ast_cmd *)ast;
    if (cmd->word)
    {
        for (size_t i = 0; cmd->word[i] != NULL; i++)
        {
            free(cmd->word[i]);
        }
    }
    free(cmd->word);
    free(ast)
}


// list free
void list_free(struct ast *ast)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list * list = (struct ast_list *)ast;
    for (size_t i = 0; i < list->nbr_cmd; i++)
    {
        (*list->cmd[i]->ftable->free)(list->cmd[i]);
    }
    free(list->cmd);
    free(ast);
}

// if free
void if_free(struct ast * ast)
{
    assert(ast && ast->typ == AST_IF);
    struct ast_if * if_ast = (struct ast_if *) ast;
    (*if_ast->condition->ftable->free)(if_ast->condition);
    (*if_ast->then_body->ftable->free)(if_ast->then_body);
    if (if_ast->else_body)
    {
        (*if_ast->else_body->ftable->free)(if_ast->else_body);
    }
    free(ast);
}

