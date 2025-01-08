#include <stdio.h>

#include "ast.h"
#include "lexer.h"

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
    struct ast *lst = parse_list(lexer);
    if (*status != PARSER_OK)
    {
        fprintf(stderr, "Do not respect the grammar\n");
        ast_free(res);
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

/*
list = and_or { ';' and_or } [ ';' ] ;
and_or = pipeline ;
pipeline = command ;
command = simple_command
          | shell_command ;

shell_command = rule_if ;
rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;

else_clause = 'else' compound_list
            | 'elif' compound_list 'then' compound_list [else_clause] ;

compound_list = and_or [';'] {'\n'} ;
simple_command = WORD { element } ;
element = WORD;
*/

// list = and_or { ';' and_or } [ ';' ] ;

struct ast *parse_list(enum parser_status *status, struct lexer *lexer)
{

    struct ast *and_or = parse_and_or(status, lexer);
    if (*status != PARSER_OK)
    {
        return NULL;
    }
    struct token tok = lexer_peek(lexer);
    while (tok.type == TOKEN_SEMI)
    {
        lexer_pop(lexer);
        struct parse_and_or(status, lexer);
    }
}

struct ast *parse_and_or(enum parser_status *status, struct lexer *lexer)
{
    return parse_pipeline(lexer);
}

struct ast *parse_pipeline(enum parser_status *status, struct lexer *lexer)
{
    return parse_command(lexer);
}

struct ast *parse_command(enum parser_status *status, struct lexer *lexer)
{
    return parse_simple_command(lexer);
}

struct ast *parse_simple(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_pop(lex);
    if (tok.type == TOKEN_WORD)
    {
        
    }
}

struct ast *parse_element(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_pop(lex);
    if (tok.type == TOKEN_WORD)
    {
        struct ast *ast_word = ast_new(AST_WORD);
        ast_word->value = tok.str;
        return ast_word;
    }
    else
    {
        errx(2, "Grammar not respected");
        // do not respect grammar, invalid word
    }

}
