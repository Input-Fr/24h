#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"


// list of command initiation
struct ast *ast_cmd_init(char **word)
{
    static struct ast_ftable ftable = {
        .run = &cmd_run,
        .free = &cmd_free,
        .pretty_print = &cmd_pretty_print,
    };
    struct ast_cmd *cmd = malloc(sizeof(struct ast_cmd));
    if (!cmd)
    {
        return NULL;
    }
    cmd->base.type = AST_COMMAND;
    cmd->base.ftable = &ftable;
    cmd->words = word;
    return &cmd->base;
}

// if initiation
struct ast *ast_if_init(struct ast *condition, struct ast *then_body,
                        struct ast *else_body)
{
    static struct ast_ftable ftable = {
        .run = &if_run,
        .free = &if_free,
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



// List initiation
struct ast *ast_list_init(void)
{
    static struct ast_ftable ftable = {
        .run = &list_run,
        .free = &list_free,
        .pretty_print = &list_pretty_print,
    };
    struct ast_list *list = calloc(1, sizeof(struct ast_list));
    if (!list)
    {
        return NULL;
    }
    list->base.type = AST_LIST;
    list->base.ftable = &ftable;
    return &list->base;
}

void list_push(struct ast *list_ast, struct ast *new_children)
{
    assert(list_ast && list_ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)list_ast;
    size_t index = list->nbr_cmd;
    list->nbr_cmd += 1;
    struct ast **ast = NULL;
    ast = realloc(list->cmd, sizeof(struct ast *) * list->nbr_cmd);
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

// negation initiation

struct ast * ast_negation_init(struct ast * condition)
{
    static struct ast_ftable ftable = {
        .run  = &negation_run,
        .free = &negation_free,
        .pretty_print = &negation_pretty_print,
    };
    struct ast_negation * nega = malloc(sizeof(struct ast_negation));
    if (!nega)
    {
        return NULL;
    }
    nega->base.type = AST_NEGATION;
    nega->base.ftable = &ftable;
    nega->condition = condition;
    return &nega->base;
}


// boucle (while and until ) initialisation
struct ast * ast_boucle_init(struct ast * condition,
        struct ast * do_body, int run_condition)
{
    static struct ast_ftable ftable = {
        .run = &boucle_run,
        .free = &boucle_free,
        .pretty_print = &boucle_pretty_print,
    };
    struct ast_boucle * boucle = malloc(sizeof(struct ast_boucle));
    if (!boucle)
    {
        return NULL;
    }
    boucle->base.type = AST_BOUCLE;
    boucle->base.ftable = &ftable;
    boucle->condition = condition;
    boucle->do_body = do_body;
    boucle->run_condition = run_condition;
    return &boucle->base;
}
