#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

struct pair_list
{
    char *key;
    char *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    int ret;  //$?
    int nb_args; //$#
    //char *arg; //$1 $2 ...
    char **all_args;
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
