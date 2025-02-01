#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../ast/ast.h"

bool hash_map_insert(struct hash_map *hash_map, char *key, void *value,
        enum val_type type)
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

    list->type = type;
    if (list->type == VARIABLE)
    {
        list->value.variable_value = value;
    }
    if (list->type == FUNCTION)
    {
        list->value.function_value = value;
    }
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
            if (p->type == VARIABLE)
            {
                p->value.variable_value = value;
            }
            if (p->type == FUNCTION)
            {
                p->value.function_value = value;
            }
            // p->value = list->value;
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

static char * get_function_name(struct ast *ast)
{
    assert(ast && ast->type == AST_FUNCTION);
    struct ast_function *fct = (struct ast_function *)ast;
    return fct->fname;
}

char *hash_map_get(struct hash_map *hash_map, char *key,struct ast **ast)
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
            if (p->type == VARIABLE)
            {
                return p->value.variable_value;
            }
            if (p->type == FUNCTION)
            {
                *ast = p->value.function_value;
                return get_function_name(*ast);
            }
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

            // if (p->type == VARIABLE && strcmp(p->key, "IFS") != 0)
            if (p->type == VARIABLE)
                free(p->value.variable_value);
            if (p->type == FUNCTION)
                free_function_hashmap(p->value.function_value);
            // if (strcmp(p->key, "IFS") != 0)
            free(p->key);

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
            free(p->key);

            if (p->type == VARIABLE)
                free(p->value.variable_value);
            if (p->type == FUNCTION)
                free(p->value.function_value);

            free(p);
            return true;
        }
        return false;
    }
}
