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
    return 0;
}
