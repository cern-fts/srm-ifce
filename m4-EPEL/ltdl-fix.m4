
dnl AC_EMI_LTDL([ ])
dnl define EMI_LTDL_LIB
dnl
AC_DEFUN(AC_EMI_LTDL,
[
    EMI_LTDL_LIB="-L/usr/lib64 -lltdl"
    AC_SUBST(EMI_LTDL_LIB)
])

