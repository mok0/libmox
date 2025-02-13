
## Build in these directories
SUBDIRS = src test

AC_LOCAL_AMFLAGS = -I m4 --install

## Distribute these directories:
DIST_SUBDIRS = $(SUBDIRS)

pkgconfigdir = $(libdir)/pkgconfig

## Need to include dummy file NOTES to keep m4/ in distro
EXTRA_DIST = bootstrap libmox.spec.in libmox.info.in Doxyfile m4/NOTES
