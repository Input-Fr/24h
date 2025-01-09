#ifndef PARSER_H
#define PARSER_H

#include "lexer/lexer.h"

/* PARSER WILL FOLLOW THE GRAMMAR :

input =
    list '\n'
    | list EOF
    | '\n'
    | EOF;

list = and_or { ';' and_or } [ ';' ] ;
and_or = pipeline ;
pipeline = command ;

command = simple_command 
        | shell_command ;

shell_command = rule_if ;
i
rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;

else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause];

compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;

simple_command = WORD { element } ;
element = WORD ;

  THIS GRAMMAR WILL EVOLVE OVER TIME
*/


enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

static struct ast *parse(enum parser_status *status, struct lexer *lexer);

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

static struct ast *parse_simple(enum parser_status *status, struct lexer *lexer);

static struct ast *parse_element(enum parser_status *status, struct lexer *lexer);

#endif /* ! PARSER_H */
