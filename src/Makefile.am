# -*- mode: Makefile ; mode: font-lock -*-

AM_CFLAGS =

libmox_h_sources = atom.h colorlib.h database.h pdb.h	\
transform.h util.h veclib.h pdblib.h

libmox_c_sources = colorlib.c database.c getword.c libatom.c lqsort.c	\
o_util.c pdblib.c pdbutil.c util.c veclib.c

library_includedir = $(includedir)/mox
library_include_HEADERS = $(libmox_h_sources)

lib_LTLIBRARIES = libmox.la 
libmox_la_SOURCES = $(libmox_c_sources) $(libmox_h_sources)
libmox_la_LDFLAGS = -version-info 1:0:0

####
