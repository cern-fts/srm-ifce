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
# Version info: $Id: configure.ac,v 1.12 2010/01/22 15:25:54 molnarzs Exp $
# Release: $Name:  $

AC_PREREQ(2.57)
# version 0.0.0 will be replaced by AC_GLITE_VERSION
AC_INIT([SRM access library for LCG_Util], [0.0.0])
# auxiliary build tools directory
AC_CONFIG_AUX_DIR([./project])
AC_CONFIG_SRCDIR([src/Makefile.am])
AM_INIT_AUTOMAKE

# Notices.
AC_COPYRIGHT([Copyright (c) 2010 The EU EMI Project
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

################################################################################
#                               Arguments
################################################################################

# --enable arguments
#-------------------------------------------------------------------------------
AC_ARG_ENABLE(
	[debug],
	[AS_HELP_STRING([--enable-debug],[Produce binaries with debugging symbols.])],
	[CFLAGS="$CFLAGS -g -DDEBUG -D_DEBUG -O0 -ggdb"])

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

# ccheck
AC_CCHECK

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

#Conditions for Gsoap on-the-fly paches
AC_MSG_NOTICE([GSOAP_VERSION is $GSOAP_VERSION])
AC_MSG_NOTICE([GSOAP_VERSION_NUM is $GSOAP_VERSION_NUM])

AC_MSG_NOTICE([checking gsoap version to apply on-the-fly patches])
AM_CONDITIONAL([USE_GSOAP_2_7_10],[test "x$GSOAP_VERSION_NUM" = "x020710"])

# --with arguments
#-------------------------------------------------------------------------------

# release number
AC_ARG_WITH([release],
    [AS_HELP_STRING([--with-release=VAL],[Release number])],
    [RELEASE=${withval}],
    [RELEASE=0])
AC_MSG_RESULT([RELEASE is ${RELEASE}])
AC_SUBST(RELEASE)

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

AC_OUTPUT

