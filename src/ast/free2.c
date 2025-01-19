#include <assert.h>
#include <err.h>
#include <stdlib.h>

#include "ast.h"

void variable_free(struct ast *ast)
{
    assert(ast && ast->type == AST_VARIABLE);
    // struct ast_variable *variable_ast = (struct ast_variable *)ast;
    // free(variable_ast->name);
    // free(variable_ast->val);
    free(ast);
}
