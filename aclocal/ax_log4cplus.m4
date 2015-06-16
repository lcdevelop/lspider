# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_log4cplus.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LOG4CPLUS([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the log4cplus C++ libraries of a particular version (or newer)
#
#   If no path to the installed log4cplus library is given the macro searchs
#   under /usr, /usr/local, /opt and /opt/local and evaluates the
#   $LOG4CPLUS_ROOT environment variable. Further documentation is available at
#   <http://randspringer.de/log4cplus/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(LOG4CPLUS_CPPFLAGS) / AC_SUBST(LOG4CPLUS_LDFLAGS)
#
#   And sets:
#
#     HAVE_LOG4CPLUS
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2009 Peter Adolphs
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 25

AC_DEFUN([AX_LOG4CPLUS],
[
AC_ARG_WITH([log4cplus],
  [AS_HELP_STRING([--with-log4cplus@<:@=ARG@:>@],
    [use log4cplus library from a standard location (ARG=yes),
     from the specified location (ARG=<path>),
     or disable it (ARG=no)
     @<:@ARG=yes@:>@ ])],
    [
    if test "$withval" = "no"; then
        want_log4cplus="no"
    elif test "$withval" = "yes"; then
        want_log4cplus="yes"
        ac_log4cplus_path=""
    else
        want_log4cplus="yes"
        ac_log4cplus_path="$withval"
    fi
    ],
    [want_log4cplus="yes"])


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

if test "x$want_log4cplus" = "xyes"; then
    log4cplus_lib_version_req=ifelse([$1], ,1.20.0,$1)
    log4cplus_lib_version_req_shorten=`expr $log4cplus_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
    log4cplus_lib_version_req_major=`expr $log4cplus_lib_version_req : '\([[0-9]]*\)'`
    log4cplus_lib_version_req_minor=`expr $log4cplus_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    log4cplus_lib_version_req_sub_minor=`expr $log4cplus_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$log4cplus_lib_version_req_sub_minor" = "x" ; then
        log4cplus_lib_version_req_sub_minor="0"
        fi
    WANT_LOG4CPLUS_VERSION=`expr $log4cplus_lib_version_req_major \* 100000 \+  $log4cplus_lib_version_req_minor \* 100 \+ $log4cplus_lib_version_req_sub_minor`
    AC_MSG_CHECKING(for log4cpluslib >= $log4cplus_lib_version_req)
    succeeded=no

    dnl On 64-bit systems check for system libraries in both lib64 and lib.
    dnl The former is specified by FHS, but e.g. Debian does not adhere to
    dnl this (as it rises problems for generic multi-arch support).
    dnl The last entry in the list is chosen by default when no libraries
    dnl are found, e.g. when only header-only libraries are installed!
    libsubdirs="lib"
    ax_arch=`uname -m`
    case $ax_arch in
      x86_64)
        libsubdirs="lib64 libx32 lib lib64"
        ;;
      ppc64|s390x|sparc64|aarch64|ppc64le)
        libsubdirs="lib64 lib lib64 ppc64le"
        ;;
    esac

    dnl allow for real multi-arch paths e.g. /usr/lib/x86_64-linux-gnu. Give
    dnl them priority over the other paths since, if libs are found there, they
    dnl are almost assuredly the ones desired.
    AC_REQUIRE([AC_CANONICAL_HOST])
    libsubdirs="lib/${host_cpu}-${host_os} $libsubdirs"

    case ${host_cpu} in
      i?86)
        libsubdirs="lib/i386-${host_os} $libsubdirs"
        ;;
    esac

    dnl first we check the system location for log4cplus libraries
    dnl this location ist chosen if log4cplus libraries are installed with the --layout=system option
    dnl or if you install log4cplus with RPM
    if test "$ac_log4cplus_path" != ""; then
        LOG4CPLUS_CPPFLAGS="-I$ac_log4cplus_path/include"
        for ac_log4cplus_path_tmp in $libsubdirs; do
                if test -d "$ac_log4cplus_path"/"$ac_log4cplus_path_tmp" ; then
                        LOG4CPLUS_LDFLAGS="-L$ac_log4cplus_path/$ac_log4cplus_path_tmp"
                        break
                fi
        done
    elif test "$cross_compiling" != yes; then
        for ac_log4cplus_path_tmp in /usr /usr/local /opt /opt/local ; do
            if test -d "$ac_log4cplus_path_tmp/include/log4cplus" && test -r "$ac_log4cplus_path_tmp/include/log4cplus"; then
                for libsubdir in $libsubdirs ; do
                    if ls "$ac_log4cplus_path_tmp/$libsubdir/liblog4cplus_"* >/dev/null 2>&1 ; then break; fi
                done
                LOG4CPLUS_LDFLAGS="-L$ac_log4cplus_path_tmp/$libsubdir"
                LOG4CPLUS_CPPFLAGS="-I$ac_log4cplus_path_tmp/include"
                break;
            fi
        done
    fi

    dnl overwrite ld flags if we have required special directory with
    dnl --with-log4cplus-libdir parameter
    if test "$ac_log4cplus_lib_path" != ""; then
       LOG4CPLUS_LDFLAGS="-L$ac_log4cplus_lib_path"
    fi

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $LOG4CPLUS_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $LOG4CPLUS_LDFLAGS"
    export LDFLAGS

    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH(C++)
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                                           @%:@include <QApplication>
    ]], [[
                                       int argc2;
                                       char * argv2[2];
                                       QApplication app(argc2, argv2);
    ]])],[
        AC_MSG_RESULT(yes)
    succeeded=yes
    found_system=yes
        ],[
        ])
    AC_LANG_POP([C++])



    dnl if we found no log4cplus with system layout we search for log4cplus libraries
    dnl built and installed without the --layout=system option or for a staged(not installed) version
    if test "x$succeeded" != "xyes"; then
        _version=0
        if test "$ac_log4cplus_path" != ""; then
            if test -d "$ac_log4cplus_path" && test -r "$ac_log4cplus_path"; then
                for i in `ls -d $ac_log4cplus_path/include/log4cplus-* 2>/dev/null`; do
                    _version_tmp=`echo $i | sed "s#$ac_log4cplus_path##" | sed 's/\/include\/log4cplus-//' | sed 's/_/./'`
                    V_CHECK=`expr $_version_tmp \> $_version`
                    if test "$V_CHECK" = "1" ; then
                        _version=$_version_tmp
                    fi
                    VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                    LOG4CPLUS_CPPFLAGS="-I$ac_log4cplus_path/include/log4cplus-$VERSION_UNDERSCORE"
                done
            fi
        else
            if test "$cross_compiling" != yes; then
                for ac_log4cplus_path in /usr /usr/local /opt /opt/local ; do
                    if test -d "$ac_log4cplus_path" && test -r "$ac_log4cplus_path"; then
                        for i in `ls -d $ac_log4cplus_path/include/log4cplus-* 2>/dev/null`; do
                            _version_tmp=`echo $i | sed "s#$ac_log4cplus_path##" | sed 's/\/include\/log4cplus-//' | sed 's/_/./'`
                            V_CHECK=`expr $_version_tmp \> $_version`
                            if test "$V_CHECK" = "1" ; then
                                _version=$_version_tmp
                                best_path=$ac_log4cplus_path
                            fi
                        done
                    fi
                done

                VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                LOG4CPLUS_CPPFLAGS="-I$best_path/include/log4cplus-$VERSION_UNDERSCORE"
                if test "$ac_log4cplus_lib_path" = ""; then
                    for libsubdir in $libsubdirs ; do
                        if ls "$best_path/$libsubdir/liblog4cplus_"* >/dev/null 2>&1 ; then break; fi
                    done
                    LOG4CPLUS_LDFLAGS="-L$best_path/$libsubdir"
                fi
            fi

            if test "x$LOG4CPLUS_ROOT" != "x"; then
                for libsubdir in $libsubdirs ; do
                    if ls "$LOG4CPLUS_ROOT/stage/$libsubdir/liblog4cplus_"* >/dev/null 2>&1 ; then break; fi
                done
                if test -d "$LOG4CPLUS_ROOT" && test -r "$LOG4CPLUS_ROOT" && test -d "$LOG4CPLUS_ROOT/stage/$libsubdir" && test -r "$LOG4CPLUS_ROOT/stage/$libsubdir"; then
                    version_dir=`expr //$LOG4CPLUS_ROOT : '.*/\(.*\)'`
                    stage_version=`echo $version_dir | sed 's/log4cplus_//' | sed 's/_/./g'`
                        stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
                    V_CHECK=`expr $stage_version_shorten \>\= $_version`
                    if test "$V_CHECK" = "1" -a "$ac_log4cplus_lib_path" = "" ; then
                        AC_MSG_NOTICE(We will use a staged log4cplus library from $LOG4CPLUS_ROOT)
                        LOG4CPLUS_CPPFLAGS="-I$LOG4CPLUS_ROOT"
                        LOG4CPLUS_LDFLAGS="-L$LOG4CPLUS_ROOT/stage/$libsubdir"
                    fi
                fi
            fi
        fi

        CPPFLAGS="$CPPFLAGS $LOG4CPLUS_CPPFLAGS"
        export CPPFLAGS
        LDFLAGS="$LDFLAGS $LOG4CPLUS_LDFLAGS"
        export LDFLAGS

        AC_LANG_PUSH(C++)
            AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                                           @%:@include <QApplication>
        ]], [[
                                       int argc2;
                                       char * argv2[2];
                                       QApplication app(argc2, argv2);
        ]])],[
            AC_MSG_RESULT(yes)
        succeeded=yes
        found_system=yes
            ],[
            ])
        AC_LANG_POP([C++])
    fi

    if test "$succeeded" != "yes" ; then
        if test "$_version" = "0" ; then
            AC_MSG_NOTICE([[We could not detect the log4cplus libraries (version $log4cplus_lib_version_req_shorten or higher). If you have a staged log4cplus library (still not installed) please specify \$LOG4CPLUS_ROOT in your environment and do not give a PATH to --with-log4cplus option.  If you are sure you have log4cplus installed, then check your version number looking in <log4cplus/version.hpp>. See http://randspringer.de/log4cplus for more documentation.]])
        else
            AC_MSG_NOTICE([Your log4cplus libraries seems to old (version $_version).])
        fi
        # execute ACTION-IF-NOT-FOUND (if present):
        ifelse([$3], , :, [$3])
    else
        AC_SUBST(LOG4CPLUS_CPPFLAGS)
        AC_SUBST(LOG4CPLUS_LDFLAGS)
        AC_DEFINE(HAVE_LOG4CPLUS,,[define if the log4cplus library is available])
        # execute ACTION-IF-FOUND (if present):
        ifelse([$2], , :, [$2])
    fi

    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
fi

])
