AC_PREREQ(2.59)
AC_INIT(CoQuiAAS, 2.0, lonca@cril.fr)

PACKAGE=CoQuiAAS
VERSION=2.0

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN], 
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])

AC_CHECK_PROGS(HAVE_PKGCONFIG,pkg-config,1,0)
if test 1 -eq "$HAVE_PKGCONFIG";
   then PKG_CHECK_MODULES([libcppunit], [cppunit], [USE_CPPUNIT=1], [USE_CPPUNIT=0])
fi
AM_CONDITIONAL([HAVE_CPPUNIT], [test "$USE_CPPUNIT" -eq 1])

AM_INIT_AUTOMAKE(subdir-objects)

AC_PROG_LIBTOOL

: ${CXXFLAGS= -std=c++11 -Wno-literal-suffix -Wno-class-memaccess -O3 -DCOQUIAAS_VERSION="2.0"}
dnl : ${CXXFLAGS= -std=c++11 -Wno-literal-suffix -g -O0 -DCOQUIAAS_VERSION="2.0"}
AC_PROG_CXX
AX_CHECK_ZLIB

AC_OUTPUT(Makefile resources/Makefile src/Makefile doc/Makefile)
