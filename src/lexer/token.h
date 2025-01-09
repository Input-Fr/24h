#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_WORD, // command
    TOKEN_BOOL, // bool
    TOKEN_IF, // if
    TOKEN_THEN, // then
    TOKEN_ELIF, // elif
    TOKEN_ELSE, // else
    TOKEN_FI, // fi
    TOKEN_SEMI, // ;
    TOKEN_NEWLINE, // \n
    TOKEN_COM, // #
    TOKEN_QUO, // 'wd'
    TOKEN_EOF,
    TOKEN_ERROR
};

struct token
{
    enum token_type type;
    char *str;
};

#endif /* !TOKEN_H */
