# lib name
lib_LIBRARIES = libexpand.a

# source files for libparser
libexpand_a_SOURCES = expand.c expand_variables.c expand_arithmetic.c expand_quotes.c expand_cmd_sub.c expand_functions.c expand.h eval/evalexpr.c eval/evalexpr.h eval/fifo_setup_destroy.c eval/lexing.c eval/my_atoi.c  eval/rpn.c  eval/stack.c  eval/step2.c eval/fifo_access.c expand_field_splitting.c

# flags needed for the libparser
libexpand_a_CPPFLAGS = -I$(top_srcdir)/src

libexpand_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
noinst_LIBRARIES = libexpand.a
