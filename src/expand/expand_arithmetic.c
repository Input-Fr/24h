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

int test_ari(char *str) // test if a variable is in a word
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

/*

static void delete_c(char *word, size_t *j)
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
    if (*word == '(')
    {
        acol = 1;
        word += 1;
    }
    if (isdigit(*word) || *word == '@' || *word == '*' || *word == '?'
        || *word == '$' || *word == '#')
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




char *delete_quote(char *str)
{
    if (!test_quote(str))
    {
        return str;
    }

    char c = ' ';
    for (size_t i = 0; str[i] != '\0'; i += 1)
    {
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
    return str;
}
*/



//static char *expand_ari(struct hash_map *h, char *str, char *res)
//{
//    char *word = str;
//    char *prev = calloc(1, strlen(word) + 1); // word before the variable
//    char *next = calloc(1, strlen(word) + 1); // word after the variable
//    char *var = delimite_ari(prev, next, word); // divide the word in 3 words
//    char *key = delete_par(var); //$((a + 1)) -> a + 1
//    char *result = "";
//    result = expand_normal_var(key, prev, next, h);
//
//    strcpy(res, result);
//    free(result);
//    expand_free(prev, next, var, key);
//    return res;
//
//}
