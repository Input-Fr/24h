# every part of 42sh
SUBDIRS = lexer parser ast hash_map

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

# linking phase
42sh_LDADD =  lexer/liblexer.a parser/libparser.a ast/libast.a hash_map/libhash_map.a
