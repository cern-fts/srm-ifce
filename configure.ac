# Copyright (c) Members of the EGEE Collaboration. 2008.
# See http://www.eu-egee.org/partners/ for details on the copyright
# holders.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# org.glite.data.gfal configure.ac
#
# Authors: Paolo Tedesco <paolo.tedesco@cern.ch>
# Version info: $Id: configure.ac,v 1.12 2010/01/22 15:25:54 molnarzs Exp $
# Release: $Name:  $

AC_PREREQ(2.57)
# version 0.0.0 will be replaced by AC_GLITE_VERSION
AC_INIT([GLite Data Grid File Access Library], [0.0.0])
# auxiliary build tools directory
AC_CONFIG_AUX_DIR([./project])
AC_CONFIG_SRCDIR([src/Makefile.am])
AM_INIT_AUTOMAKE

# Notices.
AC_COPYRIGHT([Copyright (c) 2008 The EU EGEE Project
See LICENSE file for details
])
AC_REVISION([$Revision: 1.12 $])

# Check for the basic compile environment.
AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_LIBTOOL

# Check for the basic compile environment.
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# some default values
if test $(uname -m) = 'x86_64'; then
    GLOBUS_FLAVOUR_DEFAULT=gcc64dbg
    GLOBUS_FLAVOUR_PTHR_DEFAULT=gcc64dbgpthr
else
    GLOBUS_FLAVOUR_DEFAULT=gcc32dbg
    GLOBUS_FLAVOUR_PTHR_DEFAULT=gcc32dbgpthr
fi

################################################################################
#                               Arguments
################################################################################

# --enable arguments
#-------------------------------------------------------------------------------
AC_ARG_ENABLE(
	[debug],
	[AS_HELP_STRING([--enable-debug],[Produce binaries with debugging symbols.])],
	[CFLAGS="$CFLAGS -g -DDEBUG -D_DEBUG -O0 -ggdb -L/usr/local/lib -lcheck"])

AC_ARG_ENABLE(
	[wall],
	[AS_HELP_STRING([--enable-wall],[Compile with all warnings enabled.])],
	[CFLAGS="$CFLAGS -Wall"])

# Set GLite Properties
AC_GLITE
AC_GLITE_VERSION

# Overwrite Interface version
## Split the version code
INTERFACE_MAJOR=`echo $VERSION | cut -d. -f1`
INTERFACE_MINOR=`echo $VERSION | cut -d. -f2`
## we don't care about last number, no binary incompatibilities
INTERFACE_PATCHLEVEL=0

## Calculate the libtool version fields
INTERFACE_LIBTOOL_CURRENT=`expr $INTERFACE_MAJOR + $INTERFACE_MINOR`
INTERFACE_LIBTOOL_REVISION=$INTERFACE_PATCHLEVEL
INTERFACE_LIBTOOL_AGE=$INTERFACE_MINOR

## Export these variables to Automake
AC_SUBST([INTERFACE_LIBTOOL_CURRENT])
AC_SUBST([INTERFACE_LIBTOOL_REVISION])
AC_SUBST([INTERFACE_LIBTOOL_AGE])

# globus 
AC_GLOBUS

# gsoap
AC_GSOAP

# Set GSOAP Scripts names
AC_GSOAP_FIX

# Set CGSI GSOAP Plugin Properties
AC_CGSI_GSOAP

# Set template config.h (required by stdsoap)
# This part should probably go in a macron in gsoap.m4 (something like AC_GSOAP_SRC)
# BEGIN:
AH_TEMPLATE(HAVE_FTIME)
AH_TEMPLATE(HAVE_GMTIME)
AH_TEMPLATE(HAVE_GMTIME_R)
AH_TEMPLATE(HAVE_GETTIMEOFDAY)
AH_TEMPLATE(HAVE_LOCALTIME_R)
AH_TEMPLATE(HAVE_MBTOWC)
AH_TEMPLATE(HAVE_STRTOF)
AH_TEMPLATE(HAVE_STRTOD)
AH_TEMPLATE(HAVE_STRTOL)
AH_TEMPLATE(HAVE_STRTOLL)
AH_TEMPLATE(HAVE_STRTOUL)
AH_TEMPLATE(HAVE_STRTOULL)
AH_TEMPLATE(HAVE_SSCANF)
AH_TEMPLATE(HAVE_WCTOMB)
AH_TEMPLATE(HAVE_TIMEGM)
                                                                                                                                                         
