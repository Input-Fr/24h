#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

union retouir
{
    char *variable_value;
    struct ast *function_value;
};

enum val_type
{
    VARIABLE,
    FUNCTION,
};

struct pair_list
{
    union retouir value;
    enum val_type type;
    char *key;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    int ret; //$?
    int nb_args; //$#
    char **all_args;
    size_t size;
    char *old_pwd;
};

size_t hash(char *str);
struct hash_map *hash_map_init(size_t size);
bool hash_map_insert(struct hash_map *hash_map, char *key, void *value,
                     enum val_type type);
void hash_map_free(struct hash_map *hash_map);
char *hash_map_get(struct hash_map *hash_map, char *key, struct ast **ast);
bool hash_map_remove(struct hash_map *hash_map, char *key);
void hash_map_print(struct hash_map *hash);

int function_run_hashmap(struct ast *ast, struct hash_map *h);
void free_function_hashmap(struct ast *ast);
#endif /* ! HASH_MAP_H */
