AC_INIT([lptree], [0.0.0])
AM_INIT_AUTOMAKE

: ${CFLAGS:=-g -Wall}

AC_SUBST([pkgconfigdir], '${libdir}/pkgconfig')

AC_PROG_CC
LT_INIT

# [fixme] check for piftest
PKG_CHECK_MODULES([PIFTEST], [piftest])
AC_SUBST([PIFTEST])
AC_MSG_CHECKING([for piftest])
if PIFTEST=`which piftest`; then
    AC_MSG_RESULT([$PIFTEST])
else
    AC_MSG_RESULT([no])
    AC_ERROR([piftest not found])
fi

#PKG_CHECK_MODULES([GLIB], [glib-2.0])

AC_CONFIG_FILES([ \
  Makefile \
  lptree/Makefile \
  test/Makefile \
])
AC_CONFIG_FILES([lptree.pc])
AC_CONFIG_FILES([test/piftest.conf])
AC_OUTPUT
