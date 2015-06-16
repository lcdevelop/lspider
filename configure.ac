#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(lspider, 0.1, whlichuang@126.com)
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AX_QT
if test "x$succeeded" != "xyes";then
    AC_MSG_ERROR([checking qt fail])
fi

AX_MONGO_CLIENT
if test "x$succeeded" != "xyes";then
    AC_MSG_ERROR([checking mongo-client fail])
fi

AX_LIBEVENT
if test "x$succeeded" != "xyes";then
    AC_MSG_ERROR([checking libevent fail])
fi

AX_THRIFT
if test "x$succeeded" != "xyes";then
    AC_MSG_ERROR([checking thrift fail])
fi

AX_BOOST
if test "x$succeeded" != "xyes";then
    AC_MSG_ERROR([checking boost fail])
fi

AX_LOG4CPLUS
if test "x$succeeded" != "xyes";then
    AC_MSG_ERROR([checking log4cplus fail])
fi

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
