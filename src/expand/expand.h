#ifndef expand_H
#define expand_H

#include "hash_map/hash_map.h"

//expand_variables.c
void expand_variables(struct hash_map *h, char *res);

//expand_arithmetics
int test_arithmetic(char *str);

// expand_cmd_sub.c
char *expand_sub_cmd(char *sub_cmd);
int test_cmd_sub(char *in);

//expand_quotes.c
void expand_ari(struct hash_map *h, char *res);
void delete_quote(char *str);
int test_ari(char *str);

//expand_function.c
void delete_c(char *word, size_t *j);
void separator_equal(char *name, char *val, char *as);
int test_var(char *str, size_t *i);
int test_quote(char *str);
int test_back(char *str);
void error_var(char *word);
int calcul_len(int nb);
int test_special_var(char *key);

char *expand(struct hash_map *h, char *str);


#endif /* !expand_H */
