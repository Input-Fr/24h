#ifndef AST_H
#define AST_H

enum AST_TYPE
{
    AST_LIST,
    AST_COMMAND,
    AST_IF,
    AST_ELIF,
};

struct ast_ftable
{
    void (*run)(struct ast *ast);
    void (*free)(struct ast *ast);
    void (*pretty_print)(struct ast *ast);
};


struct ast
{
    enum AST_TYPE ast_type;
    struct ast_ftable *ftable;
};


struct ast_cmd
{
    struct ast base;
    char ** words;
};


struct ast_if 
{
    struct ast base;
    struct ast *condition; // the condition
    struct ast *then_body; // the body of the then clause
    struct ast *else_body; // the body of the else, may be NULL
};

struct ast_list
{
    int nbr_cmd ;// number of commande
    struct ast ** cmd; // the list of commande
}

struct ast * ast_cmd_init(char **word);
struct ast * ast_if_init(struct ast *condition, struct ast * then_body,
        struct ast * else_body );

struct ast * ast_list_init(struct ast **cmd, int nbr_cmd);



// list ast function
 void list_run(struct ast *ast);
 void list_free(struct ast *ast);
 void list_pretty_print(struct ast *ast);

//cmd ast function
 void cmd_run(struct ast * ast);
 void cmd_free(struct ast *ast);
 void cmd_pretty_print(struct ast *ast);


// if ast function
void if_run(struct ast * ast);
void if_free(struct ast * ast);
void if_pretty_print(struct ast * ast);







//free the ast
//
//
/**
 ** \brief Allocates a new ast with the given type.
 */
struct ast *ast_new(enum AST_TYPE type);

/**
 ** \brief Recursively frees the given ast.
 */
void ast_free(struct ast *ast);

#endif /* !AST_H */
