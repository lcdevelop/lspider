AC_DEFUN([AX_LOG4CPLUS],
[
AC_ARG_WITH([log4cplus-include],
  [AS_HELP_STRING([--with-log4cplus-include=INCLUDE_DIR],
    [use log4cplus])],
        [
        if test -d "$withval"
        then
                ac_log4cplus_include_path="$withval"
        else
                AC_MSG_ERROR(--with-log4cplus-include expected directory name)
        fi
        ],
        [ac_log4cplus_include_path=""]
)


AC_ARG_WITH([log4cplus-libdir],
        AS_HELP_STRING([--with-log4cplus-libdir=LIB_DIR],
        [Force given directory for log4cplus libraries. Note that this will override library path detection, so use this parameter only if default library detection fails and you know exactly where your log4cplus libraries are located.]),
        [
        if test -d "$withval"
        then
                ac_log4cplus_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-log4cplus-libdir expected directory name)
        fi
        ],
        [ac_log4cplus_lib_path=""]
)

libsubdirs="lib64 libx32 lib lib64"

if test "$ac_log4cplus_include_path" != ""; then
    LOG4CPLUS_CPPFLAGS="-I$ac_log4cplus_include_path"
else
    ac_log4cplus_include_path="/usr/include/"
    LOG4CPLUS_CPPFLAGS="-I$ac_log4cplus_include_path"
fi

if test "$ac_log4cplus_lib_path" != ""; then
    LOG4CPLUS_LDFLAGS="-L$ac_log4cplus_lib_path"
else
    for libsubdir in $libsubdirs ; do
        if ls "$/usr/$libsubdir/liblog4cplus"* >/dev/null 2>&1 ; then break; fi
    done
    ac_log4cplus_lib_path="/usr/$libsubdir/"
    LOG4CPLUS_LDFLAGS="-L$ac_log4cplus_lib_path"
fi

succeeded=no
if ls "$ac_log4cplus_include_path/log4cplus" >/dev/null 2>&1 ; then
    if ls "$ac_log4cplus_lib_path/liblog4cplus"* >/dev/null 2>&1 ; then
        succeeded=yes
    fi
fi

AC_SUBST(LOG4CPLUS_CPPFLAGS)
AC_SUBST(LOG4CPLUS_LDFLAGS)

])
