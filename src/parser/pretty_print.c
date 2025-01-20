#include <assert.h>
#include <stdio.h>

#include "ast.h"
#include <stddef.h>

#define PRINT(AST, C) (*(AST)->ftable->pretty_print)((AST), (C))
#define PRINT_LIST(A,NB,ACT) handle_print_list((A),(NB),(ACT))



static void add_label_print(const char * label, const char * color,
		const char * shape)
{

	if (label || color || shape)
	{
	    printf(" [ ");
	    if (label)
	    {
	        printf("label = \"%s\"",label);
		if (color)
		{
		   printf(", ");
		}
	    }
	    if (color)
	    {
		printf("color = %s",color);
	        if (shape)
		{
		    printf(", ");
		}
	    }
	    if (shape)
	    {
		printf("shape = %s",shape);
	    }
	    printf(" ]");
	}
}

//create a node
static void create_node(int * node, const char  * label, const char * color,
		const char * shape)
{
	assert(node);
	printf("\t%d ", actual);
	add_label_print(label,color, shape);	
	printf(";\n");
}


// link two ast with their node
static void link(int * node1, int * node2,const char * label,const char * color)
{
	assert (node1 && node2);
	printf("\t%d -> %d",*node1, *node2);
	add_label_print(label,color, NULL);
	printf(";\n");
	    
}

// handle ast with a list of ast in variable
static int handle_print_list(struct ast ** asts,size_t nbr, int actual)
{
    int next_node = actual + 1;
    for (size_t i = 0; i < list->nbr_cmd; i++)
    {
	link(&actual,&next_node,NULL,NULL,NULL);
        next_node = PRINT(list->cmd[i], t);
    }
    return next_node;
}

// list pretty_print
int if_pretty_print(struct ast *ast, int actual)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if *list = (struct ast_if *)ast;
    int cond_int = actual + 1;
    create_node(&actual,"IF",NULL,NULL);
    link(&actual,&cond_int,"condition",NULL,NULL);
    int then_int = PRINT(list->condition, cond_int);
    link(&actual,&then_int,"then",NULL,NULL);
    int else_int = PRINT(list->then_body, then_int);
    if (list->else_body)
    {
	link(&actual,&else_int,"else",NULL,NULL);
        int res = PRINT(list->else_body, else_int);
        return res;
    }
    return else_int;
}

int and_or_pretty_print(struct ast * ast,int actual)
{
	(void)ast;
	return actual + 1;
}
int pipeline_pretty_print(struct ast * ast,int actual)
{
	(void)ast;
	return actual + 1;
}
// if  pretty_print
int list_pretty_print(struct ast *ast, int actual)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    create_node(&actual,"LIST",NULL,NULL);
    return PRINT_LIST(list->cnd,list->nbr_cmd,actual);
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
 	struct ast_simp_cmd * cmd = (struct ast_simp_cmd *)ast;
	return actual + 1;
}



// boucle (until and while) print
int boucle_pretty_print(struct ast * ast, int actual)
{
    assert(ast && ast->type == AST_BOUCLE);
    struct ast_boucle * boucle = (struct ast_boucle *)ast;
    if(boucle->run_condition)
    {
	create_node(&actual,"UNTIL",NULL,NULL);
    }
    else
    {
	create_node(&actual,"WHILE",NULL,NULL);
    }
    int next_node = PRINT(boucle->condition,(actual + 1));
    link(&actual,&(actual + 1),"condition",NULL,NULL);
    link(&actual,&next_node,"do",NULL,NULL);
    return PRINT(boucle->do_body,next_node);
}

int redirection_pretty_print(struct ast * ast, int actual)
{
    assert(ast && ast->type == AST_REDIRECTION);
    struct ast_redirection * redir = (struct ast_redirection *) ast;
    printf("\t%d [ label = \"REDIRECTION\"];\n",actual);
    if (redir->word)
    {
    	printf("\t%d [ label = \"%s\",color=black];\n",(actual + 1),redir->word);
	printf("\t %s -> %s [label  = \"type\"]",actual, (actual + 1));

    return actual + 1 + (redir->word != NULL);
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

int shell_cmd_pretty_print(struct ast * ast, int actual)
{
    assert(ast && ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd * cmd = (struct ast_shell_cmd *)ast;
    printf("\t%d [ label = \"SHELL_CMD\"];\n",actual);
    int j = PRINT_LIST(cmd->redirection,cmd->nbr_redirection,actual);
    printf("\t%d -> %d ;\n",actual, j);
    return PRINT(cmd->rule,j);
}

int for_pretty_print(struct ast * ast, int actual)
{
	(void)(ast);
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

