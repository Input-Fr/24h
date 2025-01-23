#include <assert.h>
#include <ctype.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ast.h"
#include "hash_map/hash_map.h"

void separator_equal(char *name, char *val, char *as)
{
    size_t i = 0;
    while (as[i] != '\0' && as[i] != '=')
    {
        name[i] = as[i];
        i += 1;
    }
    size_t j = 0;
    i += 1;
    while (as[i] != '\0')
    {
        val[j] = as[i];
        j += 1;
        i += 1;
    }
    val[j] = '\0';
}

int test_quote(char *str) // test if a word is quoted
{
    if (strlen(str) > 1
        && ((str[0] == '\'' && str[strlen(str) - 1] == '\'')
            || (str[0] == '"' && str[strlen(str) - 1] == '"')))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int test_var(char *str) // test if a variable is in a word
{
    if (str[0] == '\'' && str[strlen(str) - 1] == '\'')
    {
        return 0;
    }
    size_t i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '$')
        {
            if (str[i + 1] == '\0'
                || ((!isalnum(str[i + 1])) && str[i + 1] != '?'
                    && str[i + 1] != '#' && str[i + 1] != '$'
                    && str[i + 1] != '@' && str[i + 1] != '*'
                    && str[i + 1] != '{' && str[i + 1] != '_'))
            {
                return 0;
            }
            if (str[i + 1] != '\0' && str[i + 1] == '{')
            {
                if (str[i + 3] != '\0'
                    && (str[i + 2] == '#' || str[i + 2] == '$'
                        || str[i + 2] == '@' || str[i + 2] == '*'
                        || str[i + 2] == '?' || isdigit(str[i + 2])))
                { // check if ${?} ${*} ...
                    if (str[i + 3] == '}')
                    {
                        return 1;
                    }
                    else
                    {
                        exit(1);
                    }
                }

                while (str[i] != '\0' && str[i] != '}')
                {
                    if ((!isalnum(str[i])) && str[i] != '}' && str[i] != '$'
                        && str[i] != '{' && str[i] != '_')
                    {
                        exit(1);
                    }
                    i += 1;
                }
                if (str[i] == '}')
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            if (str[i + 1] != '\0' && str[i + 1] != ' ')
            {
                return 1;
            }
        }
        i += 1;
    }
    return 0;
}

void error_var(char *word)
{
    size_t i = 0;
    while (i < strlen(word) && word[i] != '}')
    {
        if (word[i] == ' ') // erreur d'espace: ${ a }
        {
            fprintf(stdout, "bad substitution\n");
            exit(1);
        }
        i += 1;
    }
}

char *delete_dollar(char *word)
{
    char *tmp = word;
    word += 1;
    char *new = calloc(1, strlen(word) + 1);
    if (strlen(word) > 1 && word[0] == '{')
    {
        error_var(word);
        word += 1;
        new = strcpy(new, word);
        size_t i = 0;
        while (*word != '\0' && *word != '}')
        {
            i += 1;
            word += 1;
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

// static void error_var_brackets(char *var)
//{
//     if (strlen(var) > 1 && var[0] == '$'
//             && var[1] == '{' && var[strlen(var) - 1] != '}')
//     {
//         exit(2);
//     }
// }

char *delimite_var(char *prev, char *next, char *word)
{
    char *tmp = word;
    prev = strcpy(prev, word);
    size_t j = 0;
    while (*word != '\0' && *word != '$')
    {
        j += 1;
        word += 1;
    }

    prev[j] = '\0';

    char *new = calloc(1, strlen(word) + 1);
    new = strcpy(new, word);
    size_t i = 0;
    int acol = 0;

    word += 1;
    if (*word == '{')
    {
        acol = 1;
        word += 1;
    }
    if (*word == '@' || *word == '*' || *word == '?' || *word == '$'
        || *word == '#')
    {
        acol += 1;
    }
    else
    {
        while (*word != '\0' && *word != '}' && *word != ' '
               && (isalnum(*word) || *word == '_'))
        {
            word += 1;
            i += 1;
        }
    }
    new[i + acol + 1] = '\0';
    // printf("new : %s\n",new);
    if (*word == '\0')
        next = "";
    else if (*word == ' ' || (!isalnum(*word)) || *word != '_')
        next = strcpy(next, word + acol);

    word = tmp;
    // error_var_brackets(word);
    return new;
}

char *delete_quote(char *word)
{
    word += 1;
    char *new = calloc(1, strlen(word) + 1);
    new = strcpy(new, word);
    new[strlen(word) - 1] = '\0';
    word -= 1;
    return new;
}

int calcul_len(int nb)
{
    size_t k = 0;
    while (nb > 1)
    {
        nb = nb / 10;
        k += 1;
    }
    return k + 1;
}

int test_special_var(char *key)
{
    if (strcmp(key, "?") == 0) //$?
        return 1;
    if (strcmp(key, "@") == 0) //$?
        return 1;
    else if (key[0] >= '1' && key[0] <= '9') //$0, $n...
        return 1;
    else if (strcmp(key, "#") == 0) //$#
        return 1;
    else if (strcmp(key, "*") == 0) //$*
        return 1;
    else if (strcmp(key, "$") == 0)
        return 1;
    else if (strcmp(key, "UID") == 0)
        return 1;
    else if (strcmp(key, "PWD") == 0)
        return 1;
    else if (strcmp(key, "RANDOM") == 0)
        return 1;
    else
        return 0;
}
