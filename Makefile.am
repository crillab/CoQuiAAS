ACLOCAL_AMFLAGS = -I m4

SUBDIRS = resources src doc

EXTRA_DIST = build.sh resources

dist-hook:
	find $(distdir)/resources -name "*.[ao]" -exec rm -rf {} \;
