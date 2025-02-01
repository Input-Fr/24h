#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ast/ast.h"
#include "expand.h"
#include "hash_map/hash_map.h"

static char *delete_dollar(char *word)
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

static int while_condition(size_t j, char *word, char *tmp, int single_quote)
{
    if ((tmp[j + 1] == '(') || single_quote
        || (*word != '\0'
            && (*word != '$' || (j > 0 && *word == '$' && tmp[j - 1] == '\\'))))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void quote_handling(size_t *single_quote, size_t *double_quote, char c)
{
    if (c == '\'' && !*single_quote && !*double_quote)
        *single_quote = 1;
    else if (c == '\'' && *single_quote && !*double_quote)
        *single_quote = 0;
    else if (c == '"' && !*single_quote && !*double_quote)
        *double_quote = 1;
    else if (c == '"' && !*single_quote && *double_quote)
        *double_quote = 0;
}

static int is_special(char c)
{
    if (isdigit(c) || c == '@' || c == '*' || c == '?' || c == '$' || c == '#')
    {
        return 1;
    }
    return 0;
}

static char *delimite_var(char *prev, char *next, char *word)
{
    char *tmp = word;
    prev = strcpy(prev, word);
    size_t j = 0;
    size_t single_quote = 0;
    size_t double_quote = 0;

    while (while_condition(j, word, tmp, single_quote))
    {
        quote_handling(&single_quote, &double_quote, tmp[j]);
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

    if (is_special(*word))
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
    if (*word == '\0')
        next = "";
    else if (*word == ' ' || (!isalnum(*word)) || *word != '_')
        next = strcpy(next, word + acol);
    word = tmp;
    // error_var_brackets(word);
    return new;
}

int is_special_var(char *str, size_t *i)
{
    if (str[*i] != '\0' && str[*i] == '{')
    {
        // check if ${?} ${*} ...
        if (str[*i + 1] != '\0' && is_special(str[*i + 1]))
        {
            if (str[*i + 2] != '\0' && str[*i + 2] == '}')
                return 1;
            else
                errx(1, "invalid variable"); // if not a valid format like ${12}
        }
    }
    else if (str[*i] != '\0' && is_special(str[*i]))
    {
        return 1;
    }
    return 0;
}

static int condition_var(char *str, int i)
{
    if ((str[0] == '$' && str[1] != '\0' && str[1] != '(')
        || ((i > 0 && str[i - 1] != '\\' && str[i] == '$' && str[i + 1] != '\0'
             && str[i + 1] != '(')))
    {
        // printf("c: %c ", str[i + 1]);
        return 1;
    }
    else
    {
        return 0;
    }
}

static void var_field(char *str, size_t *i)
{
    if (str[*i] == '^' && str[*i + 1] == '^')
    {
        *i += 1;
        while (*i + 1 < strlen(str) && !(str[*i] == '^' && str[*i + 1] == '^'))
        {
            *i += 1;
        }
        *i += 1;
    }
}

static void ari_field(char *str, size_t *i)
{
    if (str[*i] == '$' && str[*i + 1] == '(')
    {
        *i += 1;
        while (*i + 1 < strlen(str) && !(str[*i] == ')'))
        {
            *i += 1;
        }
        *i += 1;
    }
}

int test_var(char *str) // test if a variable is in a word
{
    size_t single_quote = 0;
    size_t double_quote = 0;
    size_t i = 0;
    while (i < strlen(str) && str[i] != '\0')
    {
        var_field(str, &i);
        ari_field(str, &i);
        quote_handling(&single_quote, &double_quote, str[i]);

        if (condition_var(str, i) && !single_quote)
        {
            i += 1;
            char c = str[i];
            if (is_special_var(str, &i))
                return 1;
            else if ((c != '\0' && c != '{' && c != '('
                      && (isalpha(c) || c == '_')))
                return 1; // normal variable : $name
            else if (c != '\0' && c != '{')
                return 0; // exit

            if (c != '\0' && c == '{') // variable check : ${...}
            {
                while (str[i] != '\0' && str[i] != '}')
                {
                    if ((!isalnum(str[i])) && str[i] != '}' && str[i] != '{'
                        && str[i] != '_')
                    {
                        errx(1, "invalid variable");
                    }
                    i += 1;
                }
                if (str[i] == '}')
                    return 1;
                else
                    return 0;
            }
        }
        i += 1;
    }
    return 0;
}

static void expand_free(char *prev, char *next, char *var, char *key)
{
    free(prev);
    free(next);
    free(var);
    free(key);
}

static char *expand_ret(char *prev, char *next, struct hash_map *h)
{
    size_t len = strlen(prev) + strlen(next) + calcul_len(h->ret);
    char *result = calloc(1, len + 1);
    snprintf(result, len + 1, "%s%d%s", prev, h->ret, next);
    return result;
}

static char *expand_argn(char *key, char *prev, char *next, struct hash_map *h)
{
    char *result;
    size_t len = strlen(prev) + strlen(next);
    if (atoi(key) + 1 > h->nb_args)
    {
        result = calloc(1, len + 1);
        snprintf(result, len + 1, "%s%s%s", prev, "", next);
        return result;
    }
    else
    {
        len += strlen(h->all_args[atoi(key)]);
        result = calloc(1, len + 1);
        snprintf(result, len + 1, "%s%s%s", prev, h->all_args[atoi(key) + 1],
                 next);
        return result;
    }
}

static char *expand_nb_args(char *prev, char *next, struct hash_map *h)
{
    size_t len = strlen(prev) + strlen(next) + calcul_len(h->nb_args);
    char *result = calloc(1, len + 1);
    snprintf(result, len + 1, "%s%d%s", prev, h->nb_args, next);
    return result;
}

static char *expand_all_args(char *prev, char *next, struct hash_map *h)
{
    size_t len = strlen(prev) + strlen(next);
    for (int k = 1; k < h->nb_args + 1; k += 1)
    {
        len += strlen(h->all_args[k]);
        len += 1; // for the space
    }

    char *result = calloc(1, len + 1);
    char *str = calloc(1, len + 1);
    size_t k = 0;
    for (int i = 1; i < h->nb_args + 1; i += 1)
    {
        for (size_t j = 0; (h->all_args[i])[j] != '\0'; j += 1)
        {
            str[k] = (h->all_args[i])[j];
            k += 1;
        }
        str[k] = ' ';
        k += 1;
    }
    str[k - 1] = '\0';
    snprintf(result, len + 1, "%s%s%s", prev, str, next);
    free(str);
    return result;
}

static char *expand_processid(char *prev, char *next)
{
    pid_t pid = getpid();
    size_t len = strlen(prev) + strlen(next) + calcul_len(pid);
    char *result = calloc(1, len + 1);
    snprintf(result, len, "%s%d%s", prev, pid, next);
    return result;
}

static char *expand_UID(char *prev, char *next)
{
    struct passwd *p;
    uid_t uid;
    p = getpwuid(uid = getuid());
    int vall = (int)p->pw_uid;
    size_t len = strlen(prev) + strlen(next) + calcul_len(vall);
    char *result = calloc(1, len + 1);
    snprintf(result, len + 1, "%s%d%s", prev, vall, next);
    return result;
}

static char *expand_pwd(char *prev, char *next)
{
    char *buffer = malloc(1024 * sizeof(char));
    getcwd(buffer, 1024);
    size_t len = strlen(prev) + strlen(next) + strlen(buffer);
    char *result = calloc(1, len + 1);
    snprintf(result, len + 1, "%s%s%s", prev, buffer, next);
    free(buffer);
    return result;
}

static char *expand_oldpwd(char *prev, char *next)
{
    char *str = getenv("OLDPWD");
    // setenv("OLDPWD", getcw(buf,1024),1);

    size_t len = strlen(prev) + strlen(next) + 100;
    char *result = calloc(1, len + 1);
    snprintf(result, len + 1, "%s%s%s", prev, str, next);
    // free(str);
    return result;
}

static char *expand_random(char *prev, char *next)
{
    int nb = rand();
    int a = 1;
    if (nb % 2 == 0)
    {
        a = 0;
    }
    else
    {
        a = 1;
    }
    size_t len = strlen(prev) + strlen(next) + 5 + a;
    char *result = calloc(1, len + 1);
    snprintf(result, len, "%s%d%s", prev, nb, next);
    return result;
}

static char *expand_ifs(char *prev, char *next, struct hash_map *h)
{
    size_t len = strlen(prev) + strlen(next) + 10;
    char *result = calloc(1, len + 1);
    struct ast * save = NULL;
    char *ifs = hash_map_get(h, "IFS", &save);
    (void)save;
    snprintf(result, len + 1, "%s%s%s", prev, ifs, next);
    return result;
}

static char *expand_special_var(char *key, char *prev, char *next,
                                struct hash_map *h)
{
    if (strcmp(key, "@") == 0) //$@
    {
        return expand_all_args(prev, next, h);
    }
    else if (strcmp(key, "?") == 0) //$?
    {
        return expand_ret(prev, next, h);
    }
    else if (key[0] >= '1' && key[0] <= '9') //$0, $n...
    {
        return expand_argn(key, prev, next, h);
    }
    else if (strcmp(key, "#") == 0) //$#
    {
        return expand_nb_args(prev, next, h);
    }
    else if (strcmp(key, "*") == 0) //$*
    {
        return expand_all_args(prev, next, h);
    }
    else if (strcmp(key, "$") == 0)
    {
        return expand_processid(prev, next);
    }
    else if (strcmp(key, "UID") == 0)
    {
        return expand_UID(prev, next);
    }
    else if (strcmp(key, "PWD") == 0)
    {
        return expand_pwd(prev, next);
    }
    else if (strcmp(key, "OLDPWD") == 0)
    {
        return expand_oldpwd(prev, next);
    }
    else if (strcmp(key, "RANDOM") == 0)
    {
        return expand_random(prev, next);
    }
    else if (strcmp(key, "IFS") == 0)
    {
        return expand_ifs(prev, next, h);
    }
    else
    {
        return "";
    }
}

void expand_variables(struct hash_map *h, char *res)
{
    while (test_var(res))
    {
        char *prev = calloc(1, strlen(res) + 1); // word before the variable
        char *next = calloc(1, strlen(res) + 1); // word after the variable
        char *var = delimite_var(prev, next, res); // divide the word in 3 words
        char *key = delete_dollar(var); // ${name} -> name
        if (test_special_var(key)) // $@, $$, $*, $?, $0, $#, $RANDOM,$UID,$PWD
        {
            char *tmp = expand_special_var(key, prev, next, h);
            strcpy(res, tmp);
            free(tmp);
        }
        else // variable classique : $name, ${name},"$name"...
        {
            struct ast * save = NULL;
            char *val = hash_map_get(h, key, &save); // get the value of the variable
            (void)save;
            size_t len = strlen(prev) + strlen(val) + strlen(next) + 1;
            snprintf(res, len + 4, "%s^^%s^^%s", prev, val, next); // concat
        }
        expand_free(prev, next, var, key);
    }
}
