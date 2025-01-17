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

/*
static int cmd_pretty_print(struct ast *ast, int actual)
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
*/
int simple_cmd_pretty_print(struct ast * ast, int actual)
{
	assert(ast && ast->type == AST_SIMPLE_CMD);
	return actual + 1;
}

// cmd pretty_print


// boucle (until and while) print
int boucle_pretty_print(struct ast * ast, int actual)
{
    assert(ast && ast->type == AST_BOUCLE);
    struct ast_boucle * boucle = (struct ast_boucle *)ast;
    if(boucle->run_condition)
    {
        printf("\t%d [ label = \"UNTIL\"];\n",actual);
    }
    else
    {
        printf("\t%d [ label = \"WHILE\"];\n",actual);
    }
    int t = PRINT(boucle->condition,(actual + 1)) + 1;
    printf("\t%d -> %d [label = \"condition\"];\n",actual, (actual + 1));
    printf("\t%d -> %d [label = \"do\"];\n",actual, t);
    return PRINT(boucle->do_body,t) + 1;
}

int redirection_pretty_print(struct ast * ast, int actual)
{
    (void)(ast);
    return actual + 1;
}

int element_pretty_print(struct ast * ast, int actual)
{
    assert(ast && ast->type == AST_ELEMENT);
    struct ast_element * elt = (struct ast_element *) ast;
    if (elt->type ==  WORD)
    {
       printf("\t%d [ label = \"WORD\"];\n",actual);
       printf("\t%d [label = \"%s\", color=black];\n",(actual + 1),elt->elt.word);
       printf("\t%d -> %d ;\n",actual, (actual + 1));
       return actual + 2;
    }
    else
    {
       printf("\t%d [ label = \"REDIRECTION\"];\n",actual);
       printf("\t%d -> %d ;\n",actual, (actual + 1));
       return PRINT(elt->elt.redirection, (actual + 1));
    }
}

int chell_cmd_pretty_print(struct ast * ast, int actual)
{
    assert(ast && ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd * cmd = (struct ast_shell_cmd *)ast;
    printf("\t%d [ label = \"SHELL_CMD\"];\n",actual);
    (void)(cmd);
    return actual + 1;
}
void pretty_print_ast(struct ast *ast)
{
    printf("digraph { \n");
    printf("\tlabel = \"AST PARSER\";\n");
    printf("\tnode [ shape=square, color=green ];\n");
    PRINT(ast, 1);
    printf("\n}\n");
}

