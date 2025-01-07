#ifndef LEXER_H
#define LEXER_H

#include "token.h"

/**
 * \page Lexer
 *
 * The lexer cuts some input text into blocks called tokens.

 * This process is done **on demand**: the lexer doesn't read the
 * input more than it needs, only creates tokens when lexer_peek
 * or lexer_pop is called, and no token is available.
 *
 * "2 + 3" will produce 3 tokens:
 *   - TOKEN_NUMBER { .value = 2 }
 *   - TOKEN_PLUS
 *   - TOKEN_NUMBER { .value = 3 }
 */

struct lexer
{
    const char *input; // The input data
    size_t pos; // The current offset inside the input data
    struct token current_tok; // The next token, if processed
};

/**
 * \brief Creates a new lexer given an input string.
 */
struct lexer *lexer_new(const char *input);

/**
 ** \brief Frees the given lexer, but not its input.
 */
void lexer_free(struct lexer *lexer);

/**
 * \brief Returns a token from the input string.

 * This function goes through the input string character by character and
 * builds a token. lexer_peek and lexer_pop should call it. If the input is
 * invalid, you must print something on stderr and return the appropriate token.
 */
struct token lexer_next_token(struct lexer *lexer);

/**
 * \brief Returns the next token, but doesn't move forward: calling lexer_peek
 * multiple times in a row always returns the same result.
 * This function is meant to help the parser check if the next token matches
 * some rule.
 */
struct token lexer_peek(struct lexer *lexer);

/**
 * \brief Returns the next token, and removes it from the stream:
 *   calling lexer_pop in a loop will iterate over all tokens until EOF.
 */
struct token lexer_pop(struct lexer *lexer);





void print(struct lexer *lex);

void pretty_print(struct lexer *lex);

//test.c :

int test_if(struct lexer *lexer);
int test_fi(struct lexer *lexer);
int test_then(struct lexer *lexer);
int test_elif(struct lexer *lexer);
int test_else(struct lexer *lexer);
int test_EOF(struct lexer *lexer);

//token.c :

struct token sem(void);
struct token token_if(void);
struct token token_fi(void);
struct token token_then(void);
struct token token_elif(void);
struct token token_else(void);
struct token token_error(void);
struct token token_EOF(void);


#endif /* !LEXER_H */
