#ifndef PARSER_H
#define PARSER_H

/** PARSER WILL FOLLOW THE GRAMMAR :

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

struct ast *parse(enum parser_status *status, struct lexer *lexer);

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

#endif /* ! PARSER_H */
