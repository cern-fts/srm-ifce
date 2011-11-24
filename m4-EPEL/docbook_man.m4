dnl
dnl GLITE_DOCBOOK_MAN
dnl
dnl Check for xsltproc and the manpage stylesheets
dnl
AC_DEFUN([GLITE_DOCBOOK_MAN], [
	AC_PATH_PROG([XSLTPROC], [xsltproc], [no])
	if test "$XSLTPROC" != no; then
		if test -z "$XLSTPROCFLAGS"; then
			XSLTPROCFLAGS="--nonet"
		fi
		AC_CACHE_CHECK([for DocBook XML manpage stylesheets], [glite_cv_docbook_man],
		[
			cat >conftest.xml <<"EOF"
<?xml version="1.0"?>
	<!-- "http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd" @<:@ -->
<?xml-stylesheet href="http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl" type="text/xsl"?>
<!DOCTYPE refentry PUBLIC "-//OASIS//DTD DocBook XML V4.1.2//EN"
	"http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd" @<:@
@:>@>
<refentry id="test">
<refmeta>
    <refentrytitle>TEST</refentrytitle>
    <manvolnum>test</manvolnum>
</refmeta>
</refentry>
EOF
			$XSLTPROC $XSLTPROCFLAGS http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl conftest.xml >/dev/null 2>/dev/null
			result=$?
			if test $result = 0; then
				glite_cv_docbook_man=yes
			else
				glite_cv_docbook_man=no
			fi
		])
		AC_SUBST([XSLTPROCFLAGS])
	fi
	AM_CONDITIONAL([HAVE_DOCBOOK_MAN], [test "$glite_cv_docbook_man" = yes])
])
