# -*- mode: Makefile ; mode: font-lock -*-

INCLUDES = -I$(top_srcdir)/include/mox -I$(prefix)/include
AM_CFLAGS = -DRAMADIR=\"$(pkgdatadir)\"

libmox_h_sources = atom.h colorlib.h database.h map2d.h pdb.h pdblib.h	\
transform.h util.h vector.h

libmox_c_sources = colorlib.c database.c getword.c		\
libatom.c lqsort.c map2d.c outil.c pdbutil.c util.c vector.c	\

libmox_f_sources = jacobi.f

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(libmox_h_sources)

lib_LTLIBRARIES = libmox.la 
libmox_la_SOURCES = $(libmox_c_sources) $(libmox_f_sources) $(libmox_h_sources)

####
