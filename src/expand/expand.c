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
    char *res = calloc(1, (strlen(str) * 2) + 1024);
    strcpy(res, str);
    expand_variables(h, res);
    expand_substi(res);
    expand_ari(h, res);
    delete_quote(res);

    return res;
}
