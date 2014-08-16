AC_INIT([lptree], [0.0.0])
AM_INIT_AUTOMAKE

: ${CFLAGS:=-g -Wall}

AC_SUBST([pkgconfigdir], '${libdir}/pkgconfig')

AC_PROG_CC
LT_INIT

#PKG_CHECK_MODULES([GLIB], [glib-2.0])

AC_CONFIG_FILES([ \
  Makefile \
])
AC_CONFIG_FILES([lptree.pc])
AC_OUTPUT
