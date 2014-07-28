$(builddir)/libopenredir.c: $(srcdir)/libopenredir.c.in
	tornado_tmpl $< $@

lib_LTLIBRARIES = libopenredir.la
libopenredir_la_SOURCES = libopenredir.c
libopenredir_la_LDFLAGS = -export-dynamic -avoid-version
EXTRA_DIST = libopenredir.c.in

bin_PROGRAMS=openredir
openredir_SOURCES=openredir.c
AM_CFLAGS=-DLIBDIR=\"${libdir}\"
openredir_LDADD=libopenredir.la
openredir_LDFLAGS=-ldl -llua

