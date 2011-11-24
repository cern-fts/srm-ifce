dnl
dnl GLITE_DOCBOOK_HTML
dnl
dnl Check for xsltproc and the HTML stylesheets
dnl
AC_DEFUN([GLITE_DOCBOOK_HTML], [
	AC_PATH_PROG([XSLTPROC], [xsltproc], [no])
	if test "$XSLTPROC" != no; then
		if test -z "$XLSTPROCFLAGS"; then
			XSLTPROCFLAGS="--nonet"
		fi
		AC_CACHE_CHECK([for DocBook XML HTML stylesheets], [glite_cv_docbook_html],
		[
			cat >conftest.xml <<"EOF"
<?xml version="1.0"?>
<!DOCTYPE article PUBLIC "-//OASIS//DTD DocBook XML V4.4//EN"
          "http://www.oasis-open.org/docbook/xml/4.4/docbookx.dtd">
<article>
<sect1><title>test</title></sect1>
<para>a</para>
</article>
EOF
			$XSLTPROC $XSLTPROCFLAGS http://docbook.sourceforge.net/release/xsl/current/xhtml/docbook.xsl conftest.xml >/dev/null 2>/dev/null
			result=$?
			if test $result = 0; then
				glite_cv_docbook_html=yes
			else
				glite_cv_docbook_html=no
			fi
		])
		AC_SUBST([XSLTPROCFLAGS])
	fi
	AM_CONDITIONAL([HAVE_DOCBOOK_HTML], [test "$glite_cv_docbook_html" = yes])
])
