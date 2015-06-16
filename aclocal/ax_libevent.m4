AC_DEFUN([AX_LIBEVENT],
[
AC_ARG_WITH([libevent-include],
  [AS_HELP_STRING([--with-libevent-include=INCLUDE_DIR],
    [use libevent])],
        [
        if test -d "$withval"
        then
                ac_libevent_include_path="$withval"
        else
                AC_MSG_ERROR(--with-libevent-include expected directory name)
        fi
        ],
        [ac_libevent_include_path=""]
)


AC_ARG_WITH([libevent-libdir],
        AS_HELP_STRING([--with-libevent-libdir=LIB_DIR],
        [Force given directory for libevent libraries. Note that this will override library path detection, so use this parameter only if default library detection fails and you know exactly where your libevent libraries are located.]),
        [
        if test -d "$withval"
        then
                ac_libevent_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-libevent-libdir expected directory name)
        fi
        ],
        [ac_libevent_lib_path=""]
)

libsubdirs="lib64 libx32 lib lib64"

if test "$ac_libevent_include_path" != ""; then
    LIBEVENT_CPPFLAGS="-I$ac_libevent_include_path"
else
    ac_libevent_include_path="/usr/include/"
    LIBEVENT_CPPFLAGS="-I$ac_libevent_include_path"
fi

if test "$ac_libevent_lib_path" != ""; then
    LIBEVENT_LDFLAGS="-L$ac_libevent_lib_path"
else
    for libsubdir in $libsubdirs ; do
        if ls "$/usr/$libsubdir/libevent"* >/dev/null 2>&1 ; then break; fi
    done
    ac_libevent_lib_path="/usr/$libsubdir/"
    LIBEVENT_LDFLAGS="-L$ac_libevent_lib_path"
fi

succeeded=no
if ls "$ac_libevent_include_path/event.h" >/dev/null 2>&1 ; then
    if ls "$ac_libevent_lib_path/libevent"* >/dev/null 2>&1 ; then
        succeeded=yes
    fi
fi

AC_SUBST(LIBEVENT_CPPFLAGS)
AC_SUBST(LIBEVENT_LDFLAGS)

])
