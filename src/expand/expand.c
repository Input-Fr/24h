#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "expand.h"
#include "ast/ast.h"
#include "hash_map/hash_map.h"
#include "expand/eval/evalexpr.h"


  

char *expand(struct hash_map *h, char *str)
{
    char *res = calloc(1, strlen(str) * 2);
    strcpy(res, str);

    if (test_var(str))
    {
        expand_variables(h, str, res);
    }

    if (test_ari(str))
    {
        expand_ari(h, str, res);
    }

    if (test_quote(res))
    {
        delete_quote(res);
    }

    return res;
}
