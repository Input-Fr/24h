#include "parser.h"
#include "../ast/ast.h"
#include "../lexer/lexer.h"

int is_reserved_words(struct lexer *lexer)
{
    struct token tok = lexer->current_tok;
    if (tok.type == TOKEN_IF || tok.type == TOKEN_FI || tok.type == TOKEN_ELIF
        || tok.type == TOKEN_ELSE || tok.type == TOKEN_THEN
        || tok.type == TOKEN_DO || tok.type == TOKEN_DONE
        || tok.type == TOKEN_CASE || tok.type == TOKEN_ESAC
        || tok.type == TOKEN_ESAC || tok.type == TOKEN_WHILE
        || tok.type == TOKEN_UNTIL || tok.type == TOKEN_FOR
        || tok.type == TOKEN_DO || tok.type == TOKEN_LBRACE
        || tok.type == TOKEN_RBRACE || tok.type == TOKEN_BANG
        || tok.type == TOKEN_IN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

char *reserved_words_to_char(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_IF)
        return "if";
    else if (tok.type == TOKEN_FI)
        return "fi";
    else if (tok.type == TOKEN_ELIF)
        return "elif";
    else if (tok.type == TOKEN_ELSE)
        return "else";
    else if (tok.type == TOKEN_THEN)
        return "then";
    else if (tok.type == TOKEN_DO)
        return "do";
    else if (tok.type == TOKEN_DONE)
        return "done";
    else if (tok.type == TOKEN_CASE)
        return "case";
    else if (tok.type == TOKEN_ESAC)
        return "esac";
    else if (tok.type == TOKEN_WHILE)
        return "while";
    else if (tok.type == TOKEN_UNTIL)
        return "until";
    else if (tok.type == TOKEN_FOR)
        return "for";
    else if (tok.type == TOKEN_DO)
        return "do";
    else if (tok.type == TOKEN_LBRACE)
        return "}";
    else if (tok.type == TOKEN_RBRACE)
        return "}";
    else
        return "";
}
