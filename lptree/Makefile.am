#

include_HEADERS = lptree.h

includelptree_HEADERS = \
	lptbase.h \
	lptnspec.h lptnspec-def.h \
	lptnspecint.h lptnspecint-def.h \
	lptnode.h lptnode-def.h \
	lpttree.h lpttree-def.h

lib_LTLIBRARIES = liblptree.la

liblptree_la_CFLAGS = -I$(top_srcdir) -I$(top_builddir) @LOS_CFLAGS@

liblptree_la_SOURCES = \
	lptnspec.c \
	lptnspecint.c \
	lptnode.c \
	lpttree.c

BUILT_SOURCES = \
	lptnspec-def.h lptnspec.inl \
	lptnspecint-def.h lptnspecint.inl \
	lptnode-def.h lptnode.inl \
	lpttree-def.h lpttree.inl

lptnspec-def.h lptnspec.inl \
lptnspecint-def.h lptnspecint.inl \
lptnode-def.h lptnode.inl \
lpttree-def.h lpttree.inl \
: lpttypes.stamp-h

lpttypes.stamp-h: lpttypes.in
	cpp $< | $(LOS_GENTYPES)
	echo >$@
