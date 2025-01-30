#ifndef expand_H
#define expand_H

#include "hash_map/hash_map.h"

// expand_variables.c
void expand_variables(struct hash_map *h, char *res);

// expand_arithmetics
int test_arithmetic(char *str);

// expand_cmd_sub.c
void expand_substi(char *res);
int test_cmd_sub(char *in);

// expand_quotes.c
void expand_ari(struct hash_map *h, char *res);
void delete_quote(char *str);
int test_ari(char *str);

// expand_function.c
void delete_c(char *word, size_t *j);
void separator_equal(char *name, char *val, char *as);
int test_var(char *str);
int test_quote(char *str);
int test_back(char *str);
void error_var(char *word);
int calcul_len(int nb);
int test_special_var(char *key);
int is_special_var(char *str, size_t *i);

// expand_field_splitting.c
void field_splitting(char *str, char *check, char **res, struct hash_map *h);
size_t len_ifs(char *str, struct hash_map *h);

char *expand(struct hash_map *h, char *str);

#endif /* !expand_H */
