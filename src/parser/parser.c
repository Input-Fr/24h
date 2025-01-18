#define _POSIX_C_SOURCE 200809L
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>

#include "../lexer/lexer.h"
#include "ast.h"

#define PATH_MAX 4096
static struct ast *parse_list(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_and_or(enum parser_status *status,
                                struct lexer *lexer);

static struct ast *parse_pipeline(enum parser_status *status,
                                  struct lexer *lexer);

static struct ast *parse_command(enum parser_status *status,
                                 struct lexer *lexer);

static struct ast *parse_shell_command(enum parser_status *status,
                                       struct lexer *lexer);

static struct ast *handle_then(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_rule_if(enum parser_status *status,
                                 struct lexer *lexer);

static struct ast *parse_rule_while(enum parser_status *status,
                                 struct lexer *lexer);

static struct ast *parse_rule_until(enum parser_status *status,
                                 struct lexer *lexer);

static struct ast *parse_rule_for(enum parser_status *status,
                                 struct lexer *lexer);

static struct ast *handle_elif(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_else_clause(enum parser_status *status,
                                     struct lexer *lexer);

static struct ast *parse_compound_list(enum parser_status *status,
                                       struct lexer *lexer);

static struct ast *parse_simple_command(enum parser_status *status,
                                        struct lexer *lexer);

static struct ast *parse_element(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_prefix(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_redirection(enum parser_status *status, struct lexer *lexer);


/*
input = list '\n'
    | list EOF
    | '\n'
    | EOF
    ;
*/
struct ast *parse(enum parser_status *status, struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_ERROR)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    if (lexer_peek(lexer).type == TOKEN_EOF)
    {
        lexer_pop(lexer);
        return NULL;
    }
    if (lexer_peek(lexer).type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        return NULL;
    }
    struct ast *lst = parse_list(status, lexer);
    if (*status != PARSER_OK)
    {
        fprintf(stderr, "Do not respect the grammar\n");
    }
    else // it can respect the grammar
    {
        if (lexer_peek(lexer).type == TOKEN_EOF
            || lexer_peek(lexer).type == TOKEN_NEWLINE) // end of file
        {
            return lst;
        }
        else
        {
            fprintf(stderr, "it does not respect the grammar\n");
            *status = PARSER_UNEXPECTED_TOKEN;
            (*lst->ftable->free)(lst);
        }
    }
    return NULL;
}

// list = and_or { ';' and_or } [ ';' ] ;
static struct ast *parse_list(enum parser_status *status, struct lexer *lexer)
{
    // TODO
    struct ast *and_or = parse_and_or(status, lexer);
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    struct token tok = lexer_peek(lexer);
    struct ast *ast_list = ast_list_init();
    list_push(ast_list, and_or);
    while (tok.type == TOKEN_SEMI)
    {
        lexer_pop(lexer);
        struct ast *snd_and_or = parse_and_or(status, lexer);
        if (*status != PARSER_OK)
        {
            *status = PARSER_OK;
            return ast_list;
        }
        list_push(ast_list, snd_and_or);
        tok = lexer_peek(lexer);
    }
    return ast_list;
}

/*
and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline } ;
*/
static struct ast *push_and_or(struct ast *and_or, struct token op, struct ast *right)
{
    struct ast_and_or *new = (struct ast_and_or*)ast_and_or_init(NULL);
    new->t = NODE_AND_OR;
    new->c.op = calloc(1, sizeof(struct operation));
    new->c.op->left = and_or;
    new->c.op->right = right;
    if (op.type == TOKEN_AND_IF)
    {
        new->c.op->op = AND_OP;
    }
    else
    {
        new->c.op->op = OR_OP;
    }
    return &new->base;
}
static struct ast *parse_and_or(enum parser_status *status, struct lexer *lexer)
{
    struct ast *ast_pipe = parse_pipeline(status, lexer);
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    struct ast *ast_and_or = ast_and_or_init(ast_pipe);
    struct token tok = lexer_peek(lexer);
    while(tok.type == TOKEN_AND_IF || tok.type == TOKEN_OR_IF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
        while(tok.type == TOKEN_NEWLINE)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer);
        }
        struct ast *ast_pipe = parse_pipeline(status, lexer);
        if (*status != PARSER_OK)
        {
            return NULL;
        }
        ast_and_or = push_and_or(ast_and_or, tok, ast_pipe);
        tok = lexer_peek(lexer);
    }
    return ast_and_or;
}

/*
pipeline =  ['!'] command { '|' {'\n'} command } ;
*/
static struct ast *parse_pipeline(enum parser_status *status,
                                  struct lexer *lexer)
{
    int neg = 0;
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_BANG)
    {
        lexer_pop(lexer);
        neg = 1;
    }
    struct ast *cmd = parse_command(status, lexer);
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    struct ast *ast_pipeline = ast_pipeline_init(neg, cmd);
    pipeline_push(ast_pipeline, cmd);
    tok = lexer_peek(lexer);
    while(tok.type == TOKEN_PIPE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
        while(tok.type == TOKEN_NEWLINE)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer);
        }
        cmd = parse_command(status, lexer);
        if (*status != PARSER_OK)
        {
            return NULL;
        }
        pipeline_push(ast_pipeline, cmd);
        tok = lexer_peek(lexer);
    }
    return ast_pipeline;
}

