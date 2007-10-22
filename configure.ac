# -*- mode: Autoconf; mode: font-lock -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(moxlib, 2.0, mok@bioxray.dk)


AC_CONFIG_SRCDIR([misc/chaincat.c])
AC_CONFIG_AUX_DIR([config])
#AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_CHECK_LIBM
AC_SUBST(LIBM)

CHECK_ZLIB
CHECK_GDBM

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stddef.h stdlib.h string.h sys/file.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([bzero floor memset pow putenv setenv sqrt strcasecmp strchr strdup strerror strncasecmp strrchr strstr])

AM_PATH_GSL(1.5)
if test "x$GSL_LIBS" == x; then
   AC_MSG_NOTICE([GSL not found, attempt to use LAPACK])
   ACX_LAPACK()
else
   AC_MSG_NOTICE([Using GSL])
   AC_DEFINE(HAVE_GSL,1,[Define if you have the GSL library.])
fi

AM_PATH_CGRAPH

AC_CONFIG_FILES([Makefile
		 mox.spec
		 mox.info
                 src/Makefile
		 ])

AC_OUTPUT

####
