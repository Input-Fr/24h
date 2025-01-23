#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "ast.h"
#include "build_graphivz.h"

#define PRINT(AST, C) (*(AST)->ftable->pretty_print)((AST), (C))
#define PRINT_LIST(A, NB, ACT) handle_print_list((A), (NB), (ACT))

// handle ast with a list of ast in variable
static int handle_print_list(struct ast **asts, size_t nbr, int actual) {
  int next_node = actual + 1;
  for (size_t i = 0; i < nbr; i++) {
    link(actual, next_node, NULL, NULL);
    next_node = PRINT(asts[i], next_node);
  }
  return next_node;
}

// list pretty_print
int if_pretty_print(struct ast *ast, int actual) {
  assert(ast && ast->type == AST_IF);
  struct ast_if *list = (struct ast_if *)ast;
  int cond_int = actual + 1;
  create_node(actual, "IF", NULL, NULL);
  link(actual, cond_int, "condition", NULL);
  int then_int = PRINT(list->condition, cond_int);
  link(actual, then_int, "then", NULL);
  int else_int = PRINT(list->then_body, then_int);
  if (list->else_body) {
    link(actual, else_int, "else", NULL);
    int res = PRINT(list->else_body, else_int);
    return res;
  }
  return else_int;
}

// if  pretty_print
int list_pretty_print(struct ast *ast, int actual) {
  assert(ast && ast->type == AST_LIST);
  struct ast_list *list = (struct ast_list *)ast;
  create_node(actual, "LIST", NULL, NULL);
  int next = PRINT_LIST(list->cmd, list->nbr_cmd, actual);
  return next;
}
/*
static int cmd_pretty_print(struct ast *ast, int actual)
{
    assert(ast && ast->type == AST_COMMAND);
    struct ast_cmd *cmd = (struct ast_cmd *)ast;
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

int simple_cmd_pretty_print(struct ast *ast, int actual) {
  assert(ast && ast->type == AST_SIMPLE_CMD);
  struct ast_simp_cmd *cmd = (struct ast_simp_cmd *)ast;
  create_node(actual, "SIMPLE_CMD", NULL, NULL);
  int next_node = actual + 1;
  create_node(next_node, "PREFIX", NULL, NULL);
  next_node = PRINT_LIST(cmd->prefix, cmd->nbr_prefix, (actual + 1));
  if (cmd->word) {
    create_node(next_node + 1, "WORD", "red", NULL);
    link((next_node), (next_node + 1), NULL, NULL);

    create_node((next_node + 2), cmd->word, "black", NULL);
    link((next_node + 1), (next_node + 2), NULL, NULL);
    create_node((next_node + 3), "ElEMENT", "pink", NULL);
    link((next_node), (next_node + 3), NULL, NULL);
    int node = next_node + 3;
    next_node = PRINT_LIST(cmd->element, cmd->nbr_element, node);
    return next_node;
  }
  return next_node + 1;
}

// boucle (until and while) print
int boucle_pretty_print(struct ast *ast, int actual) {
  assert(ast && ast->type == AST_BOUCLE);
  struct ast_boucle *boucle = (struct ast_boucle *)ast;
  if (boucle->run_condition) {
    create_node(actual, "UNTIL", NULL, NULL);
  } else {
    create_node(actual, "WHILE", NULL, NULL);
  }
  int next_node = PRINT(boucle->condition, (actual + 1));
  link(actual, (actual + 1), "condition", NULL);
  link(actual, next_node, "do", NULL);
  return PRINT(boucle->do_body, next_node);
}

int redirection_pretty_print(struct ast *ast, int actual) {
  assert(ast && ast->type == AST_REDIRECTION);
  struct ast_redirection *redir = (struct ast_redirection *)ast;
  create_node(actual, "REDIRECTION", NULL, NULL);
  if (redir->word) {
    create_node((actual + 1), redir->word, "black", NULL);
    link(actual, (actual + 1), "type", NULL);
  }
  return actual + 1 + (redir->word != NULL);
}

int element_pretty_print(struct ast *ast, int actual) {
  assert(ast && ast->type == AST_ELEMENT);
  struct ast_element *elt = (struct ast_element *)ast;
  if (elt->type == WORD) {
    create_node((actual), "WORD", NULL, NULL);
    create_node((actual + 1), elt->elt.word, "black", NULL);
    link(actual, (actual + 1), NULL, NULL);
    return actual + 2;
  } else {
    create_node(actual, "REDIRECTION", NULL, NULL);
    link(actual, (actual + 1), NULL, NULL);
    return PRINT(elt->elt.redirection, (actual + 1));
  }
}

int shell_cmd_pretty_print(struct ast *ast, int actual) {
  assert(ast && ast->type == AST_SHELL_CMD);
  struct ast_shell_cmd *cmd = (struct ast_shell_cmd *)ast;
  create_node(actual, "SHELL_CMD", NULL, NULL);
  int j = PRINT_LIST(cmd->redirection, cmd->nbr_redirection, actual);

  link(actual, j, NULL, NULL);
  return PRINT(cmd->rule, j);
}

void pretty_print_ast(struct ast *ast) {
  printf("digraph { \n");
  printf("\tlabel = \"AST PARSER\";\n");
  printf("\tnode [ shape=square, color=green ];\n");
  PRINT(ast, 1);
  printf("\n}\n");
}