/*
command = simple_command
          | shell_command { redirection } ;
*/
static struct ast *parse_command(enum parser_status *status,
                                 struct lexer *lexer)
{
    struct ast *ast_simplec = parse_simple_command(status, lexer);
    if (*status == PARSER_OK)
    {
        return ast_simplec;
    }
    *status = PARSER_OK;
    struct ast *ast_rule = parse_shell_command(status, lexer);
    if (*status == PARSER_OK)
    {
        struct ast *ast_shellcmd = ast_shell_cmd_init(ast_rule);
        struct ast *ast_redir = parse_redirection(status, lexer);
        while (*status == PARSER_OK)
        {
            shell_cmd_push(ast_shellcmd, ast_redir);
            ast_redir = parse_redirection(status, lexer);
        }
        *status = PARSER_OK;
        return ast_shellcmd;
    }
    return NULL;
}

/*
shell_command =
rule_if
| rule_while
| rule_until
| rule_for ;

*/
static struct ast *parse_shell_command(enum parser_status *status,
                                       struct lexer *lexer)
{
    struct ast *ast_rule = parse_rule_if(status, lexer);
    if (*status != PARSER_OK)
    {
        *status = PARSER_OK;
    }
    else
    {
        return ast_rule;
    }

    ast_rule = parse_rule_while(status, lexer);
    if (*status != PARSER_OK)
    {
        *status = PARSER_OK;
    }
    else
    {
        return ast_rule;
    }

    ast_rule = parse_rule_until(status, lexer);
    if (*status != PARSER_OK)
    {
        *status = PARSER_OK;
    }
    else
    {
        return ast_rule;
    }
    return parse_rule_for(status, lexer);
}

/*
rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
*/
static struct ast *handle_then(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_THEN)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    else
    {
        lexer_pop(lexer);
        struct ast *compound = parse_compound_list(status, lexer);
        if (*status != PARSER_OK)
        {
            return NULL;
        }
        return compound;
    }
}

static struct ast *parse_rule_if(enum parser_status *status,
                                 struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_IF)
    {
        lexer_pop(lexer);
        struct ast *compound = parse_compound_list(status, lexer);
        if (*status != PARSER_OK)
        {
            return NULL;
        }
        struct ast *ast_then = handle_then(status, lexer);
        if (*status != PARSER_OK)
        {
            (*compound->ftable->free)(compound);
            return NULL;
        }
        struct ast *ast_else_clause = parse_else_clause(status, lexer);
        if (*status != PARSER_OK)
        {
            *status = PARSER_OK;
        }
        tok = lexer_pop(lexer);
        if (tok.type == TOKEN_FI)
        {
            return ast_if_init(compound, ast_then, ast_else_clause);
        }
        else
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            (*compound->ftable->free)(compound);
            (*ast_then->ftable->free)(ast_then);
            if (ast_else_clause)
            {
                (*ast_else_clause->ftable->free)(ast_else_clause);
            }
            return NULL;
        }
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
}

