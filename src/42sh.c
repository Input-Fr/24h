#include <stdio.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "exec_ast/exec.h"

int main()//int argc, char *argv[])
{
    affiche("echo from lexer");
    print_parser();
    exec_ast();
    return 0;
}
