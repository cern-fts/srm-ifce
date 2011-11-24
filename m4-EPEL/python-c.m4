## Python m4 methods


# AM_PYTHON_C([MINIMUM-VERSION])

AC_DEFUN([AM_PYTHON_C],
 [
  dnl Find a Python interpreter.  Python versions prior to 1.5 are not
  dnl supported because the default installation locations changed from
  dnl $prefix/lib/site-python in 1.4 to $prefix/lib/python1.5/site-packages
  dnl in 1.5.
  m4_define([_AM_PYTHON_INTERPRETER_LIST],
	    [python python2 python2.3 python2.4 python2.2 python2.1 python2.0 python1.6 python1.5])

  m4_if([$1],[],[
    dnl No version check is needed.
    # Find any Python interpreter.
    AC_PATH_PROG([PYTHON], _AM_PYTHON_INTERPRETER_LIST)],[
    dnl A version check is needed.
    if test -n "$PYTHON"; then
      # If the user set $PYTHON, use it and don't search something else.
      AC_MSG_CHECKING([whether $PYTHON version >= $1])
      AM_PYTHON_CHECK_VERSION([$PYTHON], [$1],
			      [AC_MSG_RESULT(yes)],
			      [AC_MSG_ERROR(too old)])
    else
      # Otherwise, try each interpreter until we find one that satisfies
      # VERSION.
      AC_CACHE_CHECK([for a Python interpreter with version >= $1],
	[am_cv_pathless_PYTHON],[
	for am_cv_pathless_PYTHON in _AM_PYTHON_INTERPRETER_LIST : ; do
          if test "$am_cv_pathless_PYTHON" = : ; then
            AC_MSG_ERROR([no suitable Python interpreter found])
	  fi
          AM_PYTHON_CHECK_VERSION([$am_cv_pathless_PYTHON], [$1], [break])
        done])
      # Set $PYTHON to the absolute path of $am_cv_pathless_PYTHON.
      AC_PATH_PROG([PYTHON], [$am_cv_pathless_PYTHON])
    fi
  ])

  dnl Query Python for its version number.  Getting [:3] seems to be
  dnl the best way to do this; it's what "site.py" does in the standard
  dnl library.

  AC_CACHE_CHECK([for $am_cv_pathless_PYTHON version], [am_cv_python_version],
    [am_cv_python_version=`$PYTHON -c "import sys; print sys.version[[:3]]"`])
  AC_SUBST([PYTHON_VERSION], [$am_cv_python_version])

  dnl Get the values of $prefix and $exec_prefix 
  AC_CACHE_CHECK([for $am_cv_pathless_PYTHON prefix], [am_cv_python_prefix],
    [am_cv_python_prefix=`$PYTHON -c "import os; import sys; print os.path.normpath(sys.prefix)"`])
  AC_SUBST([PYTHON_PREFIX], [$am_cv_python_prefix])
  AC_CACHE_CHECK([for $am_cv_pathless_PYTHON exec_prefix], [am_cv_python_exec_prefix],
    [am_cv_python_exec_prefix=`$PYTHON -c "import os; import sys; print os.path.normpath(sys.exec_prefix)"`])
  AC_SUBST([PYTHON_PREFIX], [$am_cv_python_exec_prefix])

  AC_SUBST([PYTHON_PREFIX], [$am_cv_python_prefix])
  AC_SUBST([PYTHON_EXEC_PREFIX], [$am_cv_python_exec_prefix])

  dnl At times (like when building shared libraries) you may want
  dnl to know which OS platform Python thinks this is.

  AC_CACHE_CHECK([for $am_cv_pathless_PYTHON platform],
    [am_cv_python_platform], [am_cv_python_platform=`$PYTHON -c "import sys; print sys.platform"`])
  AC_SUBST([PYTHON_PLATFORM], [$am_cv_python_platform])

  dnl Get the values of Python-C cflags
  AC_CACHE_CHECK([for $am_cv_pathless_PYTHON include], [am_cv_python_include],
    [am_cv_python_include=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_inc(0,prefix='$PYTHON_PREFIX')"`])
  AC_SUBST([PYTHON_C_CFLAGS], [-I${am_cv_python_include}])

  dnl Get the values of Python-C LIBS
  AC_CACHE_CHECK([for $am_cv_pathless_PYTHON lib], [am_cv_python_lib],
    [am_cv_python_lib=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_lib(0,1,prefix='$PYTHON_PREFIX')" 2>/dev/null || echo "${PYTHON_PREFIX}/lib/python${PYTHON_VERSION}/config"`])
  PYTHON_C_LIBS="-L${am_cv_python_lib}/config -lpython${PYTHON_VERSION}"
  AC_SUBST([PYTHON_C_LIBS])
])

# AM_PYTHON_CHECK_VERSION(PROG, VERSION, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ---------------------------------------------------------------------------
# Run ACTION-IF-TRUE if the Python interpreter PROG has version >= VERSION.
# Run ACTION-IF-FALSE otherwise.
# This test uses sys.hexversion instead of the string equivalant (first
# word of sys.version), in order to cope with versions such as 2.2c1.
# hexversion has been introduced in Python 1.5.2; it's probably not
# worth to support older versions (1.5.1 was released on October 31, 1998).
AC_DEFUN([AM_PYTHON_CHECK_VERSION],
 [prog="import sys, string
# split strings by '.' and convert to numeric.  Append some zeros
# because we need at least 4 digits for the hex conversion.
minver = map(int, string.split('$2', '.')) + [[0, 0, 0]]
minverhex = 0
for i in xrange(0, 4): minverhex = (minverhex << 8) + minver[[i]]
sys.exit(sys.hexversion < minverhex)"
  AS_IF([AM_RUN_LOG([$1 -c "$prog"])], [$3], [$4])])