// rule_while = 'while' compound_list 'do' compound_list 'done' ;
static struct ast *parse_rule_while(enum parser_status *status,
                                 struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_WHILE)
    {
        lexer_pop(lexer);
        struct ast *ast_cmpd_lst = parse_compound_list(status, lexer);
        if (*status != PARSER_OK)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
        tok = lexer_pop(lexer);
        if (tok.type == TOKEN_DO)
        {
            struct ast *ast_cmpd_lst2 = parse_compound_list(status, lexer);
            if (*status != PARSER_OK)
            {
                (*ast_cmpd_lst->ftable->free)(ast_cmpd_lst);
                return NULL;
            }
            tok = lexer_pop(lexer);
            if (tok.type == TOKEN_DONE)
            {
                return ast_boucle_init(ast_cmpd_lst, ast_cmpd_lst2, 0);
            }
            else
            {
                (*ast_cmpd_lst->ftable->free)(ast_cmpd_lst);
                (*ast_cmpd_lst2->ftable->free)(ast_cmpd_lst2);
                *status = PARSER_UNEXPECTED_TOKEN;
                return NULL;
            }
        }
        else
        {
            (*ast_cmpd_lst->ftable->free)(ast_cmpd_lst);
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    return NULL;
}

// rule_until = 'until' compound_list 'do' compound_list 'done' ;
static struct ast *parse_rule_until(enum parser_status *status,
                                 struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_UNTIL)
    {
        lexer_pop(lexer);
        struct ast *ast_cmpd_lst = parse_compound_list(status, lexer);
        if (*status != PARSER_OK)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
        tok = lexer_pop(lexer);
        if (tok.type == TOKEN_DO)
        {
            struct ast *ast_cmpd_lst2 = parse_compound_list(status, lexer);
            if (*status != PARSER_OK)
            {
                (*ast_cmpd_lst->ftable->free)(ast_cmpd_lst);
                return NULL;
            }
            tok = lexer_pop(lexer);
            if (tok.type == TOKEN_DONE)
            {
                return ast_boucle_init(ast_cmpd_lst, ast_cmpd_lst2, 1);
            }
            else
            {
                (*ast_cmpd_lst->ftable->free)(ast_cmpd_lst);
                (*ast_cmpd_lst2->ftable->free)(ast_cmpd_lst2);
                *status = PARSER_UNEXPECTED_TOKEN;
                return NULL;
            }
        }
        else
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    return NULL;
}

// rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] ) 
//            {'\n'} 'do' compound_list 'done' ;

// to handle : [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ]
static void for_parenthesis(enum parser_status *status, struct lexer *lexer,
                            struct ast *ast_for)
{
    // TODO
    (void)ast_for;
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_SEMI)
    {
        return; // nothing to return
    }
    else
    {
        while(tok.type == TOKEN_NEWLINE)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer);
        }
        if (tok.type != TOKEN_IN)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return;
        }
        else
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer);
            while(tok.type == TOKEN_WORD)
            {
                // add word to ast for iteration list
                lexer_pop(lexer);
                tok = lexer_peek(lexer);
            }
            if (tok.type != TOKEN_SEMI && tok.type != TOKEN_NEWLINE)
            {
                *status = PARSER_UNEXPECTED_TOKEN;
                return;
            }
            lexer_pop(lexer);
        }
    }
}

// to handle : compound_list 'done'
static void handle_end(enum parser_status *status, struct lexer *lexer,
                            struct ast *ast_for)
{
    // TODO
    (void)ast_for;
    struct ast *ast_cmpd_lst = parse_compound_list(status, lexer);
    (void)ast_cmpd_lst;
    if (*status != PARSER_OK)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return;
    }
    // add cmpd list to exec part of 'for'
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_DONE)
    {
        lexer_pop(lexer);
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return;
    }

}

static struct ast *parse_rule_for(enum parser_status *status,
                                 struct lexer *lexer)
{
    // TODO
    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_FOR)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    lexer_pop(lexer);
    tok = lexer_pop(lexer);
    if (tok.type != TOKEN_WORD)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    // add word (tok.data->str) to variable name
    for_parenthesis(status, lexer, NULL); // replace null with AST
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    tok = lexer_peek(lexer);
    while(tok.type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
    }
    if (tok.type == TOKEN_DO)
    {
        lexer_pop(lexer);
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }

    handle_end(status, lexer, NULL); // replace null with AST
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    return NULL; // return AST
}

