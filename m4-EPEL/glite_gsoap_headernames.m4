dnl
dnl Determine the name of the SOAP proxy and object headers
dnl
dnl Usage: GLITE_GSOAP_HEADERNAMES([namespace], [classname])
dnl	namespace: the SOAP namespace (e.g. fireman)
dnl	classname: name of the interface (e.g. FiremanCatalog)
dnl
AC_DEFUN([GLITE_GSOAP_HEADERNAMES], [
	AC_REQUIRE([AC_GSOAP])

	if test "$GSOAP_WSDL2H_VERSION_NUM" -lt "020600"; then
		m4_toupper([$1])[_PROXY_H]=[$1]soap[$2]ServiceProxy.h
		m4_toupper([$1])[_OBJECT_H]=[$1]soap[$2]ServiceObject.h
	else
		m4_toupper([$1])[_PROXY_H]=[$1][$2]SoapBindingProxy.h
		m4_toupper([$1])[_OBJECT_H]=[$1][$2]SoapBindingObject.h
	fi

	AC_SUBST(m4_toupper([$1])[_PROXY_H])
	AC_SUBST(m4_toupper([$1])[_OBJECT_H])
])
