#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(lspider, 0.1, whlichuang@126.com)
AC_CONFIG_SRCDIR([src/mysql_base.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AX_QT
#AC_MSG_ERROR([$ac_qt_include_path], [$want_qt], [$ac_qt_lib_path])
AC_MSG_RESULT([ac_qt_include_path=$ac_qt_include_path])
AC_MSG_RESULT([ac_qt_lib_path=$ac_qt_lib_path])
AC_MSG_ERROR([stop])

#if test "x$succeeded" = "xno" ; then
#    AC_MSG_ERROR(["Error: thrift required"])
#fi

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero gettimeofday memset select socket strchr strcspn strerror strncasecmp])

AC_OUTPUT([Makefile])