/*
else_clause = 'else' compound_list
            | 'elif' compound_list 'then' compound_list [else_clause] ;
*/
static struct ast *handle_elif(enum parser_status *status, struct lexer *lexer)
{
    lexer_pop(lexer);
    struct ast *compound_list = parse_compound_list(status, lexer);
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    struct token tok = lexer_pop(lexer);
    if (tok.type != TOKEN_THEN)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    struct ast *compound_list2 = parse_compound_list(status, lexer);
    if (*status != PARSER_OK)
    {
        (*compound_list->ftable->free)(compound_list);
        return NULL;
    }
    struct ast *ast_else = parse_else_clause(status, lexer);
    if (*status != PARSER_OK)
    {
        *status = PARSER_OK;
    }
    struct ast *ast_elif = ast_if_init(compound_list, compound_list2, ast_else);
    return ast_elif;
}

static struct ast *parse_else_clause(enum parser_status *status,
                                     struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_ELSE)
    {
        lexer_pop(lexer);
        struct ast *compound = parse_compound_list(status, lexer);
        if (*status != PARSER_OK)
        {
            return NULL;
        }
        return compound;
    }
    else if (tok.type == TOKEN_ELIF)
    {
        struct ast *ast_elif = handle_elif(status, lexer);
        if (*status != PARSER_OK)
        {
            (*ast_elif->ftable->free)(ast_elif);
            return NULL;
        }
        return ast_elif;
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
}

/*
compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
*/

static struct ast *parse_compound_list(enum parser_status *status,
                                       struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    while (tok.type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
    }
    struct ast *ast_and_or = parse_and_or(status, lexer);
    if (*status != PARSER_OK)
    {
        return NULL;
    }

    struct ast *compound_list = ast_list_init();
    list_push(compound_list, ast_and_or);
    tok = lexer_peek(lexer);
    while (tok.type == TOKEN_SEMI || tok.type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
        while (tok.type == TOKEN_NEWLINE)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer);
        }
        struct ast *snd_ast_and_or = parse_and_or(status, lexer);
        if (*status != PARSER_OK)
        {
            *status = PARSER_OK;
            break;
        }
        list_push(compound_list, snd_ast_and_or);
    }
    tok = lexer_peek(lexer);
    if (tok.type == TOKEN_SEMI)
    {
        lexer_pop(lexer);
    }
    tok = lexer_peek(lexer);
    while (tok.type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
    }
    return compound_list;
}

/*
simple_command = prefix { prefix }
               | { prefix } WORD { element } ;
*/
static struct ast *parse_simple_command(enum parser_status *status,
                                        struct lexer *lexer)
{
    // TODO
    struct ast *ast_prefix = parse_prefix(status, lexer);
    struct ast *smpcmd = ast_simple_cmd_init(NULL);
    while (*status == PARSER_OK)
    {
        // put ast_prefix in list
        simple_cmd_push(smpcmd, ast_prefix);
        ast_prefix = parse_prefix(status, lexer);
    }
    *status = PARSER_OK;
    struct token tok = lexer_peek(lexer);
    if (!ast_prefix)
    {
        if (tok.type != TOKEN_WORD)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
        else // second case
        {
            ((struct ast_simp_cmd *)smpcmd)->word = tok.data->str;
            lexer_pop(lexer);
            struct ast *ast_elt = parse_element(status, lexer);
            (void)ast_elt;
            while(*status == PARSER_OK)
            {
                // add elt to ast
                simple_cmd_push(smpcmd, ast_elt);
                ast_elt = parse_element(status, lexer);
            }
            *status = PARSER_OK;
        }
    }
    else
    {
        if (tok.type == TOKEN_WORD) // second case
        {
            ((struct ast_simp_cmd *)smpcmd)->word = tok.data->str;
            lexer_pop(lexer);
            // add word to ast
            struct ast *ast_elt = parse_element(status, lexer);
            while(*status == PARSER_OK)
            {
                // add elt to ast
                simple_cmd_push(smpcmd, ast_elt);
                ast_elt = parse_element(status, lexer);
            }
            *status = PARSER_OK;
        }
    }
    return smpcmd; // return the AST
}

