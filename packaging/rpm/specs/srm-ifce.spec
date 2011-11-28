Name:		srm-ifce
Version:	1.19
Release:	1
Summary:	SRM client side library
Group:		Applications/Internet
License:	ASL 2.0
URL:		https://svnweb.cern.ch/trac/lcgutil
#
# The source of this package was pulled from upstream's vcs. Use the
# following commands to generate the tarball:
# svn export http://svn.cern.ch/guest/lcgutil/srm-ifce/branches/EPEL_trunk srm-ifce-1.19
# tar -czvf srm-ifce-1.19.tar.gz srm-ifce-1.19
Source:		%{name}-%{version}.tar.gz 
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}

BuildRequires:	automake
BuildRequires:	CGSI-gSOAP-devel%{?_isa}
BuildRequires:	gsoap-devel%{?_isa}
BuildRequires:	libtool%{?_isa}
BuildRequires:	globus-gssapi-gsi-devel%{?_isa}
BuildRequires:	globus-gss-assist-devel%{?_isa}
BuildRequires:	globus-ftp-client-devel%{?_isa}

%description
SRM client side implementation for GFAL and FTS compatible SRMv1 and SRMv2.

%package libs
Summary:	SRM client side shared libraries for FTS/gfal
Group:		System Environment/Libraries
Requires:	CGSI-gSOAP
Requires:	gsoap
Requires:	globus-gssapi-gsi
Requires:	globus-gss-assist
Requires:	globus-ftp-client

%description libs
srm-ifce is a client side implementation of the SRMv1 and SRMv2 specification 
for GFAL and FTS. SRM means Storage Resource Manager Interface, it is a 
specification of a SOAP interface providing a generic way to manage 
distributed storage systems.

%package devel
Summary:	SRM client side headers and static libraries
Group:		Development/Libraries
Requires:	srm-ifce-libs >= %{version}-%{release}

%description devel
This package contains common development libraries and header files for
the srm-ifce component.

%prep
%setup -q

%build
aclocal -I m4-EPEL;
libtoolize --force;
autoheader;
automake --foreign --add-missing --copy;
autoconf;
mkdir build;
cd build;
../configure \
	--libdir=%{_libdir} \
	--prefix=${prefix} \
	--with-version=%{version} \
	--with-release=%{release} \
	--with-emi \
	--enable-tests=no

make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make -C build DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post libs -p /sbin/ldconfig

%postun libs -p /sbin/ldconfig

%files libs
%defattr (-,root,root)
%{_bindir}/gfal_srm_ifce_version
%{_libdir}/libgfal_srm_ifce.so.*
%{_datadir}/doc/srm-access-library-for-lcg_util/LICENSE
%{_datadir}/doc/srm-access-library-for-lcg_util/RELEASE-NOTES
%{_datadir}/doc/srm-access-library-for-lcg_util/VERSION

%files devel
%defattr (-,root,root)
%{_libdir}/libgfal_srm_ifce.so
%{_includedir}/gfal_srm_ifce.h
%{_includedir}/gfal_srm_ifce_types.h

%changelog
* Mon Nov 28 2011 Adrien Devress <adevress at cern.ch> - 1.19-1
 - Initial build 
