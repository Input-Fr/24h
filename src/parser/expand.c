#include <pwd.h>
#include <sys/types.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

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


static void expand_free(char *prev, char *next, char *var, char *key)
{
    free(prev);
    free(next);
    free(var);
    free(key);
}

static void expand_UID(char *prev, char *next, char *result)
{
    struct passwd *p;
    uid_t uid;
    p = getpwuid(uid = getuid());
    int vall = (int)p->pw_uid;
    size_t len = strlen(prev) + strlen(next) + 7;
    result = realloc(result, len);
    snprintf(result,len, "%s%d%s", prev, vall, next);

}

static void expand_processid(char *prev, char *next, char *result)
{
    pid_t pid = getpid();
    size_t len = strlen(prev) + strlen(next) + 7;
    snprintf(result,len, "%s%d%s", prev, pid, next);
}

static void expand_random(char *prev, char *next, char *result)
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
    snprintf(result,len, "%s%d%s", prev, nb, next);
}

static void expand_pwd(char *prev, char *next, char *result)
{
    char *buffer = malloc(1024 * sizeof(char));
    getcwd(buffer, 1024);
    size_t len = strlen(prev) + strlen(next) + strlen(buffer) + 1;
    snprintf(result,len, "%s%s%s", prev, buffer, next);
    free(buffer);
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
    char *result = calloc(1, strlen(word) + 1024);
    char *prev = calloc(1, strlen(word) + 1);
    char *next = calloc(1, strlen(word) + 1);
    char *var = delimite_var(prev, next, word);
    char *key = delete_dollar(var);
    char *val = "";


    size_t len = strlen(prev) + strlen(next) + 5;
    if (strcmp(key,"UID") == 0)
    {
        expand_UID(prev,next,result);
    }
    else if (strcmp(key,"?") == 0)
    {
        snprintf(result,len, "%s%d%s", prev, h->ret, next);
    }
    else if (strcmp(key,"$") == 0)
    {
        expand_processid(prev,next,result);
    }
    else if (strcmp(key,"#") == 0)
    {
        snprintf(result,len, "%s%d%s", prev, h->nb_args, next);
    }
    else if (strcmp(key,"RANDOM") == 0)
    {
        expand_random(prev,next,result);
    }
    else if (strcmp(key,"PWD") == 0)
    {
        expand_pwd(prev,next,result);
    }
    else if (key[0] >= '0' && key[0] <= '9')
    {
        size_t len = strlen(prev) + strlen(next) + strlen(h->all_args[atoi(key)]);
        result = realloc(result, len);
        snprintf(result,len, "%s%s%s", prev, h->all_args[atoi(key)], next);
    }
    else
    {
        val = hash_map_get(h,key);
        len = strlen(prev) + strlen(val) + strlen(next) + 1;
        snprintf(result,len, "%s%s%s", prev, val, next);
    }
    expand_free(prev,next,var,key);
    if (a)
        free(word);
    //free(val);
    return result;
}

//a faire : $OLDPWD //$@ $* $IFS


char *expand(struct hash_map *h, char *str)
{
    return _expand(h,str);
}
