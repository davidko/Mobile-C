#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(mc_list, 0.0.1, dko@ucdavis.edu)
AC_CONFIG_SRCDIR([list.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPES(pthread_rwlock_t, [], [], [#include<pthread.h>])

# Checks for library functions.
AC_FUNC_MALLOC

AC_OUTPUT
