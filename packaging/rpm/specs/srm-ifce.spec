%define projectname srm-ifce
%define version 1.19
%define release 1_beta_epel


%define debug_package %{nil}

Name: %{projectname}
License: Apache-2.0
Summary: SRM client side layer
Version: %{version}
Release: %{release}
Group: Grid/lcg
BuildRoot: %{_tmppath}/%{projectname}-%{version}-%{release}
Source: %{projectname}-%{version}-%{release}.src.tar.gz
%description
SRM client side implementation for GFAL and FTS compatible srmv1 and srmv2


%package libs
Summary: srm-ifce shared libraries
Group: grid/lcg
BuildRequires: automake, gsoap, gsoap-devel, CGSI-gSOAP-devel,  libtool, globus-common-progs, globus-gssapi-gsi-devel, globus-gss-assist-devel, globus-ftp-client-devel
AutoReqProv: no
Requires: gsoap, CGSI-gSOAP, CGSI_gSOAP_2.7, globus-gssapi-gsi, globus-gss-assist, globus-ftp-client
%description libs
main library of the SRM implementation for GFAL and FTS

%package devel
Summary: srm-ifce headers and static libraries
Group: grid/lcg
BuildRequires: automake, gsoap, gsoap-devel, CGSI-gSOAP-devel,  libtool, globus-common-progs, globus-gssapi-gsi-devel, globus-gss-assist-devel, globus-ftp-client-devel
AutoReqProv: no
Requires: srm-ifce
%description devel
headers and static library for srm-ifce

%package tests
Summary: srm-ifce unit tests and functionals tests
Group: grid/lcg
BuildRequires: automake, gsoap, gsoap-devel, CGSI-gSOAP-devel,  libtool, globus-common-progs, globus-gssapi-gsi-devel, globus-gss-assist-devel, globus-ftp-client-devel
AutoReqProv: no
Requires: srm-ifce
%description tests
srm-ifce unit tests and functionals tests




%post
ldconfig

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT";
rm -rf build/

%prep
%setup -q
mkdir -p src/autogen build; 
aclocal -I m4-EPEL/ ; 
libtoolize --force; 
autoheader; 
automake --foreign --add-missing --copy;
autoconf 
cd build; 
../configure --enable-wall --prefix=/ --with-version=%{version} --with-release=%{release} --with-emi
cd .. ;

%build
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
make -C build all

%postun
ldconfig

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT"; 
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
make -C build -j $NUMCPU install DESTDIR="$RPM_BUILD_ROOT"


%files libs
/usr/%{_lib}/libgfal_srm_ifce.so
/usr/%{_lib}/libgfal_srm_ifce.so.*
/usr/share/doc/srm-access-library-for-lcg_util
/usr/share/doc/srm-access-library-for-lcg_util/LICENSE
/usr/share/doc/srm-access-library-for-lcg_util/RELEASE-NOTES
/usr/share/doc/srm-access-library-for-lcg_util/VERSION

%files devel
/usr/include/gfal_srm_ifce.h
/usr/include/gfal_srm_ifce_types.h

%files tests
/usr/bin/gfal_srm_ifce_systemtest
/usr/bin/gfal_srm_ifce_unittest
/usr/bin/gfal_srm_ifce_version
 
%files

%changelog
* Mon Nov 14 2011 adevress at cern.ch 
 - Initial gfal 2.0 preview release
