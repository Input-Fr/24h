# Init 42sh
AC_INIT([42sh], [1.0])

# Init automake
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# Pretty display of Makefile rules
AM_SILENT_RULES([yes])

# check ar dependency
AM_PROG_AR

# check ranlib dependency
AC_PROG_RANLIB

# check if we have a C compiler
AC_PROG_CC

# check if these flag are installed
AX_COMPILER_FLAGS([], [], [], [-Wall -Wextra -Werror -Wvla -pedantic -std=c99])

# List Makefiles in subdirectories
AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/lexer/Makefile
                    src/parser/Makefile
                    src/exec_ast/Makefile
                ])
AC_OUTPUT
