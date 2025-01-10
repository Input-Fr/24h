#include <ast.h>
// for three evaluation


// list ast eval
int list_run(struct ast *ast)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list * list = (struct ast_list *) ast;
    size_t i = 0;
    while ( i < list->nbr_cmd && (*list->cmd[i]->ftable->run)(list->cmd[i]))
    {
        i += 1;
    }
    return i >= list->nbr_cmd;
}

// cmd ast eval
int cmd_run(struct ast * ast)
{



// if ast eval
int if_run(struct ast * ast)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if * if_ast = (struct ast_if *)ast;
    if ((*if_ast->condition->ftable->run)(if_ast->condition))
    {
        return (*if_ast->then_body->ftable->run)(if_ast->then_body);
    }
    else if (!if_ast->else_body)
    {
        return 0;
    }
    else
    {
        return (*if_ast->else_body->ftable->run)(if_ast->else_body);
    }
}