AC_CHECK_FUNC(ftime,           [AC_DEFINE(HAVE_FTIME)])
AC_CHECK_FUNC(gmtime,          [AC_DEFINE(HAVE_GMTIME)])
AC_CHECK_FUNC(gmtime_r,        [AC_DEFINE(HAVE_GMTIME_R)])
AC_CHECK_FUNC(gettimeofday,    [AC_DEFINE(HAVE_GETTIMEOFDAY)])
AC_CHECK_FUNC(localtime_r,     [AC_DEFINE(HAVE_LOCALTIME_R)])
AC_CHECK_FUNC(mbtowc,          [AC_DEFINE(HAVE_MBTOWC)])
AC_CHECK_FUNC(strtof,          [AC_DEFINE(HAVE_STRTOF)])
AC_CHECK_FUNC(strtod,          [AC_DEFINE(HAVE_STRTOD)])
AC_CHECK_FUNC(strtol,          [AC_DEFINE(HAVE_STRTOL)])
AC_CHECK_FUNC(strtoll,         [AC_DEFINE(HAVE_STRTOLL)])
AC_CHECK_FUNC(strtoul,         [AC_DEFINE(HAVE_STRTOUL)])
AC_CHECK_FUNC(strtoull,        [AC_DEFINE(HAVE_STRTOULL)])
AC_CHECK_FUNC(sscanf,          [AC_DEFINE(HAVE_SSCANF)])
AC_CHECK_FUNC(wctomb,          [AC_DEFINE(HAVE_WCTOMB)])
AC_CHECK_FUNC(timegm,          [AC_DEFINE(HAVE_TIMEGM)])
#:END

# For IPv6
AH_TEMPLATE([WITH_IPV6], [For Ipv6 support])
AC_DEFINE(WITH_IPV6)

#Conditions for Gsoap on-the-fly paches
AM_CONDITIONAL(USE_GSOAP_2_7_9, [test "x$GSOAP_VERSION_NUM" = "x020709"])
AC_MSG_RESULT(["USE_GSOAP_2_7_9 is [$USE_GSOAP_2_7_9]"])

# --with arguments
#-------------------------------------------------------------------------------

# release number
AC_ARG_WITH([release],
    [AS_HELP_STRING([--with-release=VAL],[Release number])],
    [RELEASE=${withval}],
    [RELEASE=0])
AC_MSG_RESULT([RELEASE is ${RELEASE}])
AC_SUBST(RELEASE)

# lfc-location
AC_ARG_WITH([lfc-location],
    [AS_HELP_STRING([--with-lfc-location=VAL],[LFC location])],
    [LFC_LOCATION=${withval}],
    [LFC_LOCATION="/opt/lcg"])
AC_MSG_RESULT([LFC_LOCATION is ${LFC_LOCATION}])
AC_SUBST(LFC_LOCATION)

# dpm-location
AC_ARG_WITH([dpm-location],
    [AS_HELP_STRING([--with-dpm-location=VAL],[DPM location])],
    [DPM_LOCATION=${withval}],
    [DPM_LOCATION="/opt/lcg"])
AC_MSG_RESULT([DPM_LOCATION is ${DPM_LOCATION}])
AC_SUBST(DPM_LOCATION)

# dcap-location
AC_ARG_WITH([dcap-location],
    [AS_HELP_STRING([--with-dcap-location=VAL],[DCAP location])],
    [DCAP_LOCATION=${withval}],
    [DCAP_LOCATION="/opt/lcg"])
AC_MSG_RESULT([DCAP_LOCATION is ${DCAP_LOCATION}])
AC_SUBST(DCAP_LOCATION)

# voms-location
AC_ARG_WITH([voms-location],
    [AS_HELP_STRING([--with-voms-location=VAL],[VOMS location])],
    [VOMS_LOCATION=${withval}],
    [VOMS_LOCATION="/opt/lcg"])
AC_MSG_RESULT([VOMS_LOCATION is ${VOMS_LOCATION}])
VOMS_CFLAGS="-I${VOMS_LOCATION}/include -I${GLOBUS_LOCATION}/include/${GLOBUS_FLAVOUR}"
AC_MSG_RESULT([VOMS_CFLAGS is ${VOMS_CFLAGS}])
VOMS_THR_LIBS="-L${VOMS_LOCATION}/lib -L${VOMS_LOCATION}/lib64 -lvomsc_${GLOBUS_THR_FLAVOR}"
VOMS_NOTHR_LIBS="-L${VOMS_LOCATION}/lib -L${VOMS_LOCATION}/lib64 -lvomsc_${GLOBUS_NOTHR_FLAVOR}"
AC_MSG_RESULT([VOMS_THR_LIBS is ${VOMS_THR_LIBS}])
AC_MSG_RESULT([VOMS_NOTHR_LIBS is ${VOMS_NOTHR_LIBS}])
AC_SUBST(VOMS_LOCATION)
AC_SUBST(VOMS_CFLAGS)
AC_SUBST(VOMS_THR_LIBS)
AC_SUBST(VOMS_NOTHR_LIBS)

################################################################################

# Python location for optional Python 2.5 support
AC_ARG_WITH([python-location],
    [AS_HELP_STRING([--with-python-location=VAL],[Python location])],
    [PYTHON_LOCATION=${withval}],
    [PYTHON_LOCATION="/usr"])
AC_MSG_RESULT([PYTHON_LOCATION is ${PYTHON_LOCATION}])
AC_SUBST(PYTHON_LOCATION)

