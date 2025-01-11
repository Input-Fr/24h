# every part of 42sh
SUBDIRS = lexer parser exec_ast

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -fsanitize=address

# linking phase
42sh_LDADD =  lexer/liblexer.a parser/libparser.a exec_ast/libexec.a
