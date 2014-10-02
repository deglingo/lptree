#

include_HEADERS = lptree.h

includelptree_HEADERS = \
	lptbase.h \
	lptpath.h \
	lptevent.h \
	lptnspec.h lptnspec-def.h \
	lptnspecdir.h lptnspecdir-def.h \
	lptnspecint.h lptnspecint-def.h \
	lptnode.h lptnode-def.h \
	lpttree.h lpttree-def.h

lib_LTLIBRARIES = liblptree.la

liblptree_la_CFLAGS = -DCL_LOG_DOMAIN=\"lptree\" -I$(top_srcdir) -I$(top_builddir) @LOS_CFLAGS@

liblptree_la_SOURCES = \
	lptpath.c \
	lptnspec.c \
	lptnspecdir.c \
	lptnspecint.c \
	lptnode.c \
	lpttree.c

BUILT_SOURCES = \
	lptnspec-def.h lptnspec.inl \
	lptnspecdir-def.h lptnspecdir.inl \
	lptnspecint-def.h lptnspecint.inl \
	lptnode-def.h lptnode.inl \
	lpttree-def.h lpttree.inl

lptnspec-def.h lptnspec.inl \
lptnspecdir-def.h lptnspecdir.inl \
lptnspecint-def.h lptnspecint.inl \
lptnode-def.h lptnode.inl \
lpttree-def.h lpttree.inl \
: lpttypes.stamp-h

lpttypes.stamp-h: lpttypes.in
	cpp $< | $(LOS_GENTYPES)
	echo >$@
