# unversionned doc dir F20 change https://fedoraproject.org/wiki/Changes/UnversionedDocdirs
%{!?_pkgdocdir: %global _pkgdocdir %{_docdir}/%{name}-%{version}}

Name:		srm-ifce
Version:	1.22.1
Release:	1%{?dist}
Summary:	SRM client side library
Group:		Applications/Internet
License:	ASL 2.0
URL:		http://dmc.web.cern.ch/
# svn export http://svn.cern.ch/guest/lcgutil/srm-ifce/trunk srm-ifce
Source0:	http://grid-deployment.web.cern.ch/grid-deployment/dms/lcgutil/tar/%{name}/%{name}-%{version}.tar.gz 
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	cmake
BuildRequires:	CGSI-gSOAP-devel >= 1.3.6
BuildRequires:	glib2-devel
BuildRequires:	globus-ftp-client-devel
BuildRequires:	globus-gss-assist-devel

Requires:  CGSI-gSOAP >= 1.3.6

%description
srm-ifce is a client side implementation of the SRMv2 specification
for GFAL1/2 and FTS. SRM means Storage Resource Manager Interface, it is a 
specification of a SOAP interface providing a generic way to manage 
distributed storage systems.

%package devel
Summary:	SRM client side headers and development files
Group:		Development/Libraries
Requires:	%{name}%{?_isa} = %{version}-%{release}
Requires:	glib2-devel
Requires:	pkgconfig

%description devel
Development libraries, pkgconfig files and header files for
the srm-ifce.

%prep
%setup -q

%build
%cmake -D DOC_INSTALL_DIR=%{_pkgdocdir} .
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
%{_pkgdocdir}/LICENSE
%{_pkgdocdir}/VERSION
%{_pkgdocdir}/README

%files devel
%defattr (-,root,root)
%{_libdir}/pkgconfig/*
%{_libdir}/libgfal_srm_ifce.so
%{_includedir}/gfal_srm_ifce.h
%{_includedir}/gfal_srm_ifce_types.h
%{_pkgdocdir}/RELEASE-NOTES

%changelog
* Mon Apr 07 2014 Alejandro Alvarez <aalvarez at cern.ch> - 1.19.0-1
 - Release srm-ifce 1.19.0

* Fri Sep 20 2013 Adrien Devresse <adevress at cern.ch> - 1.18.0-1
 - Release srm-ifce 1.18.0

* Tue Aug 19 2013 Alejandro Alvarez <aalvarez at cern.ch> - 1.17.0-0
 - Release srm-ifce 1.17.0
 
* Tue Jul 02 2013 Adrien Devresse <adevress at cern.ch>  - 1.16.0-0
 - Release srm-ifce 1.16.0, first post-EMI release 

* Mon Jun 10 2013 Michail Salichos <msalicho at cern.ch> - 1.15.3-3
 - fixing memory leaks

* Fri Apr 26 2013 Michail Salichos <msalicho at cern.ch> - 1.15.3-2
 - added srmmv implementation

* Wed Mar 20 2013 adevress at cern.ch - 1.15.2-1
 - EMI lcgutil 1.15.0 release
 
* Thu Mar 14 2013 Michail Salichos <msalicho at cern.ch> - 1.15.2-0
 - avoid double initialization when session reuse is enabled

* Fri Feb 22 2013 Adrien Devresse <adevress at cern.ch> - 1.15.1-0
 - fix an estimatedWaitTime problem with the backoff logic
 - introduce srm session reuse


* Tue Jan 08 2013 Adrien Devresse <adevress at cern.ch> - 1.14.0-1
- correct two timeout bug LCGUTIL-78 and LCGUTIL-82

* Thu Nov 29 2012 Adrien Devresse <adevress at cern.ch> - 1.14.0-0
- correct misleading namespace in PrepareToGetRequestStatus
- correct a timeout issue related to exponential backoff system in put/get
- improve reliability of the exponential backoff wait system
- big big code cleaning
- re-factory of the context system with backward compatibility
- fix the srm timeout issue
- fix the srm put done issue for long transfer

* Fri Jul 20 2012 Adrien Devresse <adevress at cern.ch> - 1.13.0-0
 - Synchronise with EMI 2 Update 13
 - introduction of operation timeout
 - buffer overflow  and memory corruption corrections
 - minor warning corrections

* Wed Apr 18 2012 Zsolt Molnar <Zsolt.Molnar@cern.ch> - 1.12.3-1
 - Integrating to EPEL, adapting to changes in external components

* Tue Feb 14 2012 Adrien Devresse <adevress at cern.ch> - 1.12.2-10
 - correct smash stack related to gsoap

* Thu Feb 09 2012 Adrien Devresse <adevress at cern.ch> - 1.12.2-9
 - correct gsoap issue for f18

* Thu Feb 09 2012 Adrien Devresse <adevress at cern.ch> - 1.12.2-8
 - Recompile with gsoap 2.8 compatibility 

* Thu Feb 02 2012 Adrien Devresse <adevress at cern.ch> - 1.12.2-7
 - remove macro of pkgconfig dependency causing bug on i686 pkgs
 
* Mon Jan 30 2012 Adrien Devresse <adevress at cern.ch> - 1.12.2-6
 - Fix a stack smash problem relative to gsoap internal struct

* Sun Jan 15 2012 Adrien Devresse <adevress at cern.ch> - 1.12.1-5
 - add pkg-config files
 - add dist macro
 - correct buildroot path

* Wed Jan 11 2012 Adrien Devresse <adevress at cern.ch> - 1.12.1-4
 - Add a fixed source tree for packaging
 - Correct a problem of include with gcc 4.6

* Thu Jan 05 2012 Adrien Devresse <adevress at cern.ch> - 1.12.1-3
 - Second Revision for EPEL/fedora conformance

* Fri Dec 16 2011 Adrien Devresse <adevress at cern.ch> - 1.12.1-2
 - First Revision for EPEL/fedora conformance

* Mon Nov 28 2011 Adrien Devresse <adevress at cern.ch> - 1.12.1-1
 - Initial build 
