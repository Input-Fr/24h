#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
struct ast * ast_function(char * fname,struct ast * shell_command)
{
	static struct ast_ftable ftable = {
		.run = &function_run,
		.free = &variable_free,
	};
	struct ast_function * function = calloc(1,sizeof(struct ast_function));
	if (!function)
	{
		return NULL;
	}
	function->base.type = AST_FUNCTION;
	function->base.ftable = &ftable;
	function->fname = fname;
	function->shell_command = shell_command;
	return &function->base;

}
