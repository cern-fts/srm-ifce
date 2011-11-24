dnl
dnl GLITE_MYSQL_DEVEL
dnl
dnl Usage:
dnl GLITE_MYSQL_DEVEL(MINIMUM-VERSION, MAXIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl To be used instead of AC_MYSQL if using only mysql-devel package
dnl 
AC_DEFUN([mysql_devel_version_between],
[
	l=0; for i in `echo $1 | tr . ' '`; do l=`expr $i + 1000 \* $l`; done
	m=0; for i in `echo $2 | tr . ' '`; do m=`expr $i + 1000 \* $m`; done
	r=0; for i in `echo $3 | tr . ' '`; do r=`expr $i + 1000 \* $r`; done
	if test $l -le $m -a $m -le $r; then [ $4 ]; else [ $5 ]; fi
])

AC_DEFUN([GLITE_MYSQL_DEVEL],
[
	AC_ARG_WITH([mysql-devel-prefix],
    	[AS_HELP_STRING([--with-mysql-devel-prefix=PFX],[prefix where MySQL-devel is installed.])],
	    [mysql_devel_prefix=${withval}],
		[mysql_devel_prefix=${MYSQL_INSTALL_PATH:-/usr}])
	AC_MSG_RESULT([mysql_devel_prefix is $mysql_devel_prefix])
	
	version_h=${mysql_devel_prefix}/include/mysql/mysql_version.h
	
    mysql_greater_than_3=no
    result=no
	ac_cv_mysql_valid=no

	AC_MSG_RESULT([Checking if ${version_h} exists])
	if test -f ${version_h}; then
		MYSQL_VERSION=`grep MYSQL_SERVER_VERSION ${version_h} | sed 's/.*"\(.*\)"/\1/'`
        mysql_devel_version_between(3.0.0,$MYSQL_VERSION,3.999.999,
            mysql_greater_than_3=no,mysql_greater_than_3=yes)
		mysql_devel_version_between($1,$MYSQL_VERSION,$2,ac_cv_mysql_valid=yes,:)
	else
		AC_MSG_RESULT([Could not find mysql-devel header file $(version_h)!])
	fi

    AC_MSG_RESULT([mysql-devel *required* version between $1 and $2])
    AC_MSG_RESULT([mysql-devel *found* version: $MYSQL_VERSION])
    if test "x$ac_cv_mysql_valid" = "xno" ; then
        AC_MSG_RESULT([mysql-devel: **** suitable version NOT FOUND])
    else
        AC_MSG_RESULT([mysql-devel: **** suitable version FOUND])
    fi

    if test -n "$with_mysql_devel_prefix" -a "$with_mysql_devel_prefix" != "/usr" -a "x$ac_cv_mysql_valid" = "xyes" ; then
		MYSQL_CFLAGS="-I$with_mysql_devel_prefix/include -I$with_mysql_devel_prefix/include/mysql"
        if test "x$HOSTTYPE" = "xx86_64"; then
            MYSQL_LIBS="-L$with_mysql_devel_prefix/lib64 -L$with_mysql_devel_prefix/lib64/mysql"
        else
	        MYSQL_LIBS="-L$with_mysql_devel_prefix/lib -L$with_mysql_devel_prefix/lib/mysql"
        fi
        if test "x$mysql_greater_than_3" = "xyes" ; then
            MYSQL_LIBS="$MYSQL_LIBS -lmysqlclient -lz"
        else
            MYSQL_LIBS="$MYSQL_LIBS -lmysqlclient"
        fi
    else
        MYSQL_CFLAGS=""
        MYSQL_LIBS=""
    fi
	
	AC_MSG_RESULT([MYSQL_CFLAGS="${MYSQL_CFLAGS}"])
	AC_MSG_RESULT([MYSQL_LIBS="${MYSQL_LIBS}"])

    if test "x$ac_cv_mysql_valid" = "xyes" ; then
		ifelse([$3], , :, [$3])
    else
		ifelse([$4], , :, [$4])
    fi
	
    AC_SUBST(MYSQL_CFLAGS)
    AC_SUBST(MYSQL_LIBS)
])
