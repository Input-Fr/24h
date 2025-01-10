#ifndef AST_H
#define AST_H

enum AST_TYPE
{
    AST_LIST,
    AST_WORD,
    AST_COMMAND,
    AST_IF,
    AST_ELIF,
};

struct ast
{
    enum AST_TYPE ast_type;
    (void *)(run)(void *)
    (void *)(free)(void *)
    /*char *value;
    struct ast *cList;
    struct ast *left;
    struct ast *right;*/
};

struct ast_list
{

}


struct ast_word
{
}

struct ast_command
{
}

struct ast_if
{

}


struct ast_elif
{
}
/**
 ** \brief Allocates a new ast with the given type.
 */
struct ast *ast_new(enum AST_TYPE type);

/**
 ** \brief Recursively frees the given ast.
 */
void ast_free(struct ast *ast);

#endif /* !AST_H */
