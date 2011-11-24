dnl
dnl CCHECK unit testing framework
dnl 

AC_DEFUN([AC_CCHECK],
[
    AC_ARG_WITH(ccheck-location, 
	[  --with-ccheck-location=PFX     prefix where CCHECK unit testing framework is installed.],
	[], 
        with_ccheck_location=${CCHECK_LOCATION:-/opt/c-check})

	AC_MSG_RESULT([checking for ccheck... ])

	if test -n "with_ccheck_location" ; then
		CCHECK_LOCATION="$with_ccheck_location"
		CCHECK_CFLAGS="-I$with_ccheck_location/include"

        ac_ccheck_libdir='lib'
        if test `uname -s` = "Linux"
        then
          # patch linux on power, mips, itanium and intel
          for a in x86_64 powerpc ppc64 mips ia64
          do 
            if test `uname -p` = $a 
            then   
              ac_ccheck_libdir='lib64'
            fi 
    
            if test `uname -m` = $a
            then
              ac_ccheck_libdir='lib64'
            fi
          done
        fi
    
        ac_ccheck_lib="$with_ccheck_location/$ac_ccheck_libdir"
		CCHECK_LIBS="-L$ac_ccheck_lib -lcheck"
    else
		CCHECK_LOCATION=""
		CCHECK_CFLAGS=""
		CCHECK_LIBS=""
    fi

    AC_MSG_RESULT([CCHECK_LOCATION set to $CCHECK_LOCATION])
    AC_MSG_RESULT([CCHECK_CFLAGS set to $CCHECK_CFLAGS])
    AC_MSG_RESULT([CCHECK_LIBS set to $CCHECK_LIBS])

    AC_SUBST(CCHECK_LOCATION)
    AC_SUBST(CCHECK_CFLAGS)
    AC_SUBST(CCHECK_LIBS)

])

