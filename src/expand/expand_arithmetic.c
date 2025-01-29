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
#include "expand/eval/evalexpr.h"


int test_ari(char *str) // test if arithmetic
{
    size_t i = 0;
    size_t quote = 0;
    while (i < strlen(str) && str[i] != '\0')
    {
        if (str[i] == '\'' && !quote)
            quote = 1;
        else if (str[i] == '\'' && quote)
            quote = 0;
        if (((str[0] == '$' || (i > 0 && str[i - 1] != '\\' && str[i] == '$'))
             && !quote))
        {
            i += 1;
            char c = str[i];
            char c1 = str[i + 1];
            if (c != '\0' && c == '(') // arith check : $((...))
            {
                while (str[i] != '\0' && str[i] != ')')
                {
                    i += 1;
                }
                if (c1 == '(' && str[i] == ')' && str[i+1] == ')')
                    return 1;
            }
        }
        i += 1;
    }
    return 0;
}

static char *delimite_ari(char *prev, char *next, char *word)
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
    word += 3;
    char *new = calloc(1, strlen(word) + 1);
    new = strcpy(new, word);
    size_t i = 0;

    word += 1;
    while (*word != '\0' && *word != ')')
    {
        word += 1;
        i += 1;
    }
    new[i + 1] = '\0';
    if (*word == '\0')
        next = "";
    else
    {
        next = strcpy(next, word + 2);
    }


    word = tmp;
    // error_var_brackets(word);
    return new;
}



void expand_ari(struct hash_map *h, char *res)
{
    if (test_ari(res))
    {
        char *prev = calloc(1, strlen(res) + 1); // word before the variable
        char *next = calloc(1, strlen(res) + 1); // word after the variable
        char *operation = delimite_ari(prev, next, res); // divide the word in 3 words

        char *val = calloc(1, 64);
        eval_arithmetic(val, operation, h);
        size_t len = strlen(prev) + strlen(res) + strlen(next) + 1;

        snprintf(res, len, "%s%s%s", prev, val, next); // concat
        free(next);
        free(prev);
        free(operation);
        free(val);
    }
}
