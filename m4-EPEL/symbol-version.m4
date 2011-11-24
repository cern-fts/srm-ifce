dnl
dnl Helper macro for building shared libraries with versioned symbols
dnl

AC_DEFUN([GLITE_SYMBOL_VERSION],
[
	dnl AC_PROG_LD_GNU is defined by libtool
	AC_REQUIRE([AC_PROG_LD_GNU])dnl

	SYMVER_LDFLAGS=
	AC_MSG_CHECKING([version script options])
	if test "$with_gnu_ld" = yes; then
		dnl $@ means the arguments of the autoconf macro, so we cannot
		dnl insert it literally
		SYMVER_LDFLAGS='-Wl,-O1 -Wl,--version-script,$(basename @S|@@).ver'
	fi
	AC_MSG_RESULT([$SYMVER_LDFLAGS])

	AC_SUBST([SYMVER_LDFLAGS])
	AM_CONDITIONAL([SYMBOL_VERSIONING], [test "$SYMVER_LDFLAGS" != ""])
])
