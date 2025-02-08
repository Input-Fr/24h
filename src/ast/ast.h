#ifndef AST_H
#define AST_H

#include <stddef.h>

#include "hash_map/hash_map.h"

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
    AST_VARIABLE,
    AST_FUNCTION,
    AST_ELEMENT,
    AST_SUBSHELL,
};

struct ast_ftable;

struct ast
{
    enum AST_TYPE type;
    struct ast_ftable *ftable;
};

struct ast_ftable
{
    int (*run)(struct ast *ast, struct hash_map *h);
    void (*free)(struct ast *ast);
    void (*push)(struct ast *ast, struct ast *add);
};

enum REDIRECTION_TYPE
{
    LESS, // <
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

struct ast_variable
{
    struct ast base;
    char *name;
    char *val;
};

struct ast_shell_cmd
{
    struct ast base;
    struct ast **redirection;
    struct ast *rule;
    size_t nbr_redirection;
};

struct ast_simp_cmd
{
    struct ast base;
    size_t nbr_element;
    size_t nbr_prefix;
    char *word;
    struct ast **element;
    struct ast **prefix;
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
    size_t nbr_cmd; // number of and_or
    struct ast **cmd; // the list of and_or
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
    struct ast **cmd;
    size_t nbr_cmd;
};

struct ast_boucle
{
    struct ast base;
    struct ast *condition;
    struct ast *do_body;
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
    char *word;
    struct ast *redirection;
};

struct ast_element
{
    struct ast base;
    enum ELEMENT_TYPE type;
    union element elt;
};

struct ast_redirection
{
    struct ast base;
    int n;
    char *word;
    enum REDIRECTION_TYPE redir_op;
};
// init of every ast type

struct ast_for
{
    struct ast base;
    struct ast *do_body;
    char *variable;
    char **list;
    size_t nbr_elt;
};

struct ast_function
{
    struct ast base;
    char *fname;
    struct ast *shell_command;
    size_t nbr_redirection;
    int isHash;
    struct ast **redirection;
};

struct ast_subshell
{
    struct ast base;
    struct ast *compound_list;
};

// function
struct ast *ast_function_init(char *fname, struct ast *shell_command);

// if
struct ast *ast_if_init(struct ast *condition, struct ast *then_body,
                        struct ast *else_body);
// list of command
struct ast *ast_list_init(void);

// boucle (until and while)
struct ast *ast_boucle_init(struct ast *condition, struct ast *do_body,
                            int run_condition);

// for
struct ast *ast_for_init(char *variable);

// redirection
struct ast *ast_redirection_init(int fd, char *word,
                                 enum REDIRECTION_TYPE redirection_type);

// element
struct ast *ast_element_init(enum ELEMENT_TYPE type, char *word,
                             struct ast *ast);

struct ast *ast_shell_cmd_init(struct ast *rule_if);
struct ast *ast_simple_cmd_init(char *word);

struct ast *ast_pipeline_init(int neg, struct ast *cmd);

struct ast *ast_and_or_init(struct ast *pipeline);

struct ast *ast_subshell_init(struct ast *compound_list);

// subshell
int subshell_run(struct ast *ast, struct hash_map *h);
void subshell_free(struct ast *ast);
void subshell_push(struct ast *ast, struct ast *add);

// list ast function
int list_run(struct ast *ast, struct hash_map *h);
void list_free(struct ast *ast);
void list_push(struct ast *ast, struct ast *add);

// and_or ast function
int and_or_run(struct ast *ast, struct hash_map *h);
void and_or_free(struct ast *ast);
void and_or_push(struct ast *ast, struct ast *add);

// if ast function
int if_run(struct ast *ast, struct hash_map *h);
void if_free(struct ast *ast);
void if_push(struct ast *ast, struct ast *add);

// boucle (until and while)
int boucle_run(struct ast *ast, struct hash_map *h);
void boucle_free(struct ast *ast);
void boucle_push(struct ast *ast, struct ast *add);

// redirection
int redirection_run(struct ast *ast, struct hash_map *h);
void redirection_free(struct ast *ast);
void redirection_push(struct ast *ast, struct ast *add);

// element
int element_run(struct ast *ast, struct hash_map *h);
void element_free(struct ast *ast);
void element_push(struct ast *ast, struct ast *add);

// variables
struct ast *ast_variable_init(char *name, char *val);
struct ast *ast_list_init(void);
int variable_run(struct ast *ast, struct hash_map *h);
void variable_free(struct ast *ast);

// simple command
int simple_cmd_run(struct ast *ast, struct hash_map *h);
void simple_cmd_free(struct ast *ast);
void simple_cmd_push(struct ast *ast, struct ast *add);

// shell command
int shell_cmd_run(struct ast *ast, struct hash_map *h);
void shell_cmd_free(struct ast *ast);
void shell_cmd_push(struct ast *ast, struct ast *add);

// pipeline command
int pipeline_run(struct ast *ast, struct hash_map *h);
void pipeline_free(struct ast *ast);
void pipeline_push(struct ast *ast, struct ast *add);

// for
int for_run(struct ast *ast, struct hash_map *h);
void for_free(struct ast *ast);
void for_push(struct ast *ast, struct ast *add);
void for_push_Word(struct ast *ast, char *add);

// function
int function_run(struct ast *ast, struct hash_map *h);
void free_function(struct ast *ast);
void function_push(struct ast *ast, struct ast *add);

// function to expeand all the list of a char
char **expand_all(char **words, size_t nbr, struct hash_map *h);

#endif /* !AST_H */
