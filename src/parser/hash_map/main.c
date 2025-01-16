#include <stdbool.h>
#include <stdio.h>

#include "hash_map.h"

int main(void)
{
    struct hash_map *hash = hash_map_init(7);

    hash_map_insert(hash, "TARAN", "W");
    hash_map_insert(hash, "SKI", "winter");
    hash_map_insert(hash, "ACU", "51");
    hash_map_insert(hash, "a","5");
    hash_map_print(hash);
    hash_map_insert(hash, "a","6");
    hash_map_print(hash);
    //hash_map_remove(hash, "a");
    printf("%s",hash_map_get(hash,"h"));
    int i = 0;
    int s = 7;
    printf("\n");

    hash_map_free(hash);

    return 0;
}