/*
element = WORD
        | redirection ;
*/
static int redir_op(struct token tok);

static int isnum(const char *str);

static struct ast *parse_element(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_WORD)
    {
        char *str = tok.data->str;
        lexer_pop(lexer);
        if (redir_op(lexer_peek(lexer)))
        {
            struct ast *ast_redir = parse_redirection(status, lexer);
            if (*status == PARSER_OK)
            {
                // ((struct ast_redirection *)ast_redir)->n = atoi(str);
                free(str);
                return ast_element_init(REDIRECTION, NULL, ast_redir);
            }
            else
            {
                return NULL; // invalid
            }

        }
        else
        {
            return ast_element_init(WORD, str, NULL);
        }
    }
    struct ast *ast_redir = parse_redirection(status, lexer);
    if (*status == PARSER_OK)
    {
        return ast_element_init(REDIRECTION, NULL, ast_redir);
    }
    else
    {
        return NULL; // invalid
    }
}

// prefix = redirection ;
static struct ast *parse_prefix(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_ASSIGNMENT_WORD)
    {
        return ast_variable_init(NULL, NULL);
    }
    return parse_redirection(status, lexer);
}


// redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' ) WORD ;


static enum REDIRECTION_TYPE strop(struct token op);

static int valid_fd(int fd);

static struct ast *parse_redirection(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    int fd = -1;
    if (tok.type == TOKEN_WORD && isnum(tok.data->str) 
            && valid_fd(atoi(tok.data->str)))
    {
        fd = atoi(tok.data->str); // the fd
        free(tok.data->str);
        lexer_pop(lexer);
    }
    tok = lexer_peek(lexer);
    if (redir_op(tok))
    {
        lexer_pop(lexer);
        struct token tok2 = lexer_peek(lexer);
        if (tok2.type == TOKEN_WORD)
        {
            struct ast *ast_redir = ast_redirection_init(fd, tok2.data->str, strop(tok));
            lexer_pop(lexer);
            return ast_redir; // return the AST
        }
        else
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL; 
        }
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
}

// FONCTIONS ANNEXES REDIR

static enum REDIRECTION_TYPE strop(struct token op)
{
    if (op.type == TOKEN_LESS)
    {
        return LESS;
    }
    else if (op.type == TOKEN_GREAT)
    {
        return GREATER;
    }
    else if (op.type == TOKEN_DGREAT)
    {
        return DGREATER;
    }
    else if (op.type == TOKEN_DLESS)
    {
        return LESS; // not to implement yet
    }
    else if (op.type == TOKEN_LESSAND)
    {
        return LESS_AND;
    }
    else if (op.type == TOKEN_GREATAND)
    {
        return GREATER_AND;
    }
    else if (op.type == TOKEN_CLOBBER)
    {
        return CLOBBER;
    }
    else
    {
        return LESS_GREATER;
    }
}

static int redir_op(struct token tok)
{
    return tok.type == TOKEN_LESS || tok.type == TOKEN_GREAT 
        || tok.type == TOKEN_DGREAT || tok.type == TOKEN_LESSAND 
        || tok.type == TOKEN_GREATAND || tok.type == TOKEN_CLOBBER
        || tok.type == TOKEN_LESSGREAT;
}

static int isnum(const char *str)
{
    char *endptr;
    if (*str == 0) // nan
        return 0;

    strtol(str, &endptr, 10);
    return *endptr == 0;
}
static int valid_fd(int fd)
{
    return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}
/*
static int *list_fd(void)
{
    pid_t pid = getpid();

    int *fd_lst = calloc(1024, sizeof(int));

    char proc_path[PATH_MAX];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/fd", pid);

    DIR *dir = opendir(proc_path);
    if (!dir)
    {
        perror("Failed to open /proc/{pid}/fd");
        return NULL;
    }

    struct dirent *entry;
    size_t arr_len = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        fd_lst[arr_len] = atoi(entry->d_name);
        arr_len++;
    }

    closedir(dir);
    return fd_lst;
}

static int in_list(int *fd_lst, int fd)
{
    size_t i = 0;
    while (fd_lst[i])
    {
        if (fd_lst[i] == fd)
        {
            return 1;
        }
    }
    return 0;
}
*/
// -------------------------------------------
