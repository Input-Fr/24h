#ifndef EVALEXPR_H
#define EVALEXPR_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hash_map/hash_map.h"

enum token_type
{
    INT,
    ADD,
    SUB,
    MULT,
    DIV,
    MOD,
    EXP,
    LEFT,
    RIGHT
};
struct token
{
    enum token_type type;
    int value;
};

struct list
{
    struct token *data;
    struct list *next;
};
struct fifo
{
    struct list *head;
    struct list *tail;
    size_t size;
};

struct stack
{
    struct token *data;
    struct stack *next;
};

int eval_arithmetic(char *result, char *input, struct hash_map *hash_map);

int lexing(struct fifo *f, char *lexing, struct hash_map *hash_map);

int rpn(struct fifo *f, char *result);
struct fifo *step2(struct fifo *f);

struct fifo *fifo_init(void);
size_t fifo_size(struct fifo *fifo);
void fifo_push(struct fifo *fifo, struct token *tok);
int fifo_head(struct fifo *fifo);
void fifo_pop(struct fifo *fifo);
void fifo_clear(struct fifo *fifo);
void fifo_destroy(struct fifo *fifo);
void fifo_print(const struct fifo *fifo);

struct stack *stack_push(struct stack *s, struct token *tok);
struct stack *stack_pop(struct stack *s);
int stack_peek(struct stack *s);

int my_atoi(const char *str);

#endif /* ! EVALEXPR_H */
