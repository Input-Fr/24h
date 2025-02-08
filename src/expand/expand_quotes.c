#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast.h"
#include "expand.h"
#include "hash_map/hash_map.h"

int test_quote(char *str)
{
    for (size_t i = 0; str[i] != '\0'; i += 1)
    {
        if (str[i] == '"' || str[i] == '\'' || str[i] == '\\')
            return 1;
    }
    return 0;
}

static int isspecial(char c)
{
    if (c == '$' || c == '`' || c == '"' || c == '\\')
    {
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

void delete_quote(char *str)
{
    if (test_quote(str))
    {
        char c = ' ';
        for (size_t i = 0; str[i] != '\0'; i += 1)
        {
            var_field(str, &i);
            if (str[i] == '\\' && c == '"' && i + 1 < strlen(str)
                && !isspecial(str[i + 1]))
            {
                i += 1;
            }
            if (str[i] == '\\' && c != '\'')
            {
                delete_c(str, &i);
            }
            else if ((str[i] == '"' || str[i] == '\'') && (c == ' '))
            {
                c = str[i];
                delete_c(str, &i);
                i -= 1;
            }
            else if (str[i] != '\0' && str[i] == c && c != ' ')
            {
                c = ' ';
                delete_c(str, &i);
                i -= 1;
            }
        }
    }

    for (size_t j = 0; str[j] != '\0'; j += 1)
    {
        // printf("-str: %s\n", str);
        if (str[j] == '^' && str[j + 1] == '^')
        {
            delete_c(str, &j);
            delete_c(str, &j);
            j--;
        }
    }
}
