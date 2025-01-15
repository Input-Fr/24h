#include <stdbool.h>
#include <stdio.h>

#include "hash_map.h"

void hash_map_print(struct hash_map *hash)
{
    int i = 0;
    int s = 7;
    while (i < s)
    {
        printf("%d   ", i);
        struct pair_list *l = hash->data[i];
        while (l != NULL)
        {
            printf("%s : %s    ", l->key, l->value);
            l = l->next;
        }
        printf("\n");
        i += 1;
    }
}
