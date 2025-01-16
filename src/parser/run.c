#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ast.h"
#include "hash_map/hash_map.h"

#define RUN(AST,HASH_TABLE) (*(AST)->ftable->run)((AST),(HASH_TABLE))



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

static void error_var(char *word)
{
    //if (word[strlen(word)] != '}')
    //{
    //    fprintf(stdout,"bad substitution\n");
    //    exit(1);
    //}
    size_t i = 0;
    while (i < strlen(word) && word[i] != '}')
    {
        if (word[i] == ' ')        //erreur d'espace: ${ a } 
        {
            fprintf(stdout,"bad substitution\n");    
            exit(1);
        }
        i+=1;
    }

}

static char *delete_dollar(char *word)
{
    char *tmp = word;
    word += 1;
    char * new = calloc(1, strlen(word) + 1);
    if (strlen(word) > 1 && word[0] == '{')
    {
        error_var(word);
        word += 1;
        new = strcpy(new, word);
        size_t i = 0;
        while (*word != '\0' && *word != '}')
        {
            i += 1;
            word+=1;
        }
        new[i] = '\0';
    }
    else
    {
        new = strcpy(new, word);
    }
    word = tmp;
    return new;
}


static char *delimite_var(char *prev, char *next, char *word)
{
    char *tmp = word;
    prev = strcpy(prev, word);
    size_t j = 0;
    while (*word != '\0' && *word != '$')
    {
        j += 1;
        word += 1;
    }

    if (*word != '\0')
    {
        prev[j] = '\0';
    }
    else
    {
        next = NULL;
    }

    char *new = calloc(1, strlen(word) + 1);
    new = strcpy(new, word);
    size_t i = 0;
    while (*word != '\0' && *word != '}')
    {
        word+=1;
        i += 1;
    }

    if (*word != '\0')
    {
        next = strcpy(next, word + 1);
    }
    new[i + 1] = '\0';
    word = tmp;
    //free(word);
    return new;
}


static char *expand(struct hash_map *h, char *str)
{
    char *result = calloc(1, strlen(str) + 1);
    char *prev = calloc(1,strlen(str) + 1);
    char *next = calloc(1,strlen(str) + 1); 
    str = delimite_var(prev, next, str);
    char *key = delete_dollar(str);
    char *val = hash_map_get(h,key);
    snprintf(result, 100, "%s%s%s", prev, val, next);

    //printf("key :%s\n",key);
    //printf("val :%s\n",val);
    //printf("str :%s\n",result);
    free(prev);
    free(next);
    free(key);
    //free(val);
    free(str);

    return result;
}

int test_var(char *str)
{
    size_t i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '$')
        {
            if (str[i+ 1] != '\0' && str[i+1] == '{')
            {
                return 1;
            }
            if (str[i+ 1] != '\0' && str[i+1] != ' ')
            {
                return 1;
            }
        }
        i += 1;
    }
    return 0;
}
// args est de la forme ["arg1", "arg2", "arg3"]
void echo_builtin(char *args[], size_t nb_args, struct hash_map *h)
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
            char *str = args[i];
            if (str[0] != '\'' && test_var(str))
            {
                char *string = expand(h, str);
                printf("%s", string);
                free(string);
            }
            else
            {
                printf("%s", str);
            }
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
int list_run(struct ast *ast, struct hash_map *h)
{
    assert(ast && ast->type == AST_LIST);
    struct ast_list *list = (struct ast_list *)ast;
    size_t i = 0;
    while (i < list->nbr_cmd && !RUN(list->cmd[i],h))
    {
        i += 1;
    }
    return i >= list->nbr_cmd;
}

// cmd ast eval
int cmd_run(struct ast *ast, struct hash_map *h)
{
    if (h == NULL)
    {
        return 0;
    }
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
            echo_builtin(cmd->words + 1, idx - 1,h);
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
            if (fork() == 0)
            {
                int status_code = execvp(cmd->words[0], cmd->words);
                if (status_code == -1)
                {
                    printf("Terminated Incorrectly\n");
                    return 2;
                }
            }
        }
        return 0;
    }
}

// if ast eval
int if_run(struct ast *ast, struct hash_map *h)
{
    assert(ast && ast->type == AST_IF);
    struct ast_if *if_ast = (struct ast_if *)ast;
    if (RUN(if_ast->condition, h))
    {
        return RUN(if_ast->then_body,h);
    }
    else if (!if_ast->else_body)
    {
        return 1;
    }
    else
    {
        return RUN(if_ast->else_body, h);
    }
}

int variable_run(struct ast *ast, struct hash_map *h)
{
    assert(ast && ast->type == AST_VARIABLE);
    struct ast_variable *variable_ast = (struct ast_variable *)ast;
    //if (variable_ast->name != NULL)
    //{
    //    printf("%s\n", variable_ast->name);
    //}
    //if (variable_ast->name != NULL)
    //{
    //    printf("%s\n", variable_ast->val);
    //}

    hash_map_insert(h, variable_ast->name, variable_ast->val);
    return 1;
}
