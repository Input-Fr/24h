#include "hash_map.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value,
                     bool *updated)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
    *updated = false;
    struct pair_list *list;
    list = malloc(sizeof(struct pair_list));
    if (!list)
    {
        return NULL;
    }
    list->key = key;
    list->value = value;
    size_t k = hash(key) % hash_map->size;
    if (hash_map->data[k] == NULL)
    {
        hash_map->data[k] = list;
        list->next = NULL;
    }
    else
    {
        struct pair_list *p = hash_map->data[k];
        struct pair_list *tmp = hash_map->data[k];
        while (p != NULL && strcmp(p->key, list->key) != 0)
        {
            p = p->next;
        }
        if (p != NULL)
        {
            p->value = list->value;
            free(list);
            *updated = true;
        }
        else
        {
            hash_map->data[k] = list;
            list->next = tmp;
        }
    }
    return true;
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
                free(p);
                p = tmp;
            }
            free(p);
        }
        i += 1;
    }
    free(hash_map->data);
    free(hash_map);
}

void hash_map_dump(struct hash_map *hash_map)
{
    size_t s = hash_map->size;
    size_t i = 0;
    while (i < s)
    {
        struct pair_list *l = hash_map->data[i];
        while (l != NULL)
        {
            if (l->next != NULL)
            {
                printf("%s: %s, ", l->key, l->value);
            }
            else
            {
                printf("%s: %s\n", l->key, l->value);
            }
            l = l->next;
        }
        i += 1;
    }
}

const char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    size_t k = hash(key);

    // size == 0
    //
    if (hash_map == NULL)
    {
        return NULL;
    }
    if (hash_map->size == 0)
    {
        return NULL;
    }
    if (k > hash_map->size)
    {
        k = hash(key) % hash_map->size;
    }

    if (hash_map->data[k] == NULL)
    {
        return NULL;
    }
    else
    {
        struct pair_list *p = hash_map->data[k];
        while (p != NULL && strcmp(p->key, key) != 0)
        {
            p = p->next;
        }
        if (p != NULL)
        {
            return p->value;
        }
    }
    return NULL;
}

bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
    size_t k = hash(key) % hash_map->size;
    if (hash_map->data[k] == NULL)
    {
        return false;
    }
    else
    {
        struct pair_list *p = hash_map->data[k];
        if (strcmp(p->key, key) == 0)
        {
            if (p->next != NULL)
            {
                struct pair_list *sui = hash_map->data[k]->next;
                hash_map->data[k] = sui;
            }
            else
            {
                hash_map->data[k] = NULL;
            }
            free(p);
            return true;
        }
        struct pair_list *pre = p;
        p = p->next;
        while (p != NULL && strcmp(p->key, key) != 0)
        {
            p = p->next;
            pre = pre->next;
        }
        if (p != NULL)
        {
            pre->next = p->next;
            free(p);
            return true;
        }
        return false;
    }
}
