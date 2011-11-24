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
Requires: srm-ifce-libs
%description
SRM client side implementation for GFAL and FTS compatible srmv1 and srmv2


%package libs
Summary: srm-ifce shared libraries
Group: grid/lcg
BuildRequires: automake, gsoap, gsoap-devel, CGSI-gSOAP-devel,  libtool, globus-common-progs, globus-gssapi-gsi-devel, globus-gss-assist-devel, globus-ftp-client-devel
AutoReqProv: no
Requires: gsoap, CGSI-gSOAP, CGSI_gSOAP_2.7, globus-gssapi-gsi, globus-gss-assist, globus-ftp-client
%description libs
srm-ifce is a client side implementation of \
the SRMv1 and SRMv2 specification for GFAL and FTS.
SRM means Storage Resource Manager Interface, \
It is a specification of a SOAP interface providing a \
generic way to manage distributed storage systems .

%package devel
Summary: srm-ifce headers and static libraries
Group: grid/lcg
BuildRequires: automake, gsoap, gsoap-devel, CGSI-gSOAP-devel,  libtool, globus-common-progs, globus-gssapi-gsi-devel, globus-gss-assist-devel, globus-ftp-client-devel
AutoReqProv: no
Requires: srm-ifce
%description devel
headers and static library for srm-ifce





%post libs
/sbin/ldconfig

%postun libs
/sbin/ldconfig

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
../configure --libdir=%{_libdir} --prefix=/usr/ --with-version=%{version} --with-release=%{release} --with-emi --enable-tests=no
cd .. ;

%build
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
make -C build all



%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT"; 
NUMCPU=`grep processor /proc/cpuinfo | wc -l`; if [[ "$NUMCPU" == "0" ]]; then NUMCPU=1; fi;
make -C build -j $NUMCPU install DESTDIR="$RPM_BUILD_ROOT"


%files libs
%defattr (-,root,root)
/usr/%{_lib}/libgfal_srm_ifce.so.*
/usr/bin/gfal_srm_ifce_version


%files devel
%defattr (-,root,root)
/usr/%{_lib}/libgfal_srm_ifce.so
/usr/%{_lib}/libgfal_srm_ifce.a
/usr/%{_lib}/libgfal_srm_ifce.la
/usr/include/gfal_srm_ifce.h
/usr/include/gfal_srm_ifce_types.h

 
%files
/usr/share/doc/srm-access-library-for-lcg_util/LICENSE
/usr/share/doc/srm-access-library-for-lcg_util/RELEASE-NOTES
/usr/share/doc/srm-access-library-for-lcg_util/VERSION

%changelog
* Mon Nov 14 2011 adevress at cern.ch 
 - Initial gfal 2.0 preview release
