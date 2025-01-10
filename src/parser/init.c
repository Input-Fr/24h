#include "ast.h"

#include <err.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

struct ast *ast_cmd_init(char **word)
{
    static struct ast_ftable ftable = {
        .run = &cmd_run, .free = &cmd_free,
        .pretty_print = &cmd_pretty_print,
    };
    struct ast_cmd *cmd = malloc(sizeof(struct ast_cmd));
    if (!cmd)
    {
        return NULL;
    }
    cmd->base.type = AST_COMMANDE;
    cmd->base.ftable = &ftable;
    cmd->words = word;
    return &cmd->base;
}

struct ast * ast_if_init(struct ast *condition, struct ast * then_body,
        struct ast * else_body )
{
    static struct ast_ftable ftable = 
    {
        .run = &if_run, .free = &if_free,
        .pretty_print = &if_pretty_print,
    };
    struct ast_if *if_ast = malloc(sizeof(struct ast_if));
    if (!if_ast)
    {
        return NULL;
    }
    if_ast->base.type = AST_IF;
    if_ast->base.ftable = &ftable;
    if_ast->condition = condition;
    if_ast->then_body = then_body;
    if_ast->else_body = else_body;
    return &if_ast->base;
}

struct ast * ast_list_init(void)
{
    static struct ast_ftable ftable =
    {
        .run = &list_run, .free = &list_free,
        .pretty_print = &list_pretty_print,
    };
    struct ast_list * list = calloc(1,sizeof(struct ast_list));
    if (!list)
    {
        return NULL;
    }
    list->base.type = AST_LIST;
    list->base.ftable = &ftable;
    return &list->base;
}



void list_push(struct ast * list_ast,struct ast * new_children)
{
    assert(list_ast && list_ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)list_ast;
    size_t index = list->nbr_cmd;
    list->nbr_cmd += 1;
    struct ast ** ast = NULL;
    ast = realloc(list->cmd, sizeof(struct ast *) + list->nbr_cmd);
    if (ast == NULL)
    {
        return;
    }
    else
    {
        list->cmd = ast;
        list->cmd[index] = new_children;
    }
}
