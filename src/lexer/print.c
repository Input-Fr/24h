#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static void print(char *string, enum token_type type)
{
    if (type == TOKEN_WORD)
        printf("wd: %s\n", string);
    else if (type == TOKEN_COM)
        printf("co: ...\n");
    else if (type == TOKEN_IF)
        printf("rw: if\n");
    else if (type == TOKEN_THEN)
        printf("rw: then\n");
    else if (type == TOKEN_ELIF)
        printf("rw: elif\n");
    else if (type == TOKEN_ELSE)
        printf("rw: else\n");
    else if (type == TOKEN_FI)
        printf("rw: fi\n");
    else if (type == TOKEN_DO)
        printf("rw: do\n");
    else if (type == TOKEN_DONE)
        printf("rw: done\n");
    else if (type == TOKEN_CASE)
        printf("rw: case\n");
    else if (type == TOKEN_ESAC)
        printf("rw: esac\n");
    else if (type == TOKEN_WHILE)
        printf("rw: while\n");
    else if (type == TOKEN_UNTIL)
        printf("rw: until\n");
    else if (type == TOKEN_FOR)
        printf("rw: for\n");
    else if (type == TOKEN_SEMI)
        printf("se: ;\n");
    else if (type == TOKEN_PIPE)
        printf("rw: |\n");
    else if (type == TOKEN_NEWLINE)
        printf("nl: newline\n");
    else if (type == TOKEN_AND)
        printf("op: &\n");
    else if (type == TOKEN_AND_IF)
        printf("op: &&\n");
    else if (type == TOKEN_OR_IF)
        printf("op: ||\n");
    else if (type == TOKEN_DSEMI)
        printf("op: ;;\n");
    else if (type == TOKEN_DLESS)
        printf("op: <<\n");
    else if (type == TOKEN_DGREAT)
        printf("op: >>\n");
    else if (type == TOKEN_LESSAND)
        printf("op: <&\n");
    else if (type == TOKEN_GREATAND)
        printf("op: >&\n");
    else if (type == TOKEN_LESSGREAT)
        printf("op: <>\n");
    else if (type == TOKEN_DLESSDASH)
        printf("op: <<-\n");
    else if (type == TOKEN_CLOBBER)
        printf("op: >|\n");
    else if (type == TOKEN_LBRACE)
        printf("rw: {\n");
    else if (type == TOKEN_RBRACE)
        printf("rw: }\n");
    else if (type == TOKEN_BANG)
        printf("rw: !\n");
    else if (type == TOKEN_IN)
        printf("rw: in\n");
    else
        printf("?:\n");
}

void print_lex(struct lexer *lexer)
{
    int i = 0;
    while (1)
    {
        struct token tok = lexer_pop(lexer);
        enum token_type type = tok.type;
        char *string = "";
        if (tok.data != NULL)
        {
            string = tok.data->str;
        }

        if (type == TOKEN_EOF)
        {
            printf("EOF\n");
            break;
        }
        print(string, type);
        if (type == TOKEN_WORD)
        {
            free(string);
        }
        i += 1;
    }
    lexer_free(lexer);
}
