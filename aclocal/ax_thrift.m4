AC_DEFUN([AX_THRIFT],
[
AC_ARG_WITH([thrift-include],
  [AS_HELP_STRING([--with-thrift-include=INCLUDE_DIR],
    [use thrift])],
        [
        if test -d "$withval"
        then
                ac_thrift_include_path="$withval"
        else
                AC_MSG_ERROR(--with-thrift-include expected directory name)
        fi
        ],
        [ac_thrift_include_path=""]
)


AC_ARG_WITH([thrift-libdir],
        AS_HELP_STRING([--with-thrift-libdir=LIB_DIR],
        [Force given directory for thrift libraries. Note that this will override library path detection, so use this parameter only if default library detection fails and you know exactly where your thrift libraries are located.]),
        [
        if test -d "$withval"
        then
                ac_thrift_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-thrift-libdir expected directory name)
        fi
        ],
        [ac_thrift_lib_path=""]
)

libsubdirs="lib64 libx32 lib lib64"

if test "$ac_thrift_include_path" != ""; then
    THRIFT_CPPFLAGS="-I$ac_thrift_include_path"
else
    ac_thrift_include_path="/usr/include/"
    THRIFT_CPPFLAGS="-I$ac_thrift_include_path"
fi

if test "$ac_thrift_lib_path" != ""; then
    THRIFT_LDFLAGS="-L$ac_thrift_lib_path"
else
    for libsubdir in $libsubdirs ; do
        if ls "$/usr/$libsubdir/libthrift"* >/dev/null 2>&1 ; then break; fi
    done
    ac_thrift_lib_path="/usr/$libsubdir/"
    THRIFT_LDFLAGS="-L$ac_thrift_lib_path"
fi

succeeded=no
if ls "$ac_thrift_include_path/thrift" >/dev/null 2>&1 ; then
    if ls "$ac_thrift_lib_path/libthrift"* >/dev/null 2>&1 ; then
        succeeded=yes
    fi
fi

AC_SUBST(THRIFT_CPPFLAGS)
AC_SUBST(THRIFT_LDFLAGS)

])
