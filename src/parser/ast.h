#ifndef AST_H
#define AST_H

#include <stddef.h>

enum AST_TYPE
{
    AST_LIST,
    AST_AND_OR,
    AST_SIMPLE_CMD,
    AST_SHELL_CMD,
    AST_IF,
    AST_PIPELINE,
    AST_BOUCLE, // while and until
    AST_FOR,
    AST_REDIRECTION,
    AST_OPERATOR, // and// pr
    AST_ELEMENT,
};

struct ast_ftable;

struct ast
{
    enum AST_TYPE type;
    struct ast_ftable *ftable;
};

struct ast_ftable
{
    int (*run)(struct ast *ast);
    void (*free)(struct ast *ast);
    int (*pretty_print)(struct ast *ast, int actual);
    void (*push)(struct ast * ast,struct ast * add);
};

enum REDIRECTION_TYPE
{
    LESS,// <
    GREATER_AND, // >&
    GREATER, // >
    DGREATER, // >>
    LESS_AND, // <&
    CLOBBER, // >|
    LESS_GREATER, // <>
};

enum CMD_TYPE
{
    SHELL,
    SIMPLE,
};

enum and_or_type
{
    NODE_PIPELINE,
    NODE_AND_OR,
};

enum op
{
    AND_OP,
    OR_OP,
};

struct ast_shell_cmd
{
    struct ast  base;
    struct ast ** redirection;
    struct ast * rule;
    int nbr_redirection;
};

struct ast_simp_cmd
{
    struct ast base;
    int nbr_element;
    int nbr_prefix;
    struct ast ** element;
    struct ast ** prefix; 
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
    struct ast base;
    size_t nbr_cmd; // number of command
    struct ast **cmd; // the list of command
};

struct operation
{
    enum op op;
    struct ast *left;
    struct ast *right;
};

union content
{
    struct ast *pipeline;
    struct operation *op;
};

struct ast_and_or
{
    struct ast base;
    enum and_or_type t;
    union content c;
};

struct ast_pipeline
{
    struct ast base;
    int negation;
    struct ast ** cmd;
    int nbr_cmd;
};

struct ast_boucle
{
    struct ast base;
    struct ast * condition;
    struct ast * do_body;
    int run_condition;
};


// element

enum ELEMENT_TYPE
{
    WORD,
    REDIRECTION,
};

union element
{
    char * word;
    struct ast * redirection;
};

struct ast_element
{
    enum ELEMENT_TYPE type;
    union element elt;
};

struct ast_redirection
{
    struct ast base;
    int  n;
    char * word;
    enum REDIRECTION_TYPE redir_op;
};
// init of every ast type


// if
struct ast *ast_if_init(struct ast *condition, struct ast *then_body,
                        struct ast *else_body);
// list of command
struct ast *ast_list_init(void);

// boucle (until and while)
struct ast * ast_boucle_init(struct  ast  * condition,
        struct ast * do_body, int run_condition);

// redirection 
struct ast * ast_redirection_init(int fd, char * word,
        enum REDIRECTION_TYPE redirection_type);

// element
struct ast * ast_element_init(enum ELEMENT_TYPE type, char * word,
        struct ast * ast);

struct ast * ast_shell_cmd_init(struct ast * rule_if);

struct ast *ast_pipeline_init(int neg, struct ast *cmd);

struct ast *ast_and_or_init(struct ast *pipeline);

// list ast function
int list_run(struct ast *ast);
void list_free(struct ast *ast);
int list_pretty_print(struct ast *ast, int actual);
void list_push(struct ast *ast, struct ast *add);

// and_or ast function
int and_or_run(struct ast *ast);
void and_or_free(struct ast *ast);
int and_or_pretty_print(struct ast *ast, int actual);
void and_or_push(struct ast *ast, struct ast *add);

// if ast function
int if_run(struct ast *ast);
void if_free(struct ast *ast);
int if_pretty_print(struct ast *ast, int actual);
void if_push(struct ast * ast, struct ast * add); 

//boucle (until and while)
int boucle_run(struct ast * ast);
void boucle_free(struct ast * ast);
int boucle_pretty_print(struct ast * ast, int actual);
void boucle_push(struct ast * ast, struct ast * add); 

// redirection 
int redirection_run(struct ast * ast);
void redirection_free(struct ast * ast);
int redirection_pretty_print(struct ast * ast, int actual);
void redirection_push(struct ast * ast, struct ast * add);


//element
int element_run(struct ast * ast);
void element_free(struct ast *ast);
int element_pretty_print(struct ast * ast, int actual);
void element_cmd_push(struct ast * ast, struct ast * add);



// simple command //TODO
int simple_cmd_run(struct ast * ast);
void simple_cmd_free(struct ast * ast);
int simple_cmd_pretty_print(struct ast * ast, int actual);
void simple_cmd_push(struct ast * ast, struct ast * add);

// shell command 
int shell_cmd_run(struct ast * ast);
void shell_cmd_free(struct ast * ast);
int shell_cmd_pretty_print(struct ast * ast,int actual);
void shell_cmd_push(struct ast * ast,struct ast * add);

//pipeline command
int pipeline_run(struct ast* ast);
void pipeline_free(struct ast * ast);
int pipeline_pretty_print(struct ast * ast, int actual);
void pipeline_push(struct ast * ast, struct ast * add);

// pretty_print the entire_ast
void pretty_print_ast(struct ast *ast);
#endif /* !AST_H */
