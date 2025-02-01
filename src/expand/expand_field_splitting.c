#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "expand.h"
#include "expand/eval/evalexpr.h"
#include "hash_map/hash_map.h"

int test_double_quote(char *str)
{
    size_t single_quote = 0;
    size_t double_quote = 0;
    for (size_t i = 0; str[i] != '\0'; i += 1)
    {
        if (str[i] == '\'' && !single_quote && !double_quote)
            single_quote = 1;
        else if (str[i] == '\'' && single_quote && !double_quote)
            single_quote = 0;
        else if (str[i] == '"' && !single_quote && !double_quote)
            double_quote = 1;
        else if (str[i] == '"' && !single_quote && double_quote)
            double_quote = 0;

        if ((str[0] == '$' || (str[i] == '$' && i > 0 && str[i - 1] != '\\'))
            && !double_quote && !single_quote
            && (i < strlen(str) && str[i + 1] != '('))
        {
            i += 1;
            char c = str[i];
            if (is_special_var(str, &i))
                return 1;
            else if ((c != '\0' && c != '{' && (isalpha(c) || c == '_')))
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
    }
    return 0;
}

static int ispresent(char c, char *deli)
{
    for (size_t i = 0; deli[i] != '\0'; i += 1)
    {
        if (deli[i] == c)
            return 1;
    }
    return 0;
}

size_t len_ifs(char *str, struct hash_map *h)
{
    struct ast * save = NULL;
    char *deli = hash_map_get(h, "IFS",&save);
    (void)save;
    size_t cpt = 0;
    for (size_t i = 0; str[i] != '\0'; i += 1)
    {
        if (ispresent(str[i], deli))
            cpt += 1;
    }
    return cpt + 1;
}

static size_t split(char *str, char **res, char *deli)
{
    char *rest = str;
    char *token;
    size_t i = 0;
    while ((token = strtok_r(rest, deli, &rest)) != NULL)
    {
        char *word = calloc(1, strlen(token) + 1);
        strcpy(word, token);
        res[i] = word;
        i += 1;
    }
    return i;
}

void field_splitting(char *str, char *check, char **res, struct hash_map *h)
{
    if (test_double_quote(check))
    {
        struct ast * save = NULL;
        char *deli = hash_map_get(h, "IFS", &save);
        (void)save;
        size_t nb = split(str, res, deli);
        for (size_t i = 0; i < nb; i += 1)
        {
            //            printf("%s\n", res[i]);
            free(res[i]);
        }
    }
    else
    {
        res[0] = str;
    }
}
