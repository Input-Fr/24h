#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

static int is_reserved_words(struct lexer *lexer) {
  struct token tok = lexer->current_tok;
  if (tok.type == TOKEN_IF || tok.type == TOKEN_FI || tok.type == TOKEN_ELIF ||
      tok.type == TOKEN_ELSE || tok.type == TOKEN_THEN ||
      tok.type == TOKEN_DO || tok.type == TOKEN_DONE ||
      tok.type == TOKEN_CASE || tok.type == TOKEN_ESAC ||
      tok.type == TOKEN_ESAC || tok.type == TOKEN_WHILE ||
      tok.type == TOKEN_UNTIL || tok.type == TOKEN_FOR ||
      tok.type == TOKEN_DO || tok.type == TOKEN_LBRACE ||
      tok.type == TOKEN_RBRACE || tok.type == TOKEN_BANG ||
      tok.type == TOKEN_IN)
    return 1;
  else
    return 0;
}

void clear_current_tok(struct lexer *lexer) {
  if (lexer->current_tok.data != NULL) {
    if (lexer->current_tok.type != TOKEN_WORD &&
        lexer->current_tok.type != TOKEN_ASSIGNMENT_WORD &&
        lexer->current_tok.data->str != NULL && !is_reserved_words(lexer)) {
      free(lexer->current_tok.data->str);
    }
    mbt_str_free(lexer->current_tok.data);
    lexer->current_tok.data = NULL;
    lexer->current_tok.type = NO_TOKEN;
  }
}

void mbt_str_free(struct mbt_str *str) { free(str); }

struct mbt_str *mbt_str_init(void) {
  struct mbt_str *str = malloc(sizeof(struct mbt_str));
  str->str = calloc(1, 6);
  str->capacity = 5;
  str->size = 0;
  return str;
}

void mbt_str_pushc(struct mbt_str *str, char c) {
  if (str->size >= str->capacity) {
    str->capacity += 5;
    str->str = realloc(str->str, str->capacity + 1);
    (str->str)[str->size] = c;
    (str->str)[str->size + 1] = '\0';
  } else {
    (str->str)[str->size] = c;
    (str->str)[str->size + 1] = '\0';
  }

  str->size += 1;
}
