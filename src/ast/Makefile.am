# lib name
lib_LIBRARIES = libast.a

# source files for libparser
libast_a_SOURCES = ast.h pretty_print.c pretty_print2.c run.c free.c free2.c init.c expand.c push.c expand_function.c

# flags needed for the libparser
libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
noinst_LIBRARIES = libast.a
