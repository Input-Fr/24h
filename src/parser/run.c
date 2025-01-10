#include "ast.h"
#include <string.h>

#define RUN(AST) (*(AST)->ftable->run)((AST))




// for three evaluation


// list ast eval
int list_run(struct ast *ast)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list * list = (struct ast_list *) ast;
    size_t i = 0;
    while ( i < list->nbr_cmd && RUN(list->cmd[i]))
    {
        i += 1;
    }
    return i >= list->nbr_cmd;
}

// cmd ast eval
int cmd_run(struct ast * ast)
{
    assert(ast && ast->type == AST_COMMANDE);
    struct ast_cmd * cmd = (struct cmd *)ast;
    if (!cmd->word)
    {
        return 0;
    }
    else
    {
        if (!strcmp(cmd->words[0],"echo"))
        {

        }
        else
        {
            execvp(cmd->words[0],cmd->words);
        }
        return 1;
    }

}


// if ast eval
int if_run(struct ast * ast)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if * if_ast = (struct ast_if *)ast;
    if (RUN(if_ast->condition))
    {
        return RUN(if_ast->then_body);
    }
    else if (!if_ast->else_body)
    {
        return 0;
    }
    else
    {
        return RUN(if_ast->else_body);
    }
}
