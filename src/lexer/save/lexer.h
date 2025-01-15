#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
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

enum token_type
{
//reserved words
    TOKEN_WORD, // command
    TOKEN_IF, // if
    TOKEN_THEN, // then
    TOKEN_ELIF, // elif
    TOKEN_ELSE, // else
    TOKEN_FI, // fi
    TOKEN_CASE, //case
    TOKEN_ESAC, //esac
    TOKEN_WHILE, //while
    TOKEN_UNTIL, //until
    TOKEN_FOR, //for
    TOKEN_DO, //do
    TOKEN_LBRACE, // {
    TOKEN_RBRACE, //}
    TOKEN_BANG, // !
    TOKEN_IN, //in
    TOKEN_DONE, //done
// ??
    TOKEN_NEWLINE, // \n
    TOKEN_COM, // #
    TOKEN_QUOTE, // '
// operators
    TOKEN_SEMI, // ;
    TOKEN_PIPE, // |
    TOKEN_AND, // &
    TOKEN_AND_IF, // &&
    TOKEN_OR_IF, // ||
    TOKEN_DSEMI, // ;;
    TOKEN_DLESS, // <<
    TOKEN_DGREAT, // >>
    TOKEN_LESSAND, // <&
    TOKEN_GREATAND, //>&
    TOKEN_LESSGREAT, //<>
    TOKEN_DLESSDASH, //<<-
    TOKEN_CLOBBER, // >|
    TOKEN_EOF,
    TOKEN_ERROR,
    NO_TOKEN,
};

struct mbt_str
{
    char *str;
    size_t size;
    size_t capacity;
};

struct token
{
    enum token_type type;
    struct mbt_str *data;
};

enum quote
{
    NO_QUOTE,
    BACKSLASH_QUOTE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
};

struct lexer
{
    char input;
    struct token current_tok; // The next token, if processed
    int peek;
    enum quote Quoting;
    FILE *file;
};

/**
 * \brief Creates a new lexer given an input string.
 */
struct lexer *lexer_new(void);

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

// print.c :

void print_lex(struct lexer *lex);

// lexer.c :

// test_reserved_words.c :

enum token_type reserved_word(struct lexer *lexer);

// token.c :

// mbt_str.c

void mbt_str_free(struct mbt_str *str);
struct mbt_str *mbt_str_init(void);
void mbt_str_pushc(struct mbt_str *str, char c);
void clear_current_tok(struct lexer *lex);

char lexer_file(FILE *hd);
void lexer_file_back(FILE *hd);

//test_operators.c:

void operator_1(struct lexer *lexer);
int test_operator_1(struct lexer *lexer);
int test_operator(struct lexer *lexer);
struct token lexer_next_token(struct lexer *lexer);


#endif /* !LEXER_H */
