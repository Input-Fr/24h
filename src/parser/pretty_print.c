#include <assert.h>
#include <stdio.h>

#include "ast.h"
#include <stddef.h>

#define PRINT(AST, C) (*(AST)->ftable->pretty_print)((AST), (C))
// list pretty_print
int if_pretty_print(struct ast *ast, int actual)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if *list = (struct ast_if *)ast;
    int cond_int = actual + 1;
    printf("\t%d [ label = \"IF\"];\n", actual);
    printf("\t%d -> %d [label = \"condition\"];\n", actual, cond_int);
    int then_int = PRINT(list->condition, cond_int) + 1;
    printf("\t%d -> %d [label = \"then\"];\n", actual, then_int);
    int else_int = PRINT(list->then_body, then_int) + 1;
    if (list->else_body)
    {
        printf("\t%d -> %d [label = \"else\"];\n", actual, else_int);
        int res = PRINT(list->else_body, else_int) + 1;
        return res;
    }
    return else_int;
}

// if  pretty_print
int list_pretty_print(struct ast *ast, int actual)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    printf("\t%d [ label = \"LIST\"];\n", actual);
    size_t t = actual + 1;
    for (size_t i = 0; i < list->nbr_cmd; i++)
    {
        printf("\t%d -> %ld;\n", actual, t);
        t = PRINT(list->cmd[i], t) + 1;
    }
    return t;
}

// cmd pretty_print
int cmd_pretty_print(struct ast *ast, int actual)
{
    assert(ast && ast->type == AST_COMMAND);
    struct ast_cmd *cmd = (struct ast_cmd *)ast;
    printf("\t%d [ label = \"CMD\"];\n", actual);
    printf("\t%d[label = \"", (actual + 1));
    size_t i = 0;
    for (; cmd->words[i] != NULL; i++)
    {
        printf("<f%ld> %s|", i, cmd->words[i]);
    }
    printf("<f%ld> NULL\", color=black,shape=record];\n", i);
    printf("\t%d -> %d:%ld;\n", actual, (actual + 1), (i / 2));
    return actual + 1;
}


// negation pretty print

int negation_pretty_print(struct ast * ast, int actual)
{
    assert(ast && ast->type == AST_NEGATION);
    struct ast_negation * nega = (struct ast_negation *)ast;
    printf("\t%d [ label = \"!\", color=red];\n", actual);
    printf("\t%d -> %d",actual, (actual + 1));
    return PRINT(nega->condition,(actual  + 1));
}

void pretty_print_ast(struct ast *ast)
{
    printf("digraph { \n");
    printf("\tlabel = \"AST PARSER\";\n");
    printf("\tnode [ shape=square, color=green ];\n");
    PRINT(ast, 1);
    printf("\n}\n");
}


