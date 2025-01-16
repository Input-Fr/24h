# lib name
lib_LIBRARIES = libparser.a

# source files for libparser
libparser_a_SOURCES = parser.c ast.h parser.h pretty_print.c run.c free.c init.c hash_map/hash.c hash_map/hash_map.c hash_map/hash_map.h

# flags needed for the libparser
libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -fsanitize=address
noinst_LIBRARIES = libparser.a
