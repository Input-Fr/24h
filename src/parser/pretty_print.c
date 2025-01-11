#include <stdio.h>

#include "ast.h"
#include <stdio.h>
#include <assert.h>

#define PRINT(AST,C) (*(AST)->ftable->pretty_print)((AST),(C))
//list pretty_print
void if_pretty_print(struct ast *ast,int actual)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if * list = (struct ast_if *)ast;
    int cond_int = actual + 1;
    int then_int = cond_int + 1;
    int else_int = then_int + 1;

    printf("%d [ label = \"IF\"]\n",actual);
    printf("%d --> %d [label = \"condition\"]\n",actual,cond_int);

    printf("%d --> %d [label = \"then\"]\n",actual,then_int);
    if (list->else_body)
    {
        printf("%d --> %d [label = \"else\"]\n",actual,else_int);
    }
    PRINT(list->condition,cond_int);
    PRINT(list->then_body,then_int);
    if(list->else_body)
    {
        PRINT(list->else_body,else_int);
    }

}

// if  pretty_print
void list_pretty_print(struct ast * ast,int actual)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list * list  = (struct ast_list *) ast;
    printf("%d [ label = \"LIST\"]\n",actual);
    for (size_t i = 0; i < list->nbr_cmd; i++)
    {
        printf("%d --> %d\n",actual,(actual + 1 + i));
        PRINT(list->cmd[i],(actual + 1 + i));
    }

}

//cmd pretty_print
void cmd_pretty_print(struct ast *ast,int actual)
{
    assert(ast && ast->type == AST_COMMAND);
    struct ast_cmd * cmd = (struct ast_cmd *) ast;
    printf("%d [ label = \"CMD\"",actual);
    int fct_int = actual + 1;
    printf("%d[label = \"",fct_int);
    fct_int += 1;
    for (size_t i  = 0; cmd->words[i] != NULL; i++)
    {
       printf("<f%d> %s|",fct_int);
       fct_int += 1;
    }
    printf("<f%d> NULL\", color=black]\n");
    int moyenne = actual + (fct_int - actual)/2;
    printf("%d --> %d:%d\n",actual,(actual + 1),moyenne);
}


void pretty_print_ast(struct ast *ast)
{
    printf("digraph \n");
    printf("\tlabel = \"AST PARSER\";\n");
    printf("\tnode [ shape=square, color=green ];\n");
    PRINT(ast,1);
    printf("\n}\n");
}


