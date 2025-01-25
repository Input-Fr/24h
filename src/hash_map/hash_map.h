#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

// union retouir
//{
//     char *variable_value,
//     struct ast *function_value,
// };

// enum type
//{
//     variable,
//     function,
// };

struct pair_list
{
    // union retouir val;
    // enum type;
    char *key;
    char *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_function **fucntion;
    struct pair_list **data;
    int ret; //$?
    int nb_args; //$#
    char **all_args;
    char *old_pwd;
    size_t size;
};

size_t hash(char *str);
struct hash_map *hash_map_init(size_t size);
bool hash_map_insert(struct hash_map *hash_map, char *key, char *value);
void hash_map_free(struct hash_map *hash_map);
char *hash_map_get(struct hash_map *hash_map, char *key);
bool hash_map_remove(struct hash_map *hash_map, char *key);
void hash_map_print(struct hash_map *hash);

#endif /* ! HASH_MAP_H */
