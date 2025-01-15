#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "ast.h"

#define RUN(AST) (*(AST)->ftable->run)((AST))

enum REDIRECTION_TYPE
{
    LEFT_DUP,// <
    LEFT_APPEND, // <<
    RIGHT_DUP, // >
    RIGHT_APPEND, // >>
}

struct dico_redirection
{
    
}
static void printWbackslash(char *carg)
{
    size_t idx = 0;
    while (carg[idx])
    {
        if (carg[idx] == '\\')
        {
            idx++;
            if (carg[idx] == 'n')
            {
                printf("\n");
            }
            else if (carg[idx] == 't')
            {
                printf("\t");
            }
            else if (carg[idx] == '\\')
            {
                printf("\\");
            }
            else // connais pas
            {
                printf("\\");
                printf("%c", carg[idx]);
            }
        }
        else
        {
            printf("%c", carg[idx]);
        }
        idx++;
    }
}

// args est de la forme ["arg1", "arg2", "arg3"]
void echo_builtin(char *args[], size_t nb_args)
{
    bool newline = true;
    bool backslash = false;
    size_t i = 0;
    while (i < nb_args && args[i][0] == '-')
    {
        if (!strcmp(args[i], "-n"))
        {
            newline = false;
        }
        else if (!strcmp(args[i], "-e"))
        {
            backslash = true;
        }
        else if (!strcmp(args[i], "-E"))
        {
            backslash = false;
        }
        else
        {
            // pas une option juste afficher
            break;
        }
        i++;
    }

    // echo les arguments
    while (i < nb_args)
    {
        if (backslash)
        {
            char *cur_arg = args[i];
            printWbackslash(cur_arg);
        }
        else
        {
            printf("%s", args[i]);
        }
        if (i < nb_args - 1) // on sépare tout les argument d'un espace
        {
            printf(" ");
        }
        i++;
    }

    if (newline)
    {
        printf("\n");
    }

    fflush(stdout);
}

// for three evaluation

// list ast eval
int list_run(struct ast *ast)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    size_t i = 0;
    while (i < list->nbr_cmd && !RUN(list->cmd[i]))
    {
        i += 1;
    }
    return !(i >= list->nbr_cmd);
}


/*
// cmd ast eval
int cmd_run(struct ast *ast)
{
    assert(ast && ast->type == AST_COMMAND);
    struct ast_cmd *cmd = (struct ast_cmd *)ast;
    if (!cmd->words)
    {
        return 2;
    }
    else
    {
        if (!strcmp(cmd->words[0], "echo"))
        {
            int idx = 1;
            while (cmd->words[idx])
            {
                idx++;
            }
            echo_builtin(cmd->words + 1, idx - 1);
        }
        else if (!strcmp(cmd->words[0], "true"))
        {
            return 0;
        }
        else if (!strcmp(cmd->words[0], "false"))
        {
            return 1;
        }
        else
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                int status_code = execvp(cmd->words[0], cmd->words);
                if (status_code == -1)
                {
                    exit(2);
                }
            }
            int wstatus;
            waitpid(pid, &wstatus, 0);
            int return_value = WEXITSTATUS(wstatus);
            if (return_value == 2)
            {
                errx(2, "Terminated Incorrectly\n");
            }
        }
        return 0;
    }
} */

// if ast eval
int if_run(struct ast *ast)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if *if_ast = (struct ast_if *)ast;
    if (!RUN(if_ast->condition))
    {
        return RUN(if_ast->then_body);
    }
    else if (!if_ast->else_body)
    {
        return 0;
    }
    else
    {
        return RUN(if_ast->else_body);
    }
}



// boucle (until and while) ast eval 
int boucle_run(struct ast * ast)
{
    assert(ast && ast->type == AST_BOUCLE);
    struct ast_boucle * boucle = (struct ast_boucle *) ast;
    int res = 0;
    while (RUN(boucle->condition) == boucle->run_condition)
    {
        res = RUN(boucle->do_body);
    }
    return res;
}

// redirection ast eval
int redirection_run(struct ast * ast)
{
    assert(ast && ast->type == AST_REDIRECTION);
    struct ast_redirection * redi = (struct ast_redirection *)ast;
   return 1;
}

// element ast eval
int element_run(struct ast * ast)
{
    assert(ast && ast->type == AST_ELEMENT);  
    struct ast_element * elt = (struct ast_element *)ast;
    if (elt->type == WORD)
    {
        return 0;
    }
    else
    {
        return RUN(elt->elt->redirection);
    }
}

int shell_cmd_run(struct ast * ast)
{
    assert(ast && ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd * cmd = (struct ast_shell_cmd * ) ast;
    int i = 0;
    for (int i = 0; i < cmd->nbr_redirection; i++)
    {
         i = RUN(cmd->redirection[i]);
    }
    i = RUN(cmd->rule);
    return i;
}

