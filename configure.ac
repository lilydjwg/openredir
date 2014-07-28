AC_PREREQ([2.69])
AC_INIT([openredir], 1.0)
AC_CONFIG_SRCDIR([openredir.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

# Checks for libraries.
AC_DEFUN([REQUIRE_LIB], [AC_CHECK_LIB($1, $2, [], [AC_MSG_ERROR($1 library missing or unusable)])])
REQUIRE_LIB([dl], [dlsym])
REQUIRE_LIB([lua], [luaL_newstate])

# Checks for header files.
AC_CHECK_HEADERS([locale.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([setenv setlocale])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
