# lib name
lib_LIBRARIES = liblexer.a

# source files for liblexer
liblexer_a_SOURCES = lexer.c lexer.h token.c print.c token.h lexer.h test.c rw_token.c token_reco.c

# flags needed for the liblexer
liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = liblexer.a
