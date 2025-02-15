
## Build in these directories
SUBDIRS = src test

AC_LOCAL_AMFLAGS = -I m4 --install

## Distribute these directories:
DIST_SUBDIRS = $(SUBDIRS)

pkgconfigdir = $(libdir)/pkgconfig

## Need to include dummy file NOTES to keep m4/ in distro
EXTRA_DIST = bootstrap libmox.spec.in libmox.info.in Doxyfile m4/NOTES NEWS

# include Doxygen rules (requires autoconf-archive >2016-03-20)
@DX_RULES@

doxygen-prepare:
	mkdir -p doc/doxygen/html

docs: doxygen-prepare doxygen-doc

clean-local:
	-rm -rf doc/doxygen/

# Try to tell autotools to install documentation

html_DATA = $(builddir)/doc/doxygen/html/*
