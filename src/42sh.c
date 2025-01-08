#include <stdio.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "exec_ast/exec.h"

void given_string(char *input)
{
    if (input)
    {
        return;
    }
}

int main(int argc, char *argv[])
{
    if (argc == 3 && !strcmp("-c", argv[1]))
    {
        given_string(argv[2]);
    }

    /const char* string = "if world then hello";
    //const char* string = "#if world \n then hello";
    //const char* string = "if 'world then hello'";
    //const char* string = "if world ; then hello";

    //const char* string = "if 'world' then hello";
    //const char* string = "'if' world 'then hello'";
    //const char* string = "if world 'then' hello";
    //const char* string = "if world then 'hello'";

    //const char* string = "if world;\nthen hello";
    //const char* string = "if world then hello";
    //const char* string = "if #world then\nhello";
    //const char* string = "if #world then hello";
    //const char* string = "if world then hello";

    struct lexer *lex = lexer_new(string);
    if (argc > 1)
    {
        if (strcmp(argv[1],"--print") == 0)
        {
            print(lex);
        }

        if (strcmp(argv[1],"--pretty") == 0)
        {
            pretty_print(lex);
        }
    }
    free(lex);
    return 0;
}
