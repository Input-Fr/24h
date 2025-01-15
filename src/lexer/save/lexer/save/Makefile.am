# lib name
lib_LIBRARIES = liblexer.a

# source files for liblexer
liblexer_a_SOURCES = lexer.c lexer.h next_token.c test_reserved_words.c mbt_str.c print.c test_operators.c

# flags needed for the liblexer
liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -fsanitize=address

noinst_LIBRARIES = liblexer.a
