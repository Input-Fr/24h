#ifndef AST_H
#define AST_H

enum AST_TYPE
{
    AST_LIST,
    AST_WORD,
    AST_ECHO,
    AST_COMMAND,
    AST_IF,
    AST_FI,
    AST_THEN,
    AST_ELIF,
    AST_ELSE,
    AST_BOOL
};

struct ast
{
    enum AST_TYPE ast_type;
    char *value;
    struct ast *cList;
    struct ast *left;
    struct ast *right;
};

/**
 ** \brief Allocates a new ast with the given type.
 */
struct ast *ast_new(enum AST_TYPE type);

/**
 ** \brief Recursively frees the given ast.
 */
void ast_free(struct ast *ast);

#endif /* !AST_H */
