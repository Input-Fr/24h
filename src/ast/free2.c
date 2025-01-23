#include <assert.h>
#include <err.h>
#include <stdlib.h>

#include "ast.h"
#define FREE(AST) (*(AST)->ftable->free)((AST))
void variable_free(struct ast *ast)
{
    assert(ast && ast->type == AST_VARIABLE);
    // struct ast_variable *variable_ast = (struct ast_variable *)ast;
    // free(variable_ast->name);
    // free(variable_ast->val);
    free(ast);
}

void free_function(struct ast * ast)
{
    assert(ast && ast->type == AST_FUNCTION);
    struct ast_function * fct = (struct ast_function *)ast;
    FREE(fct->shell_command);
    free(fct->fname);
    free(ast);
}
