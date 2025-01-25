#include <assert.h>
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
    if (atoi(key) > h->nb_args)
    {
        result = calloc(1, len + 1);
        snprintf(result, len + 1, "%s%s%s", prev, "", next);
        return result;
    }
    else
    {
        len += strlen(h->all_args[atoi(key)]);
        result = calloc(1, len + 1);
        snprintf(result, len + 1, "%s%s%s", prev, h->all_args[atoi(key)], next);
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

static char *expand_random(char *prev, char *next)
{
    srand(time(NULL));
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
    else if (strcmp(key, "RANDOM") == 0)
    {
        return expand_random(prev, next);
    }
    else
    {
        return "";
    }
}

static char *expand_normal_var(char *key, char *prev, char *next,
                               struct hash_map *h)
{
    char *val = hash_map_get(h, key); // get the value of the variable
    size_t len = strlen(prev) + strlen(val) + strlen(next) + 1;
    char *result = calloc(1, len + 1);
    snprintf(result, len, "%s%s%s", prev, val, next); // concat
    return result;
}

static char *_expand(struct hash_map *h, char *str)
{
    char *word = str;
    char *prev = calloc(1, strlen(word) + 1); // word before the variable
    char *next = calloc(1, strlen(word) + 1); // word after the variable
    char *var = delimite_var(prev, next, word); // divide the word in 3 words
    char *key = delete_dollar(var); //${name} -> name
    char *result = "";
    if (test_special_var(key)) // $@, $$, $*, $?, $1, $#, $RANDOM, $UID, $PWD
    {
        result = expand_special_var(key, prev, next, h);
    }
    else // variable classique : $name, ${name},"$name"...
    {
        result = expand_normal_var(key, prev, next, h);
    }

    expand_free(prev, next, var, key);
    return result;
}

// a faire : $OLDPWD $@ et $IFS

char *expand(struct hash_map *h, char *str)
{
    char *res = "";
    char *tmp;
    if (test_var(str))
    {
        res = _expand(h, str);
    }
    else
    {
        tmp = calloc(1, strlen(str) * 2);
        strcpy(tmp, str);
        res = tmp;
    }

    if (test_quote(res))
    {
        res = delete_quote(res);
    }

    return res;
}
