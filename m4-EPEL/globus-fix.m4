dnl Usage:
dnl AC_GLOBUS_EMI
dnl - GLOBUS_LOCATION
dnl - GLOBUS_NOTHR_FLAVOR
dnl - GLOBUS_THR_FLAVOR
dnl - GLOBUS_NOTHR_CFLAGS
dnl - GLOBUS_THR_CFLAGS
dnl - GLOBUS_NOTHR_LIBS
dnl - GLOBUS_THR_LIBS
dnl - GLOBUS_COMMON_NOTHR_LIBS
dnl - GLOBUS_COMMON_THR_LIBS
dnl - GLOBUS_STATIC_COMMON_NOTHR_LIBS
dnl - GLOBUS_STATIC_COMMON_THR_LIBS
dnl - GLOBUS_FTP_CLIENT_NOTHR_LIBS
dnl - GLOBUS_FTP_CLIENT_THR_LIBS
dnl - GLOBUS_SSL_NOTHR_LIBS
dnl - GLOBUS_SSL_THR_LIBS
dnl - GLOBUS_STATIC_SSL_NOTHR_LIBS
dnl - GLOBUS_STATIC_SSL_THR_LIBS
dnl - GLOBUS_GSS_NOTHR_LIBS
dnl - GLOBUS_GSS_THR_LIBS
dnl - GLOBUS_LDAP_THR_LIBS

