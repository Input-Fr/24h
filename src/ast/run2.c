#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "expand/expand.h"
#include "hash_map/hash_map.h"

#define RUN(AST, HASH_TABLE) (*(AST)->ftable->run)((AST), (HASH_TABLE))
#define RUN_LIST(ASTS, ELT, HASH_TABLE)                                        \
    handle_list_ast((ASTS), (&ELT), HASH_TABLE) // évalue une liste d'ast
static int handle_list_ast(struct ast **asts, size_t *nbr_element,
                           struct hash_map *h)
{
    int j = 0;
    for (size_t i = 0; i < *nbr_element; i++)
    {
        j = RUN(asts[i], h);
    }
    return j;
}

char **expand_all(char **word, size_t nbr, struct hash_map *h)
{
    char **new_word = calloc(nbr, sizeof(char *));
    if (!new_word)
    {
        return NULL;
    }
    for (size_t i = 0; i < nbr; i++)
    {
        if (test_var(word[i]) || test_quote(word[i]))
        {
            new_word[i] = expand(h, word[i]);
        }
        else
        {
            char *save = malloc(strlen(word[i]) + 1);
            if (!save)
            {
                for (size_t j = 0; j < i; i++)
                {
                    free(new_word[i]);
                }
                free(new_word);
                return NULL;
            }
            else
            {
                strcpy(save, word[i]);
                new_word[i] = save;
            }
        }
    }
    return new_word;
}

int for_run(struct ast *ast, struct hash_map *h)
{
    assert(ast && ast->type == AST_FOR);
    struct ast_for *boucle = (struct ast_for *)ast;
    int res = 0;
    char **get = boucle->list;
    size_t nbr = boucle->nbr_elt;
    if (!nbr)
    {
        nbr = h->nb_args;
        get = h->all_args;
    }

    for (size_t i = 0; i < nbr; i++)
    {
        char *extended = expand(h, get[i]);
        hash_map_insert(h, boucle->variable, extended, VARIABLE);
        RUN(boucle->do_body, h);
        if (i < nbr - 1)
            free(extended);
    }
    hash_map_remove(h, boucle->variable);
    return res;
}

static char is_character(char w)
{
    if ((w <= 'z' && w >= 'a') || (w <= 'Z' && w >= 'A'))
    {
        return 1;
    }
    return 0;
}

static char is_number(char w)
{
    return (w >= '0' && w <= '9');
}

static char is_good(char w)
{
    if (w == '_')
    {
        return 1;
    }
    return is_number(w) || is_character(w);
}

static char is_good_name(char *name)
{
    if (is_number(name[0]))
    {
        return 0;
    }
    size_t len = strlen(name);
    size_t i = 1;
    while (i <= len && is_good(name[i]))
    {
        i += 1;
    }
    return i >= len;
}

static short egal(char *word, const char *second)
{
    return !strcmp(word, second);
}

static short is_special_builtin(char *words)
{
    int test1 = egal(words, "echo") || egal(words, "true");
    int test2 = egal(words, "false") || egal(words, "exit");
    int test3 = egal(words, "cd") || egal(words, "unset");
    return egal(words, "export") || test1 || test2 || test3;
}

static char *copy(char *word)
{
    char *copy = calloc(strlen(word) + 1, 1);
    strcpy(copy, word);
    return copy;
}

int function_run(struct ast *ast, struct hash_map *h)
{
    assert(ast && ast->type == AST_FUNCTION);
    struct ast_function *func = (struct ast_function *)ast;
    if (!is_special_builtin(func->fname) && is_good_name(func->fname))
    {
        char *copy2 = copy(func->fname);
        hash_map_remove(h, copy2);
        hash_map_insert(h, copy2, (void *)ast, FUNCTION);
        return 0;
    }
    func->isHash = 1;
    // free_function_hashmap(ast);
    return 1;
}

int function_run_hashmap(struct ast *ast, struct hash_map *h)
{
    assert(ast && ast->type == AST_FUNCTION);
    struct ast_function *func = (struct ast_function *)ast;
    int res = RUN_LIST(func->redirection, func->nbr_redirection, h);
    res = RUN(func->shell_command, h);
    return res;
}

int subshell_run(struct ast *ast, struct hash_map *h)
{
    assert(ast && ast->type == AST_SUBSHELL);
    struct ast_subshell *subshell = (struct ast_subshell *)ast;
    char *test = malloc(10);
    (void)test;
    pid_t pid = fork();
    if (pid == 0) // child
    {
        int res = RUN(subshell->compound_list, h);
        exit(res);
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        int return_value = WEXITSTATUS(wstatus);
        free(test);
        return return_value;
    }
    return 0;
}
