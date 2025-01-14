#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
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

static struct ast *handle_elif(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_else_clause(enum parser_status *status,
                                     struct lexer *lexer);

static struct ast *parse_compound_list(enum parser_status *status,
                                       struct lexer *lexer);

static struct ast *parse_simple_command(enum parser_status *status,
                                        struct lexer *lexer);

static char *parse_element(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_prefix(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_redirection(enum parser_status *status, struct lexer *lexer);

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
and_or = pipeline ;
*/
static struct ast *parse_and_or(enum parser_status *status, struct lexer *lexer)
{
    return parse_pipeline(status, lexer);
}

/*
pipeline = command ;
*/
static struct ast *parse_pipeline(enum parser_status *status,
                                  struct lexer *lexer)
{
    return parse_command(status, lexer);
}

/*
command = simple_command
          | shell_command { redirection } ;
*/
static struct ast *parse_command(enum parser_status *status,
                                 struct lexer *lexer)
{
    // TODO
    struct ast *ast_simplec = parse_simple_command(status, lexer);
    if (*status == PARSER_OK)
    {
        return ast_simplec;
    }
    *status = PARSER_OK;
    struct ast *ast_shellc = parse_shell_command(status, lexer);
    if (*status == PARSER_OK)
    {
        struct ast *ast_redir = parse_redirection(status, lexer);
        while (*status == PARSER_OK)
        {
            // add to ast
            (void)ast_redir;
            ast_redir = parse_redirection(status, lexer);
        }
        *status = PARSER_OK;
        return ast_shellc;
    }
    return NULL;
}

/*
shell_command = rule_if ;
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
    return parse_rule_until(status, lexer);
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
    // TODO
    (void)status;
    (void)lexer;
    return NULL;
}

// rule_until = 'until' compound_list 'do' compound_list 'done' ;
static struct ast *parse_rule_until(enum parser_status *status,
                                 struct lexer *lexer)
{
    // TODO
    (void)status;
    (void)lexer;
    return NULL;
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
    struct ast *ast_prefix = 
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_WORD)
    {
        lexer_pop(lexer);
        int nb_words = 2; // une liste de mot de taille 2
        char **words = calloc(nb_words, sizeof(char *));
        words[nb_words - 2] = tok.data->str;
        char *word = parse_element(status, lexer);
        while (*status == PARSER_OK)
        {
            nb_words++;
            words = realloc(words, nb_words * sizeof(char *));
            words[nb_words - 1] = 0;
            words[nb_words - 2] = word;
            word = parse_element(status, lexer);
        }
        *status = PARSER_OK;
        return ast_cmd_init(words);
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
}

/*
element = WORD
        | redirection ;
*/

static char *parse_element(enum parser_status *status, struct lexer *lexer)
{
    // TODO
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_WORD)
    {
        lexer_pop(lexer);
        return tok.data->str;
    }
    else
    {
        // do not respect grammar, invalid word
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
}

// prefix = redirection ;
static struct ast *parse_prefix(enum parser_status *status, struct lexer *lexer)
{
    return parse_redirection(status, lexer);
}

static int isnum(const char *str)
{
    char *endptr;
    if (*str == 0) // nan
        return 0;

    strtol(str, &endptr, 10);
    return *endptr == 0;
}

static int redir_op(struct token tok)
{
    return tok.type == TOKEN_LESS || tok.type == TOKEN_GREAT 
        || tok.type == TOKEN_DGREAT || tok.type == TOKEN_LESSAND 
        || tok.type == TOKEN_GREATAND || tok.type == TOKEN_CLOBBER
        || tok.type == TOKEN_LESSGREAT;
}

// redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' ) WORD ;
static struct ast *parse_redirection(enum parser_status *status, struct lexer *lexer)
{
    // TODO
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_WORD && isnum(tok.data->str) 
            && in_list(list_fd(), atoi(tok.data->str)))
    {
        int fd = atoi(tok.data->str); // the fd
        printf("redir found : %d\n", fd);
        lexer_pop(lexer);
    }
    tok = lexer_peek(lexer);
    if (redir_op(tok))
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
        if (tok.type == TOKEN_WORD)
        {
            printf("%s\n", tok.data->str); // the word
            // build the AST
            lexer_pop(lexer);
            return NULL; // return the AST
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
