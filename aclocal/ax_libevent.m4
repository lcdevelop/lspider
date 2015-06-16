# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_libevent.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIBEVENT([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the libevent C++ libraries of a particular version (or newer)
#
#   If no path to the installed libevent library is given the macro searchs
#   under /usr, /usr/local, /opt and /opt/local and evaluates the
#   $LIBEVENT_ROOT environment variable. Further documentation is available at
#   <http://randspringer.de/libevent/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(LIBEVENT_CPPFLAGS) / AC_SUBST(LIBEVENT_LDFLAGS)
#
#   And sets:
#
#     HAVE_LIBEVENT
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

AC_DEFUN([AX_LIBEVENT],
[
AC_ARG_WITH([libevent],
  [AS_HELP_STRING([--with-libevent@<:@=ARG@:>@],
    [use libevent library from a standard location (ARG=yes),
     from the specified location (ARG=<path>),
     or disable it (ARG=no)
     @<:@ARG=yes@:>@ ])],
    [
    if test "$withval" = "no"; then
        want_libevent="no"
    elif test "$withval" = "yes"; then
        want_libevent="yes"
        ac_libevent_path=""
    else
        want_libevent="yes"
        ac_libevent_path="$withval"
    fi
    ],
    [want_libevent="yes"])


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

if test "x$want_libevent" = "xyes"; then
    libevent_lib_version_req=ifelse([$1], ,1.20.0,$1)
    libevent_lib_version_req_shorten=`expr $libevent_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
    libevent_lib_version_req_major=`expr $libevent_lib_version_req : '\([[0-9]]*\)'`
    libevent_lib_version_req_minor=`expr $libevent_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    libevent_lib_version_req_sub_minor=`expr $libevent_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$libevent_lib_version_req_sub_minor" = "x" ; then
        libevent_lib_version_req_sub_minor="0"
        fi
    WANT_LIBEVENT_VERSION=`expr $libevent_lib_version_req_major \* 100000 \+  $libevent_lib_version_req_minor \* 100 \+ $libevent_lib_version_req_sub_minor`
    AC_MSG_CHECKING(for libeventlib >= $libevent_lib_version_req)
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

    dnl first we check the system location for libevent libraries
    dnl this location ist chosen if libevent libraries are installed with the --layout=system option
    dnl or if you install libevent with RPM
    if test "$ac_libevent_path" != ""; then
        LIBEVENT_CPPFLAGS="-I$ac_libevent_path/include"
        for ac_libevent_path_tmp in $libsubdirs; do
                if test -d "$ac_libevent_path"/"$ac_libevent_path_tmp" ; then
                        LIBEVENT_LDFLAGS="-L$ac_libevent_path/$ac_libevent_path_tmp"
                        break
                fi
        done
    elif test "$cross_compiling" != yes; then
        for ac_libevent_path_tmp in /usr /usr/local /opt /opt/local ; do
            if test -d "$ac_libevent_path_tmp/include/libevent" && test -r "$ac_libevent_path_tmp/include/libevent"; then
                for libsubdir in $libsubdirs ; do
                    if ls "$ac_libevent_path_tmp/$libsubdir/liblibevent_"* >/dev/null 2>&1 ; then break; fi
                done
                LIBEVENT_LDFLAGS="-L$ac_libevent_path_tmp/$libsubdir"
                LIBEVENT_CPPFLAGS="-I$ac_libevent_path_tmp/include"
                break;
            fi
        done
    fi

    dnl overwrite ld flags if we have required special directory with
    dnl --with-libevent-libdir parameter
    if test "$ac_libevent_lib_path" != ""; then
       LIBEVENT_LDFLAGS="-L$ac_libevent_lib_path"
    fi

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $LIBEVENT_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $LIBEVENT_LDFLAGS"
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



    dnl if we found no libevent with system layout we search for libevent libraries
    dnl built and installed without the --layout=system option or for a staged(not installed) version
    if test "x$succeeded" != "xyes"; then
        _version=0
        if test "$ac_libevent_path" != ""; then
            if test -d "$ac_libevent_path" && test -r "$ac_libevent_path"; then
                for i in `ls -d $ac_libevent_path/include/libevent-* 2>/dev/null`; do
                    _version_tmp=`echo $i | sed "s#$ac_libevent_path##" | sed 's/\/include\/libevent-//' | sed 's/_/./'`
                    V_CHECK=`expr $_version_tmp \> $_version`
                    if test "$V_CHECK" = "1" ; then
                        _version=$_version_tmp
                    fi
                    VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                    LIBEVENT_CPPFLAGS="-I$ac_libevent_path/include/libevent-$VERSION_UNDERSCORE"
                done
            fi
        else
            if test "$cross_compiling" != yes; then
                for ac_libevent_path in /usr /usr/local /opt /opt/local ; do
                    if test -d "$ac_libevent_path" && test -r "$ac_libevent_path"; then
                        for i in `ls -d $ac_libevent_path/include/libevent-* 2>/dev/null`; do
                            _version_tmp=`echo $i | sed "s#$ac_libevent_path##" | sed 's/\/include\/libevent-//' | sed 's/_/./'`
                            V_CHECK=`expr $_version_tmp \> $_version`
                            if test "$V_CHECK" = "1" ; then
                                _version=$_version_tmp
                                best_path=$ac_libevent_path
                            fi
                        done
                    fi
                done

                VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                LIBEVENT_CPPFLAGS="-I$best_path/include/libevent-$VERSION_UNDERSCORE"
                if test "$ac_libevent_lib_path" = ""; then
                    for libsubdir in $libsubdirs ; do
                        if ls "$best_path/$libsubdir/liblibevent_"* >/dev/null 2>&1 ; then break; fi
                    done
                    LIBEVENT_LDFLAGS="-L$best_path/$libsubdir"
                fi
            fi

            if test "x$LIBEVENT_ROOT" != "x"; then
                for libsubdir in $libsubdirs ; do
                    if ls "$LIBEVENT_ROOT/stage/$libsubdir/liblibevent_"* >/dev/null 2>&1 ; then break; fi
                done
                if test -d "$LIBEVENT_ROOT" && test -r "$LIBEVENT_ROOT" && test -d "$LIBEVENT_ROOT/stage/$libsubdir" && test -r "$LIBEVENT_ROOT/stage/$libsubdir"; then
                    version_dir=`expr //$LIBEVENT_ROOT : '.*/\(.*\)'`
                    stage_version=`echo $version_dir | sed 's/libevent_//' | sed 's/_/./g'`
                        stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
                    V_CHECK=`expr $stage_version_shorten \>\= $_version`
                    if test "$V_CHECK" = "1" -a "$ac_libevent_lib_path" = "" ; then
                        AC_MSG_NOTICE(We will use a staged libevent library from $LIBEVENT_ROOT)
                        LIBEVENT_CPPFLAGS="-I$LIBEVENT_ROOT"
                        LIBEVENT_LDFLAGS="-L$LIBEVENT_ROOT/stage/$libsubdir"
                    fi
                fi
            fi
        fi

        CPPFLAGS="$CPPFLAGS $LIBEVENT_CPPFLAGS"
        export CPPFLAGS
        LDFLAGS="$LDFLAGS $LIBEVENT_LDFLAGS"
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
            AC_MSG_NOTICE([[We could not detect the libevent libraries (version $libevent_lib_version_req_shorten or higher). If you have a staged libevent library (still not installed) please specify \$LIBEVENT_ROOT in your environment and do not give a PATH to --with-libevent option.  If you are sure you have libevent installed, then check your version number looking in <libevent/version.hpp>. See http://randspringer.de/libevent for more documentation.]])
        else
            AC_MSG_NOTICE([Your libevent libraries seems to old (version $_version).])
        fi
        # execute ACTION-IF-NOT-FOUND (if present):
        ifelse([$3], , :, [$3])
    else
        AC_SUBST(LIBEVENT_CPPFLAGS)
        AC_SUBST(LIBEVENT_LDFLAGS)
        AC_DEFINE(HAVE_LIBEVENT,,[define if the libevent library is available])
        # execute ACTION-IF-FOUND (if present):
        ifelse([$2], , :, [$2])
    fi

    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
fi

])
