#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"

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
