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
    char *result = calloc(1, 1024);
    size_t len = strlen(prev) + strlen(next) + calcul_len(h->ret) + 2;
    snprintf(result, len, "%s%d%s", prev, h->ret, next);
    return result;
}

static char *expand_argn(char *key, char *prev, char *next, struct hash_map *h)
{
    char *result = calloc(1, 1024);
    size_t len = 0;
    if (atoi(key) > h->nb_args)
    {
        len = strlen(prev) + strlen(next);
        snprintf(result, len, "%s%s%s", prev, "", next);
        return result;
    }
    else
    {
        len = strlen(prev) + strlen(next) + strlen(h->all_args[atoi(key)]);
        snprintf(result, len, "%s%s%s", prev, h->all_args[atoi(key)], next);
        return result;
    }
}

static char *expand_nb_args(char *prev, char *next, struct hash_map *h)
{
    char *result = calloc(1, 1024);
    size_t len = strlen(prev) + strlen(next) + calcul_len(h->nb_args) + 2;
    snprintf(result, len, "%s%d%s", prev, h->nb_args, next);
    return result;
}

static char *expand_all_args(char *prev, char *next, struct hash_map *h)
{
    char *result = calloc(1, 1024);
    char *str = calloc(1, 1024);
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
    size_t len = strlen(prev) + strlen(next) + 1024;
    snprintf(result, len, "%s%s%s", prev, str, next);
    free(str);
    return result;
}

static char *expand_processid(char *prev, char *next)
{
    char *result = calloc(1, 1024);
    pid_t pid = getpid();
    size_t len = strlen(prev) + strlen(next) + 7;
    snprintf(result, len, "%s%d%s", prev, pid, next);
    return result;
}

static char *expand_UID(char *prev, char *next)
{
    char *result = calloc(1, 1024);
    struct passwd *p;
    uid_t uid;
    p = getpwuid(uid = getuid());
    int vall = (int)p->pw_uid;
    size_t len = strlen(prev) + strlen(next) + 7;
    // result = realloc(result, len);
    snprintf(result, len, "%s%d%s", prev, vall, next);
    return result;
}

static char *expand_pwd(char *prev, char *next)
{
    char *result = calloc(1, 1024);
    char *buffer = malloc(1024 * sizeof(char));
    getcwd(buffer, 1024);
    size_t len = strlen(prev) + strlen(next) + strlen(buffer) + 1;
    snprintf(result, len, "%s%s%s", prev, buffer, next);
    free(buffer);
    return result;
}

static char *expand_random(char *prev, char *next)
{
    char *result = calloc(1, 1024);
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
    snprintf(result, len, "%s%d%s", prev, nb, next);
    return result;
}

static char *expand_special_var(char *key, char *prev, char *next,
                                struct hash_map *h)
{
    if (strcmp(key, "?") == 0) //$?
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
    char *result = calloc(1, 1024);
    char *val = hash_map_get(h, key); // get the value of the variable
    size_t len = strlen(prev) + strlen(val) + strlen(next) + 1;
    snprintf(result, len, "%s%s%s", prev, val, next); // concat
    return result;
}

static char *_expand(struct hash_map *h, char *str)
{
    char *word = str;
    int a = 0;
    if (word[0] == '"')
    {
        word = delete_quote(str);
        a = 1;
    }
    char *prev = calloc(1, strlen(word) + 1); // word before the variable
    char *next = calloc(1, strlen(word) + 1); // word after the variable
    char *var = delimite_var(prev, next, word); // divide the word in 3 words
    char *key = delete_dollar(var); //${name} -> name
    char *result = "";

    if (test_special_var(key)) //$$, $*, $?, $1, $#, $RANDOM, $UID, $PWD
    {
        result = expand_special_var(key, prev, next, h);
    }
    else // variable classique : $name, ${name},"$name"...
    {
        result = expand_normal_var(key, prev, next, h);
    }

    expand_free(prev, next, var, key);
    if (a)
        free(word);
    return result;
}

// a faire : $OLDPWD $@ et $IFS

char *expand(struct hash_map *h, char *str)
{
    if (test_var(str))
    {
        return _expand(h, str);
    }
    else if (test_quote(str))
    {
        return delete_quote(str);
    }
    return "";
}
