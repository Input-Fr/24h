#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

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

rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;

else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause];

compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;

simple_command = WORD { element } ;
element = WORD ;

  THIS GRAMMAR WILL EVOLVE OVER TIME
*/

/* GRAMMAR STEP 2

input =
    list '\n'
    | list EOF
    | '\n'
    | EOF;

list = and_or { ';' and_or } [ ';' ] ;

and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline } ;

pipeline = ['!'] command { '|' {'\n'} command };

command = simple_command
        | shell_command { redirection };

shell_command = rule_if
                | rule_while
                | rule_until
                | rule_for ;

rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
rule_while = 'while' compound_list 'do' compound_list 'done' ;
rule_until = 'until' compound_list 'do' compound_list 'done' ;
rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
            {'\n'} 'do' compound_list 'done' ;

else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause];

compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;

simple_command = prefix { prefix }
             | { prefix } WORD { element } ;

element = WORD
        | redirection ;

prefix = ASSIGNMENT_WORD
        | redirection ;

redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' ) WORD ;

*/
/* GRAMMAR STEP 3

input =
    list '\n'
    | list EOF
    | '\n'
    | EOF;

list = and_or { ';' and_or } [ ';' ] ;

and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline } ;

pipeline = ['!'] command { '|' {'\n'} command };

command = simple_command
        | shell_command { redirection };

shell_command = '{' compound_list '}'
                | rule_if
                | rule_while
                | rule_until
                | rule_for ;

rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
rule_while = 'while' compound_list 'do' compound_list 'done' ;
rule_until = 'until' compound_list 'do' compound_list 'done' ;
rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
            {'\n'} 'do' compound_list 'done' ;

else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause];

compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;

simple_command = prefix { prefix }
             | { prefix } WORD { element } ;

element = WORD
        | redirection ;

prefix = ASSIGNMENT_WORD
        | redirection ;

redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' ) WORD ;

*/
enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

struct ast *parse(enum parser_status *status, struct lexer *lexer);
char *reserved_words_to_char(struct lexer *lexer);
int is_reserved_words(struct lexer *lexer);

#endif /* ! PARSER_H */
