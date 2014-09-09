#

include_HEADERS = lptree.h

includelptree_HEADERS = \
	lptbase.h \
	lptnspec.h lptnspec-def.h \
	lpttree.h

lib_LTLIBRARIES = liblptree.la

liblptree_la_CFLAGS = -I$(top_srcdir) -I$(top_builddir) @LOS_CFLAGS@

liblptree_la_SOURCES = \
	lptnspec.c \
	lpttree.c

BUILT_SOURCES = \
	lptnspec-def.h lptnspec.inl

lptnspec-def.h lptnspec.inl \
: lpttypes.stamp-h

lpttypes.stamp-h: lpttypes.in
	cpp $< | $(LOS_GENTYPES)
	echo >$@
