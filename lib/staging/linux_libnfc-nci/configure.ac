#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libnfc_nci_linux], [1.0])
AM_INIT_AUTOMAKE([foreign subdir-objects color-tests])
AC_CONFIG_HEADERS([config1.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Init Libtool
LT_INIT()
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/nfcandroid_nfc_hidlimpl/halimpl/utils/phNxpConfig.cpp])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h sys/time.h unistd.h])
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h termios.h])
AC_CHECK_HEADERS([malloc.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_TYPE_INT16_T
AC_TYPE_INT8_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
_AC_FUNC_MALLOC_IF

AC_CHECK_FUNCS([gettimeofday memmove memset strerror strstr])
AC_CHECK_FUNCS([clock_gettime])
AC_CHECK_FUNCS([select])
AC_CHECK_FUNCS([dlopen])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libnfc-nci.pc])

test -z "$SED" && SED=sed

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
                  [whether to include debug symbols (default is no)])],
  [enable_debug=$enableval],
  [enable_debug=no]
)

if test "x$enable_debug" = xyes; then
  dnl Remove all optimization flags from CFLAGS
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  CPPFLAGS=`echo "$CPPFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9s]*//g'`

  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-g[0-9]*//g'`
  CPPFLAGS=`echo "$CPPFLAGS" | $SED -e 's/-g[0-9]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-g[0-9]*//g'`
  changequote([,])

  CFLAGS="$CFLAGS -g -O0"
 # Can ignore the below flag as either CFLAGS or CXXFLAGS
 # will be added
 # CPPFLAGS="$CPPFLAGS -g -O0"
  CXXFLAGS="$CXXFLAGS -g -O0"
else
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-g[0-9]*//g'`
  CPPFLAGS=`echo "$CPPFLAGS" | $SED -e 's/-g[0-9]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-g[0-9]*//g'`
  changequote([,])
fi

echo "CFLAGS=$CFLAGS"
echo "CPPFLAGS=$CPPFLAGS"
echo "CXXFLAGS=$CXXFLAGS"

AM_CONDITIONAL([DEBUG],   [test x$enable_debug  = xyes])

AC_ARG_VAR(openssldir, openssl installation path)

AC_ARG_ENABLE([llcp1_3],
[  --enable-llcp1_3    enable LLCP1.3 (secured P2P)],
[case "${enableval}" in
  yes) llcp1_3=true ;;
  no)  llcp1_3=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-llcp1_3]) ;;
esac],[llcp1_3=false])

AM_CONDITIONAL([LLCP1_3_ENABLED], [if [$llcp1_3]; then
AC_MSG_NOTICE([LLCP1.3 support is enabled (secure P2P)])
if ["${ac_cv_env_openssldir_set}" = "set"]; then
AC_MSG_NOTICE([openssldir path set])
else
AC_MSG_ERROR(path to openssldir not set (required when LLCP1.3 support is enabled))
fi
else
AC_MSG_NOTICE([LLCP1.3 support is disabled (no secure P2P)])
fi])

AM_CONDITIONAL([LLCP1_3],   [test x$llcp1_3  = xtrue])

AC_OUTPUT
