#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"





static FILE * gere_usage(int argc, char *argv[])
{
    if (argc == 1)
    {
        return stdin;
    }
    else
    {
        if (!strcmp(argv[1],"-c"))
        {
            if (argc <= 2)
            {
                return NULL;
            }

            return fmemopen(argv[2], (strlen(argv[2]) + 1),"r");
        }
        else { return fopen(argv[1],"r");
        }
    }

}

static void lexer_file (FILE * hd)
{
    char  chars = 0; 
    while((chars = fgetc(hd)))
    {

        if ( chars != EOF)
        {
            printf("%c",chars);
        }
        else
        {
            break;
        }
    }
}

int main(int argc, char *argv[])
{


    // get the sh code
    FILE * value = gere_usage(argc,argv);
    
    if (value == NULL)
    {
        return -1;
    }
    // start of the process of lexing
    lexer_file(value);
    return 0;
}
