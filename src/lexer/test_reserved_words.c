#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static int test_if(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 2 && strcmp(str, "if") == 0) {
    return 1;
  }
  return 0;
}

static int test_fi(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 2 && strcmp(str, "fi") == 0) {
    return 1;
  }
  return 0;
}

static int test_then(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 4 && strcmp(str, "then") == 0) {
    return 1;
  }
  return 0;
}

static int test_elif(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 4 && strcmp(str, "elif") == 0) {
    return 1;
  }
  return 0;
}

static int test_else(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 4 && strcmp(str, "else") == 0) {
    return 1;
  }
  return 0;
}

static int test_do(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 2 && strcmp(str, "do") == 0) {
    return 1;
  }
  return 0;
}

static int test_done(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 4 && strcmp(str, "done") == 0) {
    return 1;
  }
  return 0;
}

static int test_case(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 4 && strcmp(str, "case") == 0) {
    return 1;
  }
  return 0;
}

static int test_esac(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 4 && strcmp(str, "esac") == 0) {
    return 1;
  }
  return 0;
}

static int test_while(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 5 && strcmp(str, "while") == 0) {
    return 1;
  }
  return 0;
}

static int test_until(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 5 && strcmp(str, "until") == 0) {
    return 1;
  }
  return 0;
}

static int test_for(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 3 && strcmp(str, "for") == 0) {
    return 1;
  }
  return 0;
}

static int test_lbrace(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 1 && strcmp(str, "{") == 0) {
    return 1;
  }
  return 0;
}

static int test_rbrace(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 1 && strcmp(str, "}") == 0) {
    return 1;
  }
  return 0;
}

static int test_bang(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 1 && strcmp(str, "!") == 0) {
    return 1;
  }
  return 0;
}

static int test_in(struct lexer *lexer) {
  char *str = lexer->current_tok.data->str;
  if (lexer->current_tok.data->size == 2 && strcmp(str, "in") == 0) {
    return 1;
  }
  return 0;
}

int test_name(char *name) {
  size_t i = 0;
  if (!((name[0] >= 'a' && name[0] <= 'z') ||
        (name[0] >= 'A' && name[0] <= 'Z') || name[0] == '_')) {
    return 0;
  }

  while (name[i] != '\0' && name[i] != '=') {
    if (!((name[i] >= 'a' && name[i] <= 'z') ||
          (name[i] >= 'A' && name[i] <= 'Z') || name[i] == '_' ||
          (name[i] >= '0' && name[i] <= '9'))) {
      return 0;
    }
    i += 1;
  }

  if (name[i] == '=') {
    return 1;
  }
  return 1;
}

int test_ass(struct lexer *lexer) {
  struct token tok = lexer->current_tok;
  char *str = tok.data->str;

  if (tok.type == TOKEN_WORD && str[0] != '=' && str[0] != '"' &&
      str[0] != '\'') {
    size_t i = 0;
    if (str[0] != '=' && (str[0] != '"' || str[0] != '\'')) {
      while (str[i] != '\0' && str[i] != '=') {
        i += 1;
      }

      if (str[i] == '=') {
        if (!test_name(str)) {
          exit(127);
        }
        return 1;
      }
    }
  }
  return 0;
}

static enum token_type reserved_word_1(struct lexer *lexer) {
  if (test_if(lexer))
    return TOKEN_IF;
  else if (test_fi(lexer))
    return TOKEN_FI;
  else if (test_elif(lexer))
    return TOKEN_ELIF;
  else if (test_else(lexer))
    return TOKEN_ELSE;
  else if (test_then(lexer))
    return TOKEN_THEN;
  else if (test_do(lexer))
    return TOKEN_DO;
  else if (test_done(lexer))
    return TOKEN_DONE;
  else if (test_case(lexer))
    return TOKEN_CASE;
  else
    return NO_TOKEN;
}

static int token_rw(struct lexer *lexer) {
  if (test_if(lexer) || test_fi(lexer) || test_elif(lexer) ||
      test_else(lexer) || test_then(lexer) || test_do(lexer) ||
      test_done(lexer) || test_case(lexer)) {
    return 1;
  } else {
    return 0;
  }
}

enum token_type reserved_word(struct lexer *lexer) {
  if (test_ass(lexer))
    return TOKEN_ASSIGNMENT_WORD;
  if (token_rw(lexer))
    return reserved_word_1(lexer);
  else if (test_esac(lexer))
    return TOKEN_ESAC;
  else if (test_while(lexer))
    return TOKEN_WHILE;
  else if (test_until(lexer))
    return TOKEN_UNTIL;
  else if (test_for(lexer))
    return TOKEN_FOR;
  else if (test_lbrace(lexer))
    return TOKEN_LBRACE;
  else if (test_rbrace(lexer))
    return TOKEN_RBRACE;
  else if (test_bang(lexer))
    return TOKEN_BANG;
  else if (test_in(lexer))
    return TOKEN_IN;
  else
    return TOKEN_WORD;
}
