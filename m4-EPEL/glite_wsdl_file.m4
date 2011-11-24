dnl
dnl Helper macro: capitalize the first letter of a string
dnl
m4_define([glite_wsdl_ucfirst], [m4_translit(m4_substr([$1], [0], [1]), [a-z], [A-Z])[]m4_substr([$1], [1])])dnl

dnl
dnl Helper macro: compute the Makefile variable name for GLITE_WSDL_FILE
dnl
m4_define([glite_wsdl_varname], [m4_toupper([$1])[_WSDL_FILE]])dnl

dnl
dnl Determine the location of a WSDL file.
dnl
dnl Usage: GLITE_WSDL_FILE([subsystem], [interface])
dnl	subsystem: gLite subsystem name (e.g. data)
dnl	interface: interface name (e.g. fireman)
dnl
AC_DEFUN([GLITE_WSDL_FILE], [
	AC_REQUIRE([AC_GLITE_VERSION])

	AC_ARG_WITH([$2-wsdl],
		AC_HELP_STRING([--with-$2-wsdl=FILE], [Location of the glite_wsdl_ucfirst([$2]) WSDL file]),
		[glite_wsdl_varname([$2])="$withval"],
		[glite_wsdl_varname([$2])="${GLITE_LOCATION}/interface/org.glite.$1-$2-${INTERFACE_VERSION}.wsdl"])
	if test ! -f "$glite_wsdl_varname([$2])"; then
		AC_MSG_ERROR([The glite_wsdl_ucfirst([$2]) WSDL file $glite_wsdl_varname([$2]) was not found.])
	fi
	AC_SUBST(glite_wsdl_varname([$2]))
	AC_MSG_NOTICE([glite_wsdl_ucfirst([$2]) WSDL file is $glite_wsdl_varname([$2])])
])
