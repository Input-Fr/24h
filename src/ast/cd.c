#define _POSIX_C_SOURCE 200809L
#include "cd.h"

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PMAX 4096

static bool dot_dotdot(char *directory)
{
    char first_component[PMAX];

    if (directory == NULL || strlen(directory) == 0)
    {
        return false;
    }

    char *slash_position = strchr(directory, '/');
    if (slash_position)
    {
        size_t length = slash_position - directory;
        strncpy(first_component, directory, length);
        first_component[length] = '\0';
    }
    else
    {
        strncpy(first_component, directory, PMAX);
    }

    if (strcmp(first_component, ".") == 0 || strcmp(first_component, "..") == 0)
    {
        return true;
    }

    return false;
}

static char *convert_to_canonical(const char *curpath)
{
    if (!curpath || !*curpath)
    {
        return strdup("");
    }

    char **components = calloc(strlen(curpath), sizeof(char *));
    int comp_count = 0;

    char *path_copy = strdup(curpath);
    char *saveptr;
    char *token = strtok_r(path_copy, "/", &saveptr);

    // store in components
    while (token)
    {
        if (strcmp(token, "") != 0
            && strcmp(token, ".") != 0) // dot or / skipped
        {
            if (strcmp(token, "..") == 0)
            {
                if (comp_count > 0
                    && strcmp(components[comp_count - 1], "..") != 0)
                {
                    free(components[--comp_count]);
                }
                else
                {
                    components[comp_count++] = strdup("..");
                }
            }
            else
            {
                components[comp_count++] = strdup(token);
            }
        }
        token = strtok_r(NULL, "/", &saveptr);
    }

    size_t total_len = 1;
    for (int i = 0; i < comp_count; i++)
    {
        total_len += strlen(components[i]) + 1; // +1 for slash
    }
    total_len += 2; // null + possibly the last /

    char *result = calloc(total_len, 1);
    result[0] = '\0';

    if (curpath[0] == '/')
    {
        strcat(result, "/");
    }

    for (int i = 0; i < comp_count; i++)
    {
        if (i > 0)
            strcat(result, "/");
        strcat(result, components[i]);
    }

    for (int i = 0; i < comp_count; i++)
    {
        free(components[i]);
    }
    free(components);
    free(path_copy);

    return result;
}
static void set_curpath_to_dirop(char *curpath, char *arg)
{
    if (strlen(arg) < PMAX)
    {
        curpath = strcpy(curpath, arg);
    }
    else
    {
        errx(2, "file/path name too long");
    }
}
static int nocomment(char *home_val)
{
    int ret = cmd_cd(home_val);
    return ret;
}

static void clean(int allocated, char *pwd, char *curpath);

static void rule7(char *pwd, char *curpath);

static void rule9(char *curpath, char *arg, char *pwd);

static int minus_hyphen(void)
{
        char *oldchap = getenv("OLDPWD");
        int ret = cmd_cd(oldchap);
        printf("%s\n", oldchap);
        return ret;
}

int cmd_cd(char *arg) // following SCL algorithm
{
    char *home_val = getenv("HOME");
    char *pwd = getenv("PWD");
    if (!strcmp("-", arg))
    {
        minus_hyphen();
    }
    int allocated = 0;
    int step7 = 0;
    if (pwd == NULL)
    {
        allocated = 1;
        pwd = calloc(PMAX, 1);
        pwd = getcwd(pwd, PMAX);
    }
    char *curpath = calloc(PMAX, 1);
    if ((arg == NULL || !strcmp(arg, "")) && home_val == NULL) // 1
    {
        clean(allocated, pwd, curpath);
        return 1;
    }
    if ((arg == NULL || !strcmp(arg, "")) && home_val != NULL) // 2
    {
        clean(allocated, pwd, curpath);
        return nocomment(home_val);
    }
    if (arg && arg[0] == '/') // 3
    {
        set_curpath_to_dirop(curpath, arg);
        step7 = 1;
    }
    if (!step7 && dot_dotdot(arg)) // 4
    {
        (void)step7;
    }
    if (!step7) // 6 (5 skipped not to implement)
    {
        set_curpath_to_dirop(curpath, arg);
    }
    if (curpath[0] != '/') // 7
    {
        step7 = 0;
        rule7(pwd, curpath);
    }
    char *new = convert_to_canonical(curpath); // 8
    free(curpath);
    curpath = new;
    rule9(curpath, arg, pwd);
    setenv("OLDPWD", pwd, 1);
    int ret = chdir(curpath);
    clean(allocated, pwd, curpath);
    return ret;
}

static void rule7(char *pwd, char *curpath)
{
        char *newpath = calloc(strlen(pwd) + strlen(curpath) + 2, 1);
        strcpy(newpath, pwd);

        if (newpath[strlen(newpath) - 1] != '/')
        {
            strcat(newpath, "/");
        }

        strcat(newpath, curpath);
        free(curpath);
        curpath = newpath;
}

static void rule9(char *curpath, char *arg, char *pwd)
{
    // ------------- 9
    size_t pmax = pathconf("/", _PC_PATH_MAX);
    if ((strlen(curpath) + 1 > pmax && strlen(arg) + 1 <= pmax)
        || strlen(curpath) < pmax || strlen(arg) >= pmax)
    {
        if (pwd != NULL)
        {
            size_t pwd_len = strlen(pwd);
            char pwd_slash[PMAX + 2];
            snprintf(pwd_slash, sizeof(pwd_slash), "%s%s", pwd,
                     pwd[pwd_len - 1] == '/' ? "" : "/");
            if (strncmp(curpath, pwd_slash, strlen(pwd_slash))
                == 0) // pwd in curpath ?
            {
                size_t prefix_len = strlen(pwd_slash);
                memmove(curpath, curpath + prefix_len,
                        strlen(curpath) - prefix_len + 1);
            }
        }
    }
    // --------------
}

static void clean(int allocated, char *pwd, char *curpath)
{
    if (allocated)
    {
        free(pwd);
    }
    free(curpath);
}
