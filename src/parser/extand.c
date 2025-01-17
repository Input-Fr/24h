#include <pwd.h>
#include <sys/types.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ast.h"
#include "hash_map/hash_map.h"

int test_var(char *str)
{
    size_t i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '$')
        {
            if (str[i + 1] != '\0' && str[i+1] == '{')
            {
                while (str[i] != '\0' && str[i] == '}')
                {
                    i += 1;
                }
                if (str[i] == '}')
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
            if (str[i + 1] != '\0' && str[i+1] != ' ')
            {
                return 1;
            }
        }
        i += 1;
    }
    return 0;
}


static void error_var(char *word)
{
    size_t i = 0;
    while (i < strlen(word) && word[i] != '}')
    {
        if (word[i] == ' ')        //erreur d'espace: ${ a } 
        {
            fprintf(stdout,"bad substitution\n");
            exit(1);
        }
        i+=1;
    }

}

static char *delete_dollar(char *word)
{
    char *tmp = word;
    word += 1;
    char * new = calloc(1, strlen(word) + 1);
    if (strlen(word) > 1 && word[0] == '{')
    {
        error_var(word);
        word += 1;
        new = strcpy(new, word);
        size_t i = 0;
        while (*word != '\0' && *word != '}')
        {
            i += 1;
            word+=1;
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


static char *delimite_var(char *prev, char *next, char *word)
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
    while (*word != '\0' && *word != '}' && *word != ' ')
    {
        word+=1;
        i += 1;
    }
    if (*word == '\0')
    {
        next = "";
    }
    else if (*word == ' ')
    {
        next = strcpy(next, word);
    }
    else
    {
        next = strcpy(next, word + 1);
    }

    if (*word == '}')
    {
        new[i + 1] = '\0';
    }
    else
    {
        new[i] = '\0';
    }

    word = tmp;
    //free(word);
    return new;
}

char *delete_quote(char *word)
{
    word += 1;
    char * new = calloc(1, strlen(word) + 1);
    new = strcpy(new, word);
    new[strlen(word) - 1] = '\0';
    word -= 1;
    return new;
}


char *expand(struct hash_map *h, char *str)
{
    char *word = str;
    int a = 0;
    if (word[0] == '"')
    {
        word = delete_quote(str);
        a = 1;
    }

    char *result = calloc(1, strlen(word) + 1);
    char *prev = calloc(1, strlen(word) + 1);
    char *next = calloc(1, strlen(word) + 1);

    char *var = delimite_var(prev, next, word);
    char *key = delete_dollar(var);
    char *val = "";
    if (strcmp(key,"UID") == 0)
    {
            struct passwd *p;
            uid_t uid;
            p = getpwuid(uid = getuid());
            int vall = (int)p->pw_uid;
            size_t len = strlen(prev) + 6 + strlen(next) + 1;
            result = realloc(result, len);
            snprintf(result,len, "%s%d%s", prev, vall, next);
    }
    else if (strcmp(key,"?") == 0)
    {
        size_t len = strlen(prev) + 5 + strlen(next) + 1;  //faire calcul de la len
        result = realloc(result, len);
        snprintf(result,len, "%s%d%s", prev, h->ret, next);
    }
    else if (strcmp(key,"#") == 0)
    {
        size_t len = strlen(prev) + 1 + strlen(next) + 1;
        snprintf(result,len, "%s%d%s", prev, h->nb_args, next);
    }
    else if (strcmp(key,"*") == 0)
    {
        size_t len = strlen(prev) + 1 + strlen(next) + 1;
        snprintf(result,len, "%s%d%s", prev, h->nb_args, next);
    }
    else if (key[0] >= '0' && key[0] <= '9')
    {
        size_t len = strlen(prev) + 1 + strlen(next) + 1;
        snprintf(result,len, "%s%s%s", prev, h->arg, next);
    }
    else
    {
        val = hash_map_get(h,key);
        size_t len = strlen(prev) + strlen(val) + strlen(next) + 1;
        snprintf(result,len, "%s%s%s", prev, val, next);
    }

    free(prev);
    free(next);
    free(var);
    free(key);
    if (a)
        free(word);
    //free(val);
    return result;
}
