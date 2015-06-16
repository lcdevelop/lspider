# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_thrift.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_THRIFT([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the thrift C++ libraries of a particular version (or newer)
#
#   If no path to the installed thrift library is given the macro searchs
#   under /usr, /usr/local, /opt and /opt/local and evaluates the
#   $THRIFT_ROOT environment variable. Further documentation is available at
#   <http://randspringer.de/thrift/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(THRIFT_CPPFLAGS) / AC_SUBST(THRIFT_LDFLAGS)
#
#   And sets:
#
#     HAVE_THRIFT
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

AC_DEFUN([AX_THRIFT],
[
AC_ARG_WITH([thrift],
  [AS_HELP_STRING([--with-thrift@<:@=ARG@:>@],
    [use thrift library from a standard location (ARG=yes),
     from the specified location (ARG=<path>),
     or disable it (ARG=no)
     @<:@ARG=yes@:>@ ])],
    [
    if test "$withval" = "no"; then
        want_thrift="no"
    elif test "$withval" = "yes"; then
        want_thrift="yes"
        ac_thrift_path=""
    else
        want_thrift="yes"
        ac_thrift_path="$withval"
    fi
    ],
    [want_thrift="yes"])


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

if test "x$want_thrift" = "xyes"; then
    thrift_lib_version_req=ifelse([$1], ,1.20.0,$1)
    thrift_lib_version_req_shorten=`expr $thrift_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
    thrift_lib_version_req_major=`expr $thrift_lib_version_req : '\([[0-9]]*\)'`
    thrift_lib_version_req_minor=`expr $thrift_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    thrift_lib_version_req_sub_minor=`expr $thrift_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$thrift_lib_version_req_sub_minor" = "x" ; then
        thrift_lib_version_req_sub_minor="0"
        fi
    WANT_THRIFT_VERSION=`expr $thrift_lib_version_req_major \* 100000 \+  $thrift_lib_version_req_minor \* 100 \+ $thrift_lib_version_req_sub_minor`
    AC_MSG_CHECKING(for thriftlib >= $thrift_lib_version_req)
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

    dnl first we check the system location for thrift libraries
    dnl this location ist chosen if thrift libraries are installed with the --layout=system option
    dnl or if you install thrift with RPM
    if test "$ac_thrift_path" != ""; then
        THRIFT_CPPFLAGS="-I$ac_thrift_path/include"
        for ac_thrift_path_tmp in $libsubdirs; do
                if test -d "$ac_thrift_path"/"$ac_thrift_path_tmp" ; then
                        THRIFT_LDFLAGS="-L$ac_thrift_path/$ac_thrift_path_tmp"
                        break
                fi
        done
    elif test "$cross_compiling" != yes; then
        for ac_thrift_path_tmp in /usr /usr/local /opt /opt/local ; do
            if test -d "$ac_thrift_path_tmp/include/thrift" && test -r "$ac_thrift_path_tmp/include/thrift"; then
                for libsubdir in $libsubdirs ; do
                    if ls "$ac_thrift_path_tmp/$libsubdir/libthrift_"* >/dev/null 2>&1 ; then break; fi
                done
                THRIFT_LDFLAGS="-L$ac_thrift_path_tmp/$libsubdir"
                THRIFT_CPPFLAGS="-I$ac_thrift_path_tmp/include"
                break;
            fi
        done
    fi

    dnl overwrite ld flags if we have required special directory with
    dnl --with-thrift-libdir parameter
    if test "$ac_thrift_lib_path" != ""; then
       THRIFT_LDFLAGS="-L$ac_thrift_lib_path"
    fi

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $THRIFT_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $THRIFT_LDFLAGS"
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



    dnl if we found no thrift with system layout we search for thrift libraries
    dnl built and installed without the --layout=system option or for a staged(not installed) version
    if test "x$succeeded" != "xyes"; then
        _version=0
        if test "$ac_thrift_path" != ""; then
            if test -d "$ac_thrift_path" && test -r "$ac_thrift_path"; then
                for i in `ls -d $ac_thrift_path/include/thrift-* 2>/dev/null`; do
                    _version_tmp=`echo $i | sed "s#$ac_thrift_path##" | sed 's/\/include\/thrift-//' | sed 's/_/./'`
                    V_CHECK=`expr $_version_tmp \> $_version`
                    if test "$V_CHECK" = "1" ; then
                        _version=$_version_tmp
                    fi
                    VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                    THRIFT_CPPFLAGS="-I$ac_thrift_path/include/thrift-$VERSION_UNDERSCORE"
                done
            fi
        else
            if test "$cross_compiling" != yes; then
                for ac_thrift_path in /usr /usr/local /opt /opt/local ; do
                    if test -d "$ac_thrift_path" && test -r "$ac_thrift_path"; then
                        for i in `ls -d $ac_thrift_path/include/thrift-* 2>/dev/null`; do
                            _version_tmp=`echo $i | sed "s#$ac_thrift_path##" | sed 's/\/include\/thrift-//' | sed 's/_/./'`
                            V_CHECK=`expr $_version_tmp \> $_version`
                            if test "$V_CHECK" = "1" ; then
                                _version=$_version_tmp
                                best_path=$ac_thrift_path
                            fi
                        done
                    fi
                done

                VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                THRIFT_CPPFLAGS="-I$best_path/include/thrift-$VERSION_UNDERSCORE"
                if test "$ac_thrift_lib_path" = ""; then
                    for libsubdir in $libsubdirs ; do
                        if ls "$best_path/$libsubdir/libthrift_"* >/dev/null 2>&1 ; then break; fi
                    done
                    THRIFT_LDFLAGS="-L$best_path/$libsubdir"
                fi
            fi

            if test "x$THRIFT_ROOT" != "x"; then
                for libsubdir in $libsubdirs ; do
                    if ls "$THRIFT_ROOT/stage/$libsubdir/libthrift_"* >/dev/null 2>&1 ; then break; fi
                done
                if test -d "$THRIFT_ROOT" && test -r "$THRIFT_ROOT" && test -d "$THRIFT_ROOT/stage/$libsubdir" && test -r "$THRIFT_ROOT/stage/$libsubdir"; then
                    version_dir=`expr //$THRIFT_ROOT : '.*/\(.*\)'`
                    stage_version=`echo $version_dir | sed 's/thrift_//' | sed 's/_/./g'`
                        stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
                    V_CHECK=`expr $stage_version_shorten \>\= $_version`
                    if test "$V_CHECK" = "1" -a "$ac_thrift_lib_path" = "" ; then
                        AC_MSG_NOTICE(We will use a staged thrift library from $THRIFT_ROOT)
                        THRIFT_CPPFLAGS="-I$THRIFT_ROOT"
                        THRIFT_LDFLAGS="-L$THRIFT_ROOT/stage/$libsubdir"
                    fi
                fi
            fi
        fi

        CPPFLAGS="$CPPFLAGS $THRIFT_CPPFLAGS"
        export CPPFLAGS
        LDFLAGS="$LDFLAGS $THRIFT_LDFLAGS"
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
            AC_MSG_NOTICE([[We could not detect the thrift libraries (version $thrift_lib_version_req_shorten or higher). If you have a staged thrift library (still not installed) please specify \$THRIFT_ROOT in your environment and do not give a PATH to --with-thrift option.  If you are sure you have thrift installed, then check your version number looking in <thrift/version.hpp>. See http://randspringer.de/thrift for more documentation.]])
        else
            AC_MSG_NOTICE([Your thrift libraries seems to old (version $_version).])
        fi
        # execute ACTION-IF-NOT-FOUND (if present):
        ifelse([$3], , :, [$3])
    else
        AC_SUBST(THRIFT_CPPFLAGS)
        AC_SUBST(THRIFT_LDFLAGS)
        AC_DEFINE(HAVE_THRIFT,,[define if the thrift library is available])
        # execute ACTION-IF-FOUND (if present):
        ifelse([$2], , :, [$2])
    fi

    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
fi

])