AC_DEFUN([AC_GLOBUS_EMI],
[

     with_globus_prefix=${GLOBUS_LOCATION:-/usr}

    AC_ARG_WITH(globus_nothr_flavor,
    [  --with-globus-nothr-flavor=flavor [default=gcc32dbg]],
    [],
        with_globus_nothr_flavor="")

    AC_MSG_RESULT(["GLOBUS nothread flavor is $with_globus_nothr_flavor"])

    AC_ARG_WITH(globus_thr_flavor,
        [  --with-globus-thr-flavor=flavor [default=gcc32dbgpthr]],
        [],
        with_globus_thr_flavor="")

    AC_MSG_RESULT(["GLOBUS thread flavor is $with_globus_thr_flavor"])

    ac_cv_globus_nothr_valid=no
    ac_cv_globus_thr_valid1=no
    ac_cv_globus_thr_valid2=no

    GLOBUS_NOTHR_CFLAGS="-I$with_globus_prefix/include"
    GLOBUS_THR_CFLAGS="-I$with_globus_prefix/include"

    ac_globus_ldlib="-L/usr/lib64"

    GLOBUS_COMMON_NOTHR_LIBS="$ac_globus_ldlib -lglobus_common"
    GLOBUS_COMMON_THR_LIBS="$ac_globus_ldlib -lglobus_common"

    GLOBUS_STATIC_COMMON_NOTHR_LIBS="$with_globus_prefix/lib/libglobus_common.a"
    GLOBUS_STATIC_COMMON_THR_LIBS="$with_globus_prefix/lib/libglobus_common.a"

    GLOBUS_FTP_CLIENT_NOTHR_LIBS="$ac_globus_ldlib -lglobus_ftp_client"
    GLOBUS_FTP_CLIENT_THR_LIBS="$ac_globus_ldlib -lglobus_ftp_client"

    GLOBUS_GSS_NOTHR_LIBS="$ac_globus_ldlib -lglobus_gssapi_gsi -lglobus_gss_assist"
    GLOBUS_GSS_THR_LIBS="$ac_globus_ldlib -lglobus_gssapi_gsi -lglobus_gss_assist"

    GLOBUS_LDAP_THR_LIBS="$ac_globus_ldlib -lldap -llber"

    dnl Needed by LCAS/LCMAPS voms plugins
    GLOBUS_GSI_NOTHR_LIBS="$ac_globus_ldlib -lglobus_gsi_credential"
    GLOBUS_GSI_THR_LIBS="$ac_globus_ldlib -lglobus_gsi_credential"

    dnl
    dnl check whether globus in place, if not return error
    dnl
    AC_MSG_CHECKING([for globus version])
    grep GLOBUS_VERSION= $with_globus_prefix/bin/globus-version | cut -d'"' -f2 >& globus.version
    ac_globus_version=`cat globus.version`
    ac_globus_point_version=`cut -d. -f3 globus.version`
    ac_globus_minor_version=`cut -d. -f2 globus.version`
    ac_globus_major_version=`cut -d. -f1 globus.version`

    if test -n "$ac_globus_point_version" ; then
        AC_MSG_RESULT([$ac_globus_version])
    else
        GLOBUS_NOTHR_CFLAGS=""
        GLOBUS_THR_CFLAGS=""
        GLOBUS_NOTHR_LIBS=""
        GLOBUS_THR_LIBS=""
        GLOBUS_COMMON_NOTHR_LIBS=""
        GLOBUS_COMMON_THR_LIBS=""
        GLOBUS_STATIC_COMMON_NOTHR_LIBS=""
        GLOBUS_STATIC_COMMON_THR_LIBS=""
        GLOBUS_FTP_CLIENT_NOTHR_LIBS=""
        GLOBUS_FTP_CLIENT_THR_LIBS=""
        GLOBUS_SSL_NOTHR_LIBS=""
        GLOBUS_SSL_THR_LIBS=""
        GLOBUS_STATIC_SSL_NOTHR_LIBS=""
        GLOBUS_STATIC_SSL_THR_LIBS=""
        GLOBUS_LDAP_THR_LIBS=""
        echo ac_point_version $ac_globus_point_version
        AC_MSG_ERROR([no])
    fi


    dnl
    dnl only perform ssl checks if globus <= 4.0.6 (include VDT-1.6.1 and 1.8.1)
    dnl
    if test [ "$ac_globus_point_version" -le 6 -a $ac_globus_minor_version == 0 -a $ac_globus_major_version == 4 ] ; then
        dnl
        dnl check nothr openssl header
        dnl
        GLOBUS_SSL_NOTHR_LIBS="$ac_globus_ldlib -lssl_$with_globus_nothr_flavor -lcrypto_$with_globus_nothr_flavor"
        GLOBUS_SSL_THR_LIBS="$ac_globus_ldlib -lssl_$with_globus_thr_flavor -lcrypto_$with_globus_thr_flavor"
        GLOBUS_STATIC_SSL_NOTHR_LIBS="$with_globus_prefix/lib/libssl_$with_globus_nothr_flavor.a $with_globus_prefix/lib/libcrypto_$with_globus_nothr_flavor.a"
        GLOBUS_STATIC_SSL_THR_LIBS="$with_globus_prefix/lib/libssl_$with_globus_thr_flavor.a $with_globus_prefix/lib/libcrypto_$with_globus_thr_flavor.a"

        ac_globus_nothr_ssl="$with_globus_prefix/include/$with_globus_nothr_flavor/openssl"
 
        AC_MSG_CHECKING([for $ac_globus_nothr_ssl/ssl.h])
 
        if test ! -f "$ac_globus_nothr_ssl/ssl.h" ; then
            ac_globus_nothr_ssl=""
            AC_MSG_RESULT([no])
        else
            AC_MSG_RESULT([yes])
        fi
    
        AC_MSG_CHECKING([for openssl nothr])
    
        if test -n "$ac_globus_nothr_ssl" ; then
            GLOBUS_NOTHR_CFLAGS="-I$ac_globus_nothr_ssl $GLOBUS_NOTHR_CFLAGS"
        fi

        if test -n "$ac_globus_nothr_ssl" ; then
            dnl
            dnl maybe do some complex test of globus instalation here later
            dnl
            ac_save_CFLAGS=$CFLAGS
            CFLAGS="$GLOBUS_NOTHR_CFLAGS $CFLAGS"
            AC_TRY_COMPILE([
                 #include "ssl.h"
                 #include "globus_gss_assist.h"
               ],
               [globus_gss_assist_ex aex],
               [ac_cv_globus_nothr_valid=yes],
               [ac_cv_globus_nothr_valid=no])
            CFLAGS=$ac_save_CFLAGS
            AC_MSG_RESULT([$ac_cv_globus_nothr_valid])
        fi

        dnl
        dnl check thr openssl header
        dnl
        ac_globus_thr_ssl="$with_globus_prefix/include/$with_globus_thr_flavor/openssl"

        AC_MSG_CHECKING([for $ac_globus_thr_ssl/ssl.h])
    
        if test ! -f "$ac_globus_thr_ssl/ssl.h" ; then
            ac_globus_thr_ssl=""
            AC_MSG_RESULT([no])
        else
            AC_MSG_RESULT([yes])
        fi
    
        if test -n "$ac_globus_thr_ssl" ; then
            GLOBUS_THR_CFLAGS="-I$ac_globus_thr_ssl $GLOBUS_THR_CFLAGS"
        fi
    
        AC_MSG_CHECKING([checking openssl thr])

        if test -n "$ac_globus_thr_ssl" ; then
            dnl
            dnl maybe do some complex test of globus instalation here later
            dnl
            ac_save_CFLAGS=$CFLAGS
            CFLAGS="$GLOBUS_THR_CFLAGS $CFLAGS"
            AC_TRY_COMPILE([
                 #include "openssl/ssl.h"
                 #include "globus_gss_assist.h"
                ],
                [globus_gss_assist_ex aex],
                [ac_cv_globus_thr_valid1=yes],
                [ac_cv_globus_thr_valid1=no])
                CFLAGS=$ac_save_CFLAGS
                AC_MSG_RESULT([$ac_cv_globus_thr_valid1])
        fi
    fi
    
    if test "x$HOSTTYPE" = "xx86_64"; then
        dnl
        dnl Temporarily remove this check on x86_64 since ldap is not available in VDT
        dnl

        ac_cv_globus_thr_valid2="yes"
        GLOBUS_LDAP_THR_LIBS=""
    else
        dnl
        dnl check thr ldap header
        dnl
        ac_globus_thr_ldap="$with_globus_prefix/include/$with_globus_thr_flavor"
                                                                                    
        AC_MSG_CHECKING([for $ac_globus_thr_ldap/lber.h])
        
        if test ! -f "$ac_globus_thr_ldap/lber.h" ; then
            ac_globus_thr_ldap=""
            AC_MSG_RESULT([no])
        else
            AC_MSG_RESULT([yes])
        fi
    
        AC_MSG_CHECKING([for ldap thr])
    
        if test -n "$ac_globus_thr_ldap" ; then
            dnl
            dnl maybe do some complex test of globus instalation here later
            dnl
            ac_save_CFLAGS=$CFLAGS
            CFLAGS="$GLOBUS_THR_CFLAGS $CFLAGS"
            AC_TRY_COMPILE([
                  #include "ldap.h"
                  #include "lber.h"
               ],
               [
               LDAPMessage *ldresult;
               BerElement *ber;
               ],
               [ac_cv_globus_thr_valid2=yes],
               [ac_cv_globus_thr_valid2=no])
            CFLAGS=$ac_save_CFLAGS
            AC_MSG_RESULT([$ac_cv_globus_thr_valid2])
        fi
    fi
    
    dnl
    dnl only perform ssl checks if globus <= 4.0.6 (include VDT-1.6.1 and 1.8.1)
    dnl
    if test [ "$ac_globus_point_version" -ge 7 -o "$ac_globus_minor_version" -ge 1 -o "$ac_globus_major_version" -ge 5 ] ; then
        GLOBUS_LOCATION=$with_globus_prefix
        GLOBUS_NOTHR_FLAVOR=$with_globus_nothr_flavor
        GLOBUS_THR_FLAVOR=$with_globus_thr_flavor
        GLOBUS_SSL_NOTHR_LIBS=""
        GLOBUS_SSL_THR_LIBS=""
        GLOBUS_STATIC_SSL_NOTHR_LIBS=""
        GLOBUS_STATIC_SSL_THR_LIBS=""
        AC_MSG_RESULT([Set globus environment variables and unset SSL variables])
        ifelse([$2], , :, [$2])
    else
        if test x$ac_cv_globus_nothr_valid = xyes -a x$ac_cv_globus_thr_valid1 = xyes -a x$ac_cv_globus_thr_valid2 = xyes ; then
            GLOBUS_LOCATION=$with_globus_prefix
            GLOBUS_NOTHR_FLAVOR=$with_globus_nothr_flavor
            GLOBUS_THR_FLAVOR=$with_globus_thr_flavor
            AC_MSG_RESULT([All 3 globus tests pass])
            ifelse([$2], , :, [$2])
        else
            GLOBUS_NOTHR_CFLAGS=""
            GLOBUS_THR_CFLAGS=""
            GLOBUS_NOTHR_LIBS=""
            GLOBUS_THR_LIBS=""
            GLOBUS_COMMON_NOTHR_LIBS=""
            GLOBUS_COMMON_THR_LIBS=""
            GLOBUS_STATIC_COMMON_NOTHR_LIBS=""
            GLOBUS_STATIC_COMMON_THR_LIBS=""
            GLOBUS_FTP_CLIENT_NOTHR_LIBS=""
            GLOBUS_FTP_CLIENT_THR_LIBS=""
            GLOBUS_SSL_NOTHR_LIBS=""
            GLOBUS_SSL_THR_LIBS=""
            GLOBUS_STATIC_SSL_NOTHR_LIBS=""
            GLOBUS_STATIC_SSL_THR_LIBS=""
            GLOBUS_LDAP_THR_LIBS=""
            AC_MSG_WARN([No all 3 globus tests pass unset variables])
            ifelse([$3], , :, [$3])
        fi
    fi
    
    AC_SUBST(GLOBUS_LOCATION)
    AC_SUBST(GLOBUS_NOTHR_FLAVOR)
    AC_SUBST(GLOBUS_THR_FLAVOR)
    AC_SUBST(GLOBUS_NOTHR_CFLAGS)
    AC_SUBST(GLOBUS_THR_CFLAGS)
    AC_SUBST(GLOBUS_NOTHR_LIBS)
    AC_SUBST(GLOBUS_THR_LIBS)
    AC_SUBST(GLOBUS_COMMON_NOTHR_LIBS)
    AC_SUBST(GLOBUS_COMMON_THR_LIBS)
    AC_SUBST(GLOBUS_STATIC_COMMON_NOTHR_LIBS)
    AC_SUBST(GLOBUS_STATIC_COMMON_THR_LIBS)
    AC_SUBST(GLOBUS_FTP_CLIENT_NOTHR_LIBS)
    AC_SUBST(GLOBUS_FTP_CLIENT_THR_LIBS)
    AC_SUBST(GLOBUS_SSL_NOTHR_LIBS)
    AC_SUBST(GLOBUS_SSL_THR_LIBS)
    AC_SUBST(GLOBUS_STATIC_SSL_NOTHR_LIBS)
    AC_SUBST(GLOBUS_STATIC_SSL_THR_LIBS)
    AC_SUBST(GLOBUS_GSS_NOTHR_LIBS)
    AC_SUBST(GLOBUS_GSS_THR_LIBS)
    AC_SUBST(GLOBUS_LDAP_THR_LIBS)
])
