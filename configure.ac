dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(libgram, 1.0, [http://www.jovislab.com/gramd])

LT_INIT

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_PROG_CXX

AC_LANG_PUSH(C++)
AC_CHECK_HEADER(tr1/unordered_map,[AC_DEFINE([LIBGRAM_HAVE_TR1],[],["Have tr1"])],[])
AC_CHECK_HEADER(unordered_map,[AC_DEFINE([LIBGRAM_HAVE_CXX0X],[],["Have C++0x"])],[])
AC_LANG_POP

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

