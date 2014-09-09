AC_INIT([lptree], [0.0.0])
AM_INIT_AUTOMAKE

: ${CFLAGS:=-g -Wall}

AC_SUBST([pkgconfigdir], '${libdir}/pkgconfig')
AC_SUBST([includelptreedir], ['${includedir}/lptree'])

AC_PROG_CC
AM_PROG_CC_C_O
LT_INIT

dnl # [fixme] check for piftest
dnl PKG_CHECK_MODULES([PIFTEST], [piftest])
dnl AC_SUBST([PIFTEST])
dnl AC_MSG_CHECKING([for piftest])
dnl if PIFTEST=`which piftest`; then
dnl     AC_MSG_RESULT([$PIFTEST])
dnl else
dnl     AC_MSG_RESULT([no])
dnl     AC_ERROR([piftest not found])
dnl fi

#PKG_CHECK_MODULES([GLIB], [glib-2.0])

# [fixme]
#AM_PATH_PIFTEST
AM_PIFTEST_SUITE([test])
dnl test -d test/.deps || mkdir -vp test/.deps
dnl for n in lptree-test test_base; do
dnl     f="test/.deps/$n.Po"
dnl     test -e "$f" || echo "$n.o: $n.c" >"$f"
dnl done

PKG_CHECK_MODULES([LOS], [los])
AC_PATH_PROG([LOS_GENTYPES], [los-gentypes])
test x"$LOS_GENTYPES" != x || AC_ERROR([los-gentypes not found])

AC_CONFIG_FILES([ \
  Makefile \
  lptree/Makefile \
  test/Makefile \
])
AC_CONFIG_FILES([lptree.pc])
AC_OUTPUT
