#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc == 0)
    {
        return 0;
    }

    //const char* string = "if world then hello";
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
