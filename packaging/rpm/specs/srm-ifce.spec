Name:		srm-ifce
Version:	1.12
Release:	4
Summary:	SRM client side library
Group:		Applications/Internet
License:	ASL 2.0
URL:		https://svnweb.cern.ch/trac/lcgutil
#
# The source of this package was pulled from upstream's vcs. Use the
# following commands to generate the tarball:
# svn export http://svn.cern.ch/guest/lcgutil/srm-ifce/branches/EPEL_1_12_4 srm-ifce-1.12
# tar -czvf srm-ifce-1.12.tar.gz srm-ifce-1.12
Source:		%{name}-%{version}.tar.gz 
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}

BuildRequires:	cmake
BuildRequires:	CGSI-gSOAP-devel
BuildRequires:	globus-ftp-client-devel
BuildRequires:	globus-gss-assist-devel

%description
srm-ifce is a client side implementation of the SRMv1 and SRMv2 specification 
for GFAL and FTS. SRM means Storage Resource Manager Interface, it is a 
specification of a SOAP interface providing a generic way to manage 
distributed storage systems.

%package devel
Summary:	SRM client side headers and development files
Group:		Development/Libraries
Requires:	%{name}%{?_isa} = %{version}-%{release}

%description devel
This package contains common development libraries and header files for
the srm-ifce component.

%prep
%setup -q

%build
%cmake -D DOC_INSTALL_DIR=%{_docdir}/%{name}-%{version} .
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make %{?_smp_mflags} DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr (-,root,root)
%{_bindir}/gfal_srm_ifce_version
%{_libdir}/libgfal_srm_ifce.so.*
%{_docdir}/%{name}-%{version}/LICENSE
%{_docdir}/%{name}-%{version}/VERSION
%{_docdir}/%{name}-%{version}/README

%files devel
%defattr (-,root,root)
%{_libdir}/libgfal_srm_ifce.so
%{_includedir}/gfal_srm_ifce.h
%{_includedir}/gfal_srm_ifce_types.h
%{_docdir}/%{name}-%{version}/RELEASE-NOTES

%changelog
* Wed Jan 11 2012 Adrien Devress <adevress at cern.ch> - 1.12-4
 - Add a fixed source tree for packaging
 - Correct a problem of include with gcc 4.6

* Thu Jan 05 2012 Adrien Devress <adevress at cern.ch> - 1.12-3 
 - Second Revision for EPEL/fedora conformance

* Fri Dec 16 2011 Adrien Devress <adevress at cern.ch> - 1.12-2
 - First Revision for EPEL/fedora conformance

* Mon Nov 28 2011 Adrien Devress <adevress at cern.ch> - 1.12-1
 - Initial build 
