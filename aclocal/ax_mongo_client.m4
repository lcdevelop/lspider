AC_DEFUN([AX_MONGO_CLIENT],
[
AC_ARG_WITH([mongo-client-include],
  [AS_HELP_STRING([--with-mongo-client-include=INCLUDE_DIR],
    [use mongo-client])],
        [
        if test -d "$withval"
        then
                ac_mongo_client_include_path="$withval"
        else
                AC_MSG_ERROR(--with-mongo-client-include expected directory name)
        fi
        ],
        [ac_mongo_client_include_path=""]
)


AC_ARG_WITH([mongo-client-libdir],
        AS_HELP_STRING([--with-mongo-client-libdir=LIB_DIR],
        [Force given directory for mongo-client libraries. Note that this will override library path detection, so use this parameter only if default library detection fails and you know exactly where your mongo-client libraries are located.]),
        [
        if test -d "$withval"
        then
                ac_mongo_client_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-mongo-client-libdir expected directory name)
        fi
        ],
        [ac_mongo_client_lib_path=""]
)

libsubdirs="lib64 libx32 lib lib64"

if test "$ac_mongo_client_include_path" != ""; then
    MONGO_CLIENT_CPPFLAGS="-I$ac_mongo_client_include_path"
else
    ac_mongo_client_include_path="/usr/include/mongo-client/"
    MONGO_CLIENT_CPPFLAGS="-I$ac_mongo_client_include_path"
fi

if test "$ac_mongo_client_lib_path" != ""; then
    MONGO_CLIENT_LDFLAGS="-L$ac_mongo_client_lib_path"
else
    for libsubdir in $libsubdirs ; do
        if ls "$/usr/$libsubdir/libmongoclient"* >/dev/null 2>&1 ; then break; fi
    done
    ac_mongo_client_lib_path="/usr/$libsubdir/"
    MONGO_CLIENT_LDFLAGS="-L$ac_mongo_client_lib_path"
fi

succeeded=no
if ls "$ac_mongo_client_include_path/mongo" >/dev/null 2>&1 ; then
    if ls "$ac_mongo_client_lib_path/libmongoclient"* >/dev/null 2>&1 ; then
        succeeded=yes
    fi
fi

AC_SUBST(MONGO_CLIENT_CPPFLAGS)
AC_SUBST(MONGO_CLIENT_LDFLAGS)

])
