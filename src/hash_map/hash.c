#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *ha;
    ha = malloc(sizeof(struct hash_map));

    if (ha == NULL)
    {
        return NULL;
    }

    ha->size = size;
    size_t s = 0;

    struct pair_list **dat;
    dat = malloc(size * sizeof(struct pair_list *));

    ha->data = dat;
    while (s < size)
    {
        ha->data[s] = NULL;
        s += 1;
    }

    return ha;
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map == NULL)
    {
        return;
    }
    size_t siz = hash_map->size;
    size_t i = 0;

    while (i < siz)
    {
        struct pair_list *p = hash_map->data[i];
        if (p != NULL)
        {
            while (p->next != NULL)
            {
                struct pair_list *tmp = p->next;
                free(p->key);
                free(p->value);
                free(p);
                p = tmp;
            }
            free(p->key);
            free(p->value);
            free(p);
        }
        i += 1;
    }
    free(hash_map->data);
    free(hash_map);
}

size_t hash(char *key)
{
    size_t i = 0;
    size_t hash = 0;

    for (i = 0; key[i] != '\0'; ++i)
        hash += key[i];
    hash += i;

    return hash;
}

// void hash_map_print(struct hash_map *hash)
//{
//     int i = 0;
//     int s = 7;
//     while (i < s)
//     {
//         printf("%d   ", i);
//         struct pair_list *l = hash->data[i];
//         while (l != NULL)
//         {
//             printf("%s : %s    ", l->key, l->value);
//             l = l->next;
//         }
//         printf("\n");
//         i += 1;
//     }
// }
