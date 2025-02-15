
## Build in these directories
SUBDIRS = src test

AC_LOCAL_AMFLAGS = -I m4 --install

## Distribute these directories:
DIST_SUBDIRS = $(SUBDIRS)

pkgconfigdir = $(libdir)/pkgconfig

## Need to include dummy file NOTES to keep m4/ in distro
EXTRA_DIST = bootstrap Doxyfile m4/NOTES

# This target name is an Automake extension point:

all-local:
	mkdir -p doc/doxygen/html
	doxygen

install-data-local: install-html-docs

uninstall-local:
	rm -r $(DESTDIR)$(docdir)


.PHONY: install-html-docs
install-html-docs:
	mkdir -p $(DESTDIR)$(docdir)
	cp -a doc/doxygen/html $(DESTDIR)$(docdir)

.PHONY: clean-local
clean-local:
	rm -rf doc/doxygen/
