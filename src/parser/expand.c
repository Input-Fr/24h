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

int test_quote(char *str)  //test if a word is quoted
{
    if ((str[0] == '\'' && str[strlen(str) - 1] == '\'')
            || (str[0] == '"' && str[strlen(str) - 1] == '"'))
    {
        return 1;
    }
    else
    {
        return 0;
    }

}
int test_var(char *str)  //test if a variable is in a word
{
    if (str[0] == '\'' && str[strlen(str) - 1] == '\'')
    {
        return 0;
    }
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
    //result = realloc(result, len);
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

static void expand_all_args(char *prev, char *next, char *result,struct hash_map *h)
{
    char *str = calloc(1, 1024);
    size_t k = 0;
    for(int i = 1; i < h->nb_args + 1; i += 1)
    {
        for (size_t j = 0; (h->all_args[i])[j] != '\0'; j+=1)
        {
            str[k] = (h->all_args[i])[j];
            k+=1;
        }
        str[k] = ' ';
        k+=1;
    }
    str[k-1] = '\0';
    size_t len = strlen(prev) + strlen(next) + 1024;
    snprintf(result,len, "%s%s%s", prev, str, next);
    free(str);
}

static int calcul_len(int nb)
{
    size_t k = 0;
    while (nb > 1)
    {
        nb = nb / 10;
        k += 1;
    }
    return k;
}

static void expand_info_var(char *key, char *prev, char *next, char *result)
{
    if (strcmp(key,"$") == 0)
    {
        expand_processid(prev,next,result);
    }
    else if (strcmp(key,"UID") == 0)
    {
        expand_UID(prev,next,result);
    }
    else if (strcmp(key,"PWD") == 0)
    {
        expand_pwd(prev,next,result);
    }
    else if (strcmp(key,"RANDOM") == 0)
    {
        expand_random(prev,next,result);
    }
}

static int test_info_var(char *key)
{
    if (((strcmp(key,"$") == 0) || (strcmp(key,"UID") == 0) 
                || (strcmp(key,"PWD") == 0)) ||  (strcmp(key,"RANDOM") == 0))
        return 1;
    else
        return 0;
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
    char *result = calloc(1, 1024);
    char *prev = calloc(1, strlen(word) + 1); //word before the variable
    char *next = calloc(1, strlen(word) + 1); //word after the variable
    char *var = delimite_var(prev, next, word);//divide the word in 3 words
    char *key = delete_dollar(var);          //${name} -> name
    char *val = "";
    size_t len = 0;

    if (strcmp(key,"?") == 0)                //$?
    {
        len = strlen(prev) + strlen(next) + calcul_len(h->ret) + 2;
        snprintf(result,len, "%s%d%s", prev, h->ret, next);
    }
    else if (key[0] >= '0' && key[0] <= '9') //$0, $n...
    {
        if (atoi(key) > h->nb_args)
        {
            len = strlen(prev) + strlen(next);
            snprintf(result,len, "%s%s%s", prev, "", next);
        }
        else
        {
            len = strlen(prev) + strlen(next) + strlen(h->all_args[atoi(key)]);
            snprintf(result,len, "%s%s%s", prev, h->all_args[atoi(key)], next);
        }
    }
    else if (strcmp(key,"#") == 0)           //$#
    {
        len = strlen(prev) + strlen(next) + calcul_len(h->nb_args) + 2;
        snprintf(result,len, "%s%d%s", prev, h->nb_args, next);
    }
    else if (strcmp(key,"*") == 0)          //$*
    {
        expand_all_args(prev,next,result,h);
    }
    else if (test_info_var(key))            //$$, $PUID, $RANDOM, $PWD
    {
        expand_info_var(key,prev,next,result);
    }
    else          // variable classique : $name, ${name},"$name"...
    {
        val = hash_map_get(h,key);    //get the value of the variable
        len = strlen(prev) + strlen(val) + strlen(next) + 1;
        snprintf(result,len, "%s%s%s", prev, val, next); //concat
    }

    expand_free(prev,next,var,key);
    if (a)
        free(word);
    //free(val);
    return result;
}

//a faire : $OLDPWD $@ et $IFS

char *expand(struct hash_map *h, char *str)
{
    if (test_var(str))
    {
        return _expand(h,str);
    }
    else if (test_quote(str))
    {
        return delete_quote(str);
    }
    return "";
}   
