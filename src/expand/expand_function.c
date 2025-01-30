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

void error_var(char *word)
{
    size_t i = 0;
    while (i < strlen(word) && word[i] != '}')
    {
        if (word[i] == ' ') // erreur d'espace: ${ a }
        {
            errx(1, "bad substitution");
        }
        i += 1;
    }
}

void delete_c(char *word, size_t *j)
{
    char *copy = calloc(1, strlen(word) * 2);
    char *tmp = copy;
    strcpy(copy, word);
    for (size_t i = 0; i != *j; i += 1)
    {
        copy += 1;
    }

    word[*j] = '\0';
    char *tmp2 = calloc(1, strlen(word) + strlen(copy));
    snprintf(tmp2, strlen(word) + strlen(copy), "%s%s", word, copy + 1);
    strcpy(word, tmp2);
    free(tmp2);
    free(tmp);
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
    else if (strcmp(key, "OLDPWD") == 0)
        return 1;
    else if (strcmp(key, "IFS") == 0)
        return 1;
    else if (strcmp(key, "RANDOM") == 0)
        return 1;
    else
        return 0;
}
