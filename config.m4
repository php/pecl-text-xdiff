dnl $Id$
dnl config.m4 for extension xdiff

PHP_ARG_WITH(xdiff, for xdiff support,
[  --with-xdiff             Include xdiff support])


if test "$PHP_XDIFF" != "no"; then

  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/xdiff.h"  # you most likely want to change this
  if test -r $PHP_XDIFF/; then # path given as parameter
    XDIFF_DIR=$PHP_XDIFF
  else # search default path list
    AC_MSG_CHECKING([for xdiff files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        XDIFF_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$XDIFF_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libxdiff distribution])
  fi

  PHP_ADD_INCLUDE($XDIFF_DIR/include)

  EXTRA_LIBS="-lm"
  AC_CHECK_LIB(dl, dlopen, [
    EXTRA_LIBS="$EXTRA_LIBS -ldl"
  ])

  LIBNAME=xdiff
  LIBSYMBOL=xdl_diff

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $XDIFF_DIR/lib, XDIFF_SHARED_LIBADD)
    AC_DEFINE(HAVE_XDIFFLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong xdiff lib version or lib not found])
  ],[
    -L$XDIFF_DIR/lib $EXTRA_LIBS
  ])
  PHP_SUBST(XDIFF_SHARED_LIBADD)

  LIBNAME=xdiff
  LIBSYMBOL=xdl_set_allocator

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    AC_DEFINE(HAVE_XDL_SET_ALLOCATOR,1,[ ])
  ],[   ],[
    -L$XDIFF_DIR/lib
  ])

  dnl
  dnl Check for xdiff 0.9 or greater availability
  dnl
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$XDIFF_DIR/include
  AC_TRY_COMPILE([
#include <xdiff.h>
#include <stdlib.h>
  ], [
memallocator_t a;
a.priv = NULL;
  ], [
    AC_DEFINE(HAVE_XDL_ALLOCATOR_PRIV, 1, [ ])
  ])
  CPPFLAGS=$old_CPPFLAGS

  PHP_NEW_EXTENSION(xdiff, xdiff.c, $ext_shared)
fi
