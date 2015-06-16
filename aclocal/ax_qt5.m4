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
])
