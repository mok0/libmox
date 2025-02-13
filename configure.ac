# -*- mode: Autoconf; mode: font-lock -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([libmox],[2.2.0],[mortenkjeldgaard@gmail.com])

## Force autoconf to be at least this version
AC_PREREQ([2.71])

## Safety check to see if sources can be located
AC_CONFIG_SRCDIR([src/libatom.c])

## Put autotools' auxilliary files in a subdir
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIRS([m4])

## Enable automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_PROG_AR

## Checks for programs.
#AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

## Initalize libtool
LT_INIT
LT_LIB_M
AC_SUBST(LIBM)

AC_CHECK_LIB(z, gzopen)
AC_CHECK_LIB(gdbm, gdbm_fetch)

## Checks for header files.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP
AC_CHECK_HEADERS([malloc.h stddef.h stdlib.h string.h sys/file.h unistd.h])

## Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

## Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([bzero floor memset pow putenv setenv sqrt strcasecmp strchr strdup strerror strncasecmp strrchr strstr])

## Generate various files
AC_CONFIG_FILES([Makefile
		 libmox.spec
		 libmox.info
                 src/Makefile
                 test/Makefile
		 ])

AC_OUTPUT

####
