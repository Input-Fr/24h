#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"

struct ast * ast_variable_init(char *name, char *val)
{
    static struct ast_ftable ftable = {
        .run = &variable_run,
        .free = &variable_free,
    };
    struct ast_variable *ast_variable = malloc(sizeof(struct ast_if));
    ast_variable->base.type = AST_VARIABLE;
    ast_variable->base.ftable = &ftable;
    ast_variable->name = name;
    ast_variable->val = val;
    return &ast_variable->base;
}

// and_or initiation
struct ast *ast_and_or_init(struct ast *pipeline)
{
    static struct ast_ftable ftable = {
        .run = &and_or_run,
        .free = &and_or_free,
        .pretty_print = &and_or_pretty_print,
        .push = &and_or_push,
    };
    struct ast_and_or *and_or = calloc(1, sizeof(struct ast_and_or));
    if (!and_or)
    {
        return NULL;
    }
    and_or->base.type = AST_AND_OR;
    and_or->base.ftable = &ftable;
    and_or->t = NODE_PIPELINE;
    and_or->c.pipeline = pipeline;
    return &and_or->base;
}

// if initiation
struct ast *ast_if_init(struct ast *condition, struct ast *then_body,
                        struct ast *else_body)
{
    static struct ast_ftable ftable = {
        .run = &if_run,
        .free = &if_free,
        .pretty_print = &if_pretty_print,
        .push = &if_push,
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
        .push = &list_push,
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

// boucle (while and until ) initialisation
struct ast * ast_boucle_init(struct ast * condition,
        struct ast * do_body, int run_condition)
{
    static struct ast_ftable ftable = {
        .run = &boucle_run,
        .free = &boucle_free,
        .pretty_print = &boucle_pretty_print,
        .push = &boucle_push
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

struct ast * ast_redirection_init(int fd, char * words, 
        enum REDIRECTION_TYPE redirection_type)
{
    static struct ast_ftable ftable = {
        .run = &redirection_run,
        .free = &redirection_free,
        .pretty_print = &redirection_pretty_print,
        .push = &redirection_push,
    };
    struct ast_redirection * redi = malloc(sizeof(struct ast_redirection));
    if (!redi)
    {
        return NULL;
    }
    redi->base.type = AST_REDIRECTION;
    redi->base.ftable = &ftable;
    redi->n = fd;
    redi->word = words;
    redi->redir_op = redirection_type;
    return &redi->base;
}

struct ast * ast_element_init(enum ELEMENT_TYPE type,char * word,
        struct ast * ast)
{
    static struct ast_ftable ftable = {
        .run = &element_run,
        .free = &element_free,
        .pretty_print = &element_pretty_print,
        .push = &element_push,
    };
    struct ast_element* element = malloc(sizeof(struct ast_element));
    if (!element)
    {
        return NULL;
    }
    element->base.type = AST_ELEMENT;
    element->base.ftable = &ftable;
    element->type = type;
    if (type == WORD)
    {
        element->elt.redirection = ast;
    }
    else
    {
        element->elt.word = word;
    }
    return &element->base;
}

struct ast * ast_shell_cmd_init(struct ast * rule)
{
    static struct ast_ftable ftable = {
        .run  = &shell_cmd_run,
        .free = &shell_cmd_free,
        .pretty_print = &shell_cmd_pretty_print,
        .push = &shell_cmd_push,
    };
    struct ast_shell_cmd * cmd = calloc(1,sizeof(struct ast_shell_cmd));
    if (!cmd)
    {
        return NULL;
    }
    cmd->base.type = AST_SHELL_CMD;
    cmd->base.ftable = &ftable;
    cmd->rule = rule;
    return  &cmd->base;
}

struct ast *ast_pipeline_init(int neg, struct ast *cmd)
{
    static struct ast_ftable ftable = {
        .run  = &pipeline_run,
        .free = &pipeline_free,
        .pretty_print = &pipeline_pretty_print,
        .push = &pipeline_push,
    };

    struct ast_pipeline *pipe = calloc(1, sizeof(struct ast_pipeline));
    if (!pipe)
    {
        return NULL;
    }
    pipe->base.type = AST_PIPELINE;
    pipe->base.ftable = &ftable;
    pipe->negation = neg;
    (void)cmd;
    return &pipe->base;
}


struct ast * ast_simple_cmd_init(char * word)
{
    static struct ast_ftable ftable = {
        .run = &simple_cmd_run,
        .free = &simple_cmd_free,
        .pretty_print = &simple_cmd_pretty_print,
        .push = &simple_cmd_push,
    };
    struct ast_simp_cmd * cmd = calloc(1,sizeof(struct ast_simp_cmd));
    if (!cmd)
    {
        return NULL;
    }
    cmd->word = word;
    cmd->base.type = AST_SIMPLE_CMD;
    cmd->base.ftable = &ftable; 
    return &cmd->base;
}	
