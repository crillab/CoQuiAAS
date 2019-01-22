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

AM_INIT_AUTOMAKE(subdir-objects)

AC_PROG_LIBTOOL

: ${CXXFLAGS= -std=c++11 -Wno-literal-suffix -g -O3 -DCOQUIAAS_VERSION="3.0"}
AC_PROG_CXX

AX_CHECK_ZLIB

AX_BOOST_BASE([1.54],,[AC_MSG_ERROR([CoQuiAAS needs Boost, but it was not found in your system])])
AX_BOOST_LOG([1.54],,[AC_MSG_ERROR([CoQuiAAS needs Boost_LOG, but it was not found in your system])])
AX_BOOST_LOG_SETUP([1.54],,[AC_MSG_ERROR([CoQuiAAS needs Boost_LOG_SETUP, but it was not found in your system])])
AX_BOOST_SYSTEM([1.54],,[AC_MSG_ERROR([CoQuiAAS needs Boost_SYSTEM, but it was not found in your system])])
AX_BOOST_THREAD([1.54],,[AC_MSG_ERROR([CoQuiAAS needs Boost_THREAD, but it was not found in your system])])

AC_OUTPUT(Makefile resources/Makefile src/Makefile doc/Makefile)
