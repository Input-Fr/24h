# lib name
lib_LIBRARIES = libast.a

# source files for libparser
libast_a_SOURCES = ast.h cd.h run.c cd.c run2.c free.c free2.c init.c init2.c push.c push2.c

# flags needed for the libparser
libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
noinst_LIBRARIES = libast.a
