AC_DEFUN([AX_QT],
[
AC_ARG_WITH([qt-include],
  [AS_HELP_STRING([--with-qt-include=INCLUDE_DIR],
    [use qt])],
        [
        if test -d "$withval"
        then
                ac_qt_include_path="$withval"
        else
                AC_MSG_ERROR(--with-qt-include expected directory name)
        fi
        ],
        [ac_qt_include_path=""]
)


AC_ARG_WITH([qt-libdir],
        AS_HELP_STRING([--with-qt-libdir=LIB_DIR],
        [Force given directory for qt libraries. Note that this will override library path detection, so use this parameter only if default library detection fails and you know exactly where your qt libraries are located.]),
        [
        if test -d "$withval"
        then
                ac_qt_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-qt-libdir expected directory name)
        fi
        ],
        [ac_qt_lib_path=""]
)

libsubdirs="lib64 libx32 lib lib64"

if test "$ac_qt_include_path" != ""; then
    QT_CPPFLAGS="-I$ac_qt_include_path"
else
    ac_qt_include_path="/usr/include/qt5/"
    QT_CPPFLAGS="-I$ac_qt_include_path"
fi

if test "$ac_qt_lib_path" != ""; then
    QT_LDFLAGS="-L$ac_qt_lib_path"
else
    for libsubdir in $libsubdirs ; do
        if ls "$/usr/$libsubdir/libQt"* >/dev/null 2>&1 ; then break; fi
    done
    ac_qt_lib_path="/usr/$libsubdir/"
    QT_LDFLAGS="-L$ac_qt_lib_path"
fi

succeeded=no
if ls "$ac_qt_include_path/QtGui" >/dev/null 2>&1 ; then
    if ls "$ac_qt_lib_path/libQt"* >/dev/null 2>&1 ; then
        succeeded=yes
    fi
fi

AC_SUBST(QT_CPPFLAGS)
AC_SUBST(QT_LDFLAGS)

])
