#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include "ast.h"

#define RUN(AST) (*(AST)->ftable->run)((AST))

// FONCTION ANNEXE REDIR
struct s_redirection
{
    int original_fd;
    int saved_fd;
    struct s_redirection *next;
};

// liste globale des fd sauvegardés
struct s_redirection *s_redir = NULL;

static void restore(void);

static int handle_redirection(int fd, enum REDIRECTION_TYPE redir_op, char *word);

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
static void echo_builtin(char *args[], size_t nb_args)
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
    int ret = handle_redirection(redi->n, redi->redir_op, redi->word);
    return ret;
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
        return RUN(elt->elt.redirection);
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
    restore();
    return i;
}

int pipeline_run(struct ast* ast)
{
    assert(ast && ast->type == AST_PIPELINE);
    struct ast_pipeline *ast_pipe = (struct ast_pipeline *)ast;
    int save_stdout = dup(STDOUT_FILENO);
    int save_stdin = dup(STDIN_FILENO);
    (void)save_stdin;
    (void)save_stdout;
    int ret = RUN(ast_pipe->cmd[0]);
    for (int i = 1; i < ast_pipe->nbr_cmd; i++)
    {
        ret = RUN(ast_pipe->cmd[i]);
    }
    ret = ast_pipe->negation ? !ret : ret;
    return ret;
}


static int save_fd(int fd)
{
    int save = dup(fd);
    if (save == -1)
    {
        perror("dup");
        return 1;
    }
    struct s_redirection *new = calloc(1, sizeof(struct s_redirection));
    if (!new)
    {
        close(save);
        return 1;
    }

    new->original_fd = fd;
    new->saved_fd = save;
    new->next = s_redir;
    s_redir = new;
    return 0;
}

static void restore(void)
{
    while(s_redir)
    {
        struct s_redirection *cur = s_redir;
        
        if (dup2(cur->saved_fd, cur->original_fd) == -1)
        {
            perror("error during restore");
        }
        
        close(cur->saved_fd);
        s_redir = cur->next;
        free(cur);
    }
}

static int str_to_fd(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (!isdigit(str[i]))
        {
            return -1;
        }
    }
    return atoi(str);
}

static void set_vars(int *fd, int *flags, enum REDIRECTION_TYPE redir_op)
{
    switch (redir_op)
    {
        case LESS:
            *fd = *fd == -1 ? 0 : *fd;
            *flags = O_RDONLY;
            break;
        case GREATER:
            *fd = *fd == -1 ? 1 : *fd;
            *flags = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case DGREATER:
            *fd = *fd == -1 ? 1 : *fd;
            *flags = O_WRONLY | O_CREAT | O_APPEND;
            break;
        case CLOBBER:
            *fd = *fd == -1 ? 1 : *fd;
            *flags = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case LESS_GREATER:
            *fd = *fd == -1 ? 0 : *fd;
            *flags = O_RDWR | O_CREAT;
            break;
        case GREATER_AND:
            *fd = *fd == -1 ? 1 : *fd;
            break;
        case LESS_AND:
            *fd = *fd == -1 ? 0 : *fd;
            break;
        default:
            return;
    }
}

static int handle_redirection(int fd, enum REDIRECTION_TYPE redir_op, char *word)
{
    int flags;
    int new_fd;
    set_vars(&fd, &flags, redir_op);

    if (redir_op == GREATER_AND || redir_op == LESS_AND)
    {
        if (word[0] == '-')
        {
            if (save_fd(fd) != -1)
            {
                close(fd);
                return 0;
            }
            return 1;
        }
        int fd2 = str_to_fd(word);
        if (fd2 < 0 || fcntl(fd2, F_GETFL) == -1)
        {
            return 1; // not a regular fd
        }
        if (save_fd(fd) == -1)
        {
            return 1;
        }
        if (dup2(fd2, fd) == -1)
        {
            return 1; // error while duplicating
        }
        
        return 0;
    }
    if (save_fd(fd) == -1)
    {
        return 1;
    }

    
    new_fd = open(word, flags, 0644);
    if (new_fd == -1)
    {
        return 1; // error while opening
    }

    if (dup2(new_fd, fd) == -1)
    {
        perror("dup2");
        close(new_fd);
        return 1;
    }

    close(new_fd);
    return 0;
}


// FIN ANNEXE -------------
