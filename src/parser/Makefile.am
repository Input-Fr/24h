# lib name
lib_LIBRARIES = libparser.a

# source files for libparser
libparser_a_SOURCES = parser.c parser.h ast.c ast.h

# flags needed for the libparser
libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libparser.a
