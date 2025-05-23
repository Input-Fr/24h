#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"

struct ast *ast_function_init(char *fname, struct ast *shell_command)
{
    static struct ast_ftable ftable = {
        .run = &function_run,
        .free = &free_function,
    };
    struct ast_function *function = calloc(1, sizeof(struct ast_function));
    if (!function)
    {
        return NULL;
    }
    function->base.type = AST_FUNCTION;
    function->base.ftable = &ftable;
    function->fname = fname;
    function->shell_command = shell_command;
    return &function->base;
}

struct ast *ast_simple_cmd_init(char *word)
{
    static struct ast_ftable ftable = {
        .run = &simple_cmd_run,
        .free = &simple_cmd_free,
        .push = &simple_cmd_push,
    };
    struct ast_simp_cmd *cmd = calloc(1, sizeof(struct ast_simp_cmd));
    if (!cmd)
    {
        return NULL;
    }
    cmd->word = word;
    cmd->base.type = AST_SIMPLE_CMD;
    cmd->base.ftable = &ftable;
    return &cmd->base;
}

struct ast *ast_for_init(char *variable)
{
    static struct ast_ftable ftable = {
        .run = &for_run,
        .free = &for_free,
        .push = &for_push,
    };
    struct ast_for *boucle = calloc(1, sizeof(struct ast_for));
    if (!boucle)
    {
        return NULL;
    }
    boucle->base.type = AST_FOR;
    boucle->base.ftable = &ftable;
    boucle->variable = variable;
    return &boucle->base;
}

struct ast *ast_subshell_init(struct ast *compound_list)
{
    static struct ast_ftable ftable = {
        .run = &subshell_run,
        .free = &subshell_free,
    };
    struct ast_subshell *sub = calloc(1, sizeof(struct ast_subshell));
    if (!sub)
    {
        return NULL;
    }
    sub->base.type = AST_SUBSHELL;
    sub->base.ftable = &ftable;
    sub->compound_list = compound_list;
    return &sub->base;
}
