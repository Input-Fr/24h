#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool hash_map_insert(struct hash_map *hash_map, char *key, char *value)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
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
        }
        else
        {
            hash_map->data[k] = list;
            list->next = tmp;
        }
    }
    return true;
}

char *hash_map_get(struct hash_map *hash_map, char *key)
{
    size_t k = hash(key);
    if (hash_map == NULL || hash_map->size == 0)
    {
        return NULL;
    }
    if (k > hash_map->size)
    {
        k = hash(key) % hash_map->size;
    }

    if (hash_map->data[k] == NULL)
    {
        return "";
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
    return "";
}

bool hash_map_remove(struct hash_map *hash_map, char *key)
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
