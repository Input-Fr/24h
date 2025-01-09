#include "parser.h"

#include <stdio.h>

#include "ast.h"
#include "lexer/lexer.h"

static struct ast *parse_list(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_and_or(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_pipeline(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_command(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_shell_command(enum parser_status *status, struct lexer *lexer);

static struct ast *handle_then(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_rule_if(enum parser_status *status, struct lexer *lexer);

static struct ast *handle_elif(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_else_clause(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_compound_list(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_simple_command(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_element(enum parser_status *status, struct lexer *lexer);


static void add_elt_to_list(struct ast *base_node, struct ast *elt)
{
    struct ast *traveler = base_node;
    while (traveler->cList)
    {
        traveler = traveler->cList;
    }
    traveler->cList = elt;
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
        ast_free(lst);
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
            ast_free(lst);
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
    struct ast *ast_list = ast_new(AST_LIST);
    add_elt_to_list(ast_list, and_or);
    while (tok.type == TOKEN_SEMI)
    {
        lexer_pop(lexer);
        struct ast *snd_and_or = parse_and_or(status, lexer);
        if (*status != PARSER_OK)
        {
            ast_free(and_or);
            return NULL;
        }
        add_elt_to_list(ast_list, snd_and_or);
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
static struct ast *parse_pipeline(enum parser_status *status, struct lexer *lexer)
{
    return parse_command(status, lexer);
}

/*
command = simple_command
          | shell_command ;
*/
static struct ast *parse_command(enum parser_status *status, struct lexer *lexer)
{
    struct ast *ast_simplec = parse_simple_command(status, lexer);
    if (*status == PARSER_OK)
    {
        return ast_simplec;
    }
    *status = PARSER_OK;
    struct ast *ast_shellc = parse_shell_command(status, lexer);
    if (*status == PARSER_OK)
    {
        return ast_shellc;
    }
    return NULL;
}

/*
shell_command = rule_if ;
*/
static struct ast *parse_shell_command(enum parser_status *status, struct lexer *lexer)
{
    return parse_rule_if(status, lexer);
}

/*
rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
*/
static struct ast *handle_then(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_pop(lexer);
    if (tok.type != TOKEN_THEN)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    else
    {
        struct ast *compound = parse_compound_list(status, lexer);
        if (*status != PARSER_OK)
        {
            return NULL;
        }
        struct ast *ast_then = ast_new(AST_THEN);
        add_elt_to_list(ast_then, compound);
        return ast_then;
    }
}

static struct ast *parse_rule_if(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_pop(lexer);
    if (tok.type == TOKEN_IF)
    {
        struct ast *compound = parse_compound_list(status, lexer);
        if (*status != PARSER_OK)
        {
            return NULL;
        }
        struct ast *ast_if = ast_new(AST_IF);
        add_elt_to_list(ast_if, compound);
        struct ast *ast_then = handle_then(status, lexer);
        if (*status != PARSER_OK)
        {
            ast_free(ast_if);
            return NULL;
        }
        ast_if->left = ast_then;
        struct ast *ast_else_clause = parse_else_clause(status, lexer);
        if (*status == PARSER_OK)
        {
            ast_if->right = ast_else_clause;
        }
        else
        {
            *status = PARSER_OK;
        }
        tok = lexer_pop(lexer);
        if (tok.type == TOKEN_FI)
        {
            return ast_if;
        }
        else
        {
            ast_free(ast_if);
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
        return NULL;
    }
    struct ast *ast_elif = ast_new(AST_ELIF);
    add_elt_to_list(ast_elif, compound_list);
    struct ast *ast_then = ast_new(AST_THEN);
    add_elt_to_list(ast_then, compound_list2);
    ast_elif->left = ast_then;
    struct ast *ast_else = parse_else_clause(status, lexer);
    ast_elif->right = ast_else;
    if (*status != PARSER_OK)
    {
        *status = PARSER_OK;
    }
    return ast_elif;
}

static struct ast *parse_else_clause(enum parser_status *status, struct lexer *lexer)
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
        struct ast *ast_else = ast_new(AST_ELSE);
        add_elt_to_list(ast_else, compound);
        return ast_else;
    }
    else if (tok.type == TOKEN_ELIF)
    {
        struct ast *ast_elif = handle_elif(status, lexer);
        if (*status != PARSER_OK)
        {
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
compound_list = and_or [';'] {'\n'} ;
*/
static struct ast *parse_compound_list(enum parser_status *status, struct lexer *lexer)
{
    struct ast *and_or = parse_and_or(status, lexer);
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_SEMI)
    {
        lexer_pop(lexer);
    }
    tok = lexer_peek(lexer);
    while (tok.type == TOKEN_NEWLINE)
    {
        tok = lexer_pop(lexer);
    }
    return and_or;
}

/*
simple_command = WORD { element } ;
element = WORD;
*/
static struct ast *parse_simple_command(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_WORD)
    {
       lexer_pop(lexer);
       struct ast *command_ast = ast_new(AST_COMMAND);
       struct ast *lst_elt = ast_new(AST_WORD);
       lst_elt->value = tok.str;
       command_ast->cList = lst_elt;
       struct ast *elt = parse_element(status, lexer);
       while (*status == PARSER_OK)
       {
            add_elt_to_list(command_ast, elt);
            elt = parse_element(status, lexer);
       }
       *status = PARSER_OK;
       return command_ast;
    }
    else
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
}

static struct ast *parse_element(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_WORD)
    {
        lexer_pop(lexer);
        struct ast *ast_word = ast_new(AST_WORD);
        ast_word->value = tok.str;
        return ast_word;
    }
    else
    {
        // do not respect grammar, invalid word
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }

}
