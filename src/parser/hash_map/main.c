#include <stdbool.h>
#include <stdio.h>

#include "hash_map.h"

int main(void)
{
    bool boo = false;
    struct hash_map *hash = hash_map_init(7);

    hash_map_insert(hash, "TARAN", "W", &boo);
    //hash_map_insert(hash, "C", "42", &boo);
    //hash_map_insert(hash, "42", "life", &boo);
    hash_map_insert(hash, "SKI", "winter", &boo);
    hash_map_insert(hash, "ACU", "51", &boo);
    //hash_map_insert(hash, "ACU", "52", &boo);
    //hash_map_insert(hash, "test", "42", &boo);
    hash_map_insert(hash, "a","5", &boo);
    hash_map_remove(hash, "a");
    int i = 0;
    int s = 7;
    //printf("\n");
    //printf("--- %s\n", hash_map_get(hash,"SK"));
    //printf("\n");
    //while (i < s)
    //{
    //    printf("%d   ", i);
    //    struct pair_list *l = hash->data[i];
    //    while (l != NULL)
    //    {
    //        printf("%s (%s) ", l->key, l->value);
    //        l = l->next;
    //    }
    //    printf("\n");
    //    i += 1;
    //}
    //    printf("%s", hash_map_get(hash, "o"));
    //hash_map_dump(hash);
    hash_map_print(hash);
    printf("\n");
    //hash_map_remove(hash, "42");
    //hash_map_remove(hash, "test");
    //hash_map_remove(hash, "ACU");
    //hash_map_dump(hash);

    hash_map_free(hash);

    return 0;
}