# The optional Python location may not be in the PATH 
# and LD_LIBRARY_PATH, so it has to be added that the
# checking macro succeeds.
ac_saved_path="$PATH"
ac_saved_ld_library_path="$LD_LIBRARY_PATH"
PATH="$PYTHON_LOCATION/bin:$PATH"
LD_LIBRARY_PATH="$PYTHON_LOCATION/lib"
export PATH LD_LIBRARY_PATH
AM_PATH_PYTHON([2.3])

# PYTHON_CFLAGS and PYTHON_LIBS in the original
# python.m4 macro are based on the ${prefix} and
# ${execprefix} automake variables. However the
# optional $PYTHON_LOCATION may point to a 
# different place, so these variables are re-evaluated.

dnl Get the values of Python CFLAGS
AC_CACHE_CHECK([for $am_cv_pathless_PYTHON include], [am_cv_python_include],
[am_cv_python_include=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_inc(0,prefix='$PYTHON_LOCATION')"`])
AC_SUBST([PYTHON_CFLAGS], ["-I${am_cv_python_include}"])

dnl Get the values of Python LIBS
AC_CACHE_CHECK([for $am_cv_pathless_PYTHON lib], [am_cv_python_lib],
[am_cv_python_lib=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_lib(0,1,prefix='$PYTHON_LOCATION')" 2>/dev/null || echo "${PYTHON_LOCATION}/lib/python${PYTHON_VERSION}/config"`])
AC_SUBST([PYTHON_LIBS], ["-L${am_cv_python_lib}/config -lpython${PYTHON_VERSION}"])

PATH="$ac_saved_path"
LD_LIBRARY_PATH="$ac_saved_ld_library_path"

AC_MSG_RESULT([PYTHON_LIBS is ${PYTHON_LIBS}])
AC_MSG_RESULT([PYTHON_CFLAGS is ${PYTHON_CFLAGS}])

# swig
AC_SWIG([1.3.0])
# we also need to set SWIG_LIB, because the one built into 
# the 'swig' binary will not be valid in a userspace build
for SWIG_LIB_PATH in \
    ${SWIG_INSTALL_PATH}/lib/swig${swig_major_ver}.${swig_minor_ver} \
    ${SWIG_INSTALL_PATH}/lib/swig${swig_major_ver}.${swig_minor_ver}.${swig_micro_ver} \
    ${SWIG_INSTALL_PATH}/lib/swig/${swig_major_ver}.${swig_minor_ver} \
    ${SWIG_INSTALL_PATH}/lib/swig/${swig_major_ver}.${swig_minor_ver}.${swig_micro_ver} \
    ${SWIG_INSTALL_PATH}/lib64/swig${swig_major_ver}.${swig_minor_ver} \
    ${SWIG_INSTALL_PATH}/lib64/swig${swig_major_ver}.${swig_minor_ver}.${swig_micro_ver} \
    ${SWIG_INSTALL_PATH}/lib64/swig/${swig_major_ver}.${swig_minor_ver} \
    ${SWIG_INSTALL_PATH}/lib64/swig/${swig_major_ver}.${swig_minor_ver}.${swig_micro_ver} \
    ${SWIG_INSTALL_PATH}/share/swig${swig_major_ver}.${swig_minor_ver} \
    ${SWIG_INSTALL_PATH}/share/swig${swig_major_ver}.${swig_minor_ver}.${swig_micro_ver} \
    ${SWIG_INSTALL_PATH}/share/swig/${swig_major_ver}.${swig_minor_ver} \
    ${SWIG_INSTALL_PATH}/share/swig/${swig_major_ver}.${swig_minor_ver}.${swig_micro_ver} \
    failed; do
    if test -d ${SWIG_LIB_PATH}; then break; fi
done
if test "${SWIG_LIB_PATH}" = 'failed'; then
    AC_MSG_RESULT([Could not find swig library!])
else
    AC_MSG_RESULT([swig library path: $SWIG_LIB_PATH])
    AC_SUBST(SWIG_LIB_PATH)
fi


# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([memmove memset mkdir putenv rmdir strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol])

#Conditions for Gsoap on-the-fly paches
AC_MSG_NOTICE([GSOAP_VERSION is $GSOAP_VERSION])
AC_MSG_NOTICE([GSOAP_VERSION_NUM is $GSOAP_VERSION_NUM])

AC_MSG_NOTICE([checking gsoap version to apply on-the-fly patches])
AM_CONDITIONAL([USE_GSOAP_2_7_10],[test "x$GSOAP_VERSION_NUM" = "x020710"])

AC_CONFIG_HEADERS([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
#AC_CONFIG_FILES([test/Makefile])
#AC_CONFIG_FILES([test/regression/Makefile])
#AC_CONFIG_FILES([test/regression/52502_mkdir_dpm_se/Makefile])
#AC_CONFIG_FILES([test/regression/62445_gfal_open_twice_fails/Makefile])

AC_OUTPUT

