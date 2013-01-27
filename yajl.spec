# $Id$

%define YAJLVER 2.0.5

%if %(echo %{_target} | grep -q 'i.86*' && echo 1 || echo 0)
%define _lib lib
%endif

%if "%{?_lib}" == "lib64"
%define _cmake_lib_suffix64 -DLIB_SUFFIX=64
%endif

%ifos aix5.1 aix5.2 aix5.3 aix6.1 aix7.1
%define _disable_docs -DDISABLE_DOCS=1
%endif # aix

%define _unpackaged_files_terminate_build 0

Name: yajl
Version: %{YAJLVER}
Summary: Yet Another JSON Library (YAJL)
Release: 3%{?dist}
License: BSD
Group: Development/Libraries
Source0: yajl-%{YAJLVER}.tar.bz2
Url: https://github.com/likema/yajl
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildRequires: gcc, cmake

%description
Yet Another JSON Library. YAJL is a small event-driven
(SAX-style) JSON parser written in ANSI C, and a small
validating JSON generator.

%package devel
Summary: Libraries, includes, etc to develop with YAJL
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Yet Another JSON Library. YAJL is a small event-driven
(SAX-style) JSON parser written in ANSI C, and a small
validating JSON generator.

This sub-package provides the libraries and includes
necessary for developing against the YAJL library.

%ifnos aix5.1 aix5.2 aix5.3 aix6.1 aix7.1
%package doc
Summary: Documentation to develop with YAJL
Group: Documentation

%description doc
Yet Another JSON Library. YAJL is a small event-driven
(SAX-style) JSON parser written in ANSI C, and a small
validating JSON generator.

This sub-package provides the API documentation.
%endif # aix

%package tools
Summary: Tools that use YAJL
Group: Development/Tools
Requires: %{name} = %{version}-%{release}

%description tools
Yet Another JSON Library. YAJL is a small event-driven
(SAX-style) JSON parser written in ANSI C, and a small
validating JSON generator.

This sub-package provides a small collection of tools.

json_reformat - beautifies or minimizes format of JSON data
json_verify - validates JSON data

%prep
%setup -q

%build
cmake \
	-DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_SKIP_RPATH=ON \
	%{?_disable_docs} \
	%{?_cmake_lib_suffix64}
make %{?_smp_mflags}


%ifnos aix5.1 aix5.2 aix5.3 aix6.1 aix7.1
%check
LD_LIBRARY_PATH=src test/run_tests.sh
%endif # aix

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%ifnos aix5.1 aix5.2 aix5.3 aix6.1 aix7.1
%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
%endif # aix

%files
%defattr(-,root,root)
%doc COPYING ChangeLog README.md TODO
%ifos aix5.1 aix5.2 aix5.3 aix6.1 aix7.1
%{_libdir}/lib*.so
%else
%{_libdir}/lib*.so.*
%endif # aix

%files devel
%defattr(-,root,root)
%doc COPYING
%ifnos aix5.1 aix5.2 aix5.3 aix6.1 aix7.1
%doc %{_mandir}/*
%{_libdir}/lib*.so
%endif # aix
%{_libdir}/lib*.a
%{_includedir}/*

%ifnos aix5.1 aix5.2 aix5.3 aix6.1 aix7.1
%files doc
%defattr(-,root,root)
%doc COPYING
%doc share/doc/libyajl-doc/*
%endif # aix

%files tools
%defattr(-,root,root)
%doc COPYING
%{_bindir}/*

%changelog
* Sat Jan 26 2013 Like Ma <likemartinma@gmail.com> - 2.0.5-3
- Refine source directory structures and cmake files.

* Sun Apr 01 2012 Like Ma <likemartinma@gmail.com> - 2.0.5-2
- Merge from upstream 2.0.5

* Thu Nov 19 2011 Like Ma <likemartinma@gmail.com> - 2.0.3-2
- Add -fPIC to yajl static lib.

* Thu Nov 18 2011 Like Ma <likemartinma@gmail.com> - 2.0.3-1
- Update to 2.0.3 release

* Thu Nov 10 2011 Daniel P. Berrange <berrange@redhat.com> - 2.0.1-1
- Update to 2.0.1 release

* Tue May  3 2011 Daniel P. Berrange <berrange@redhat.com> - 1.0.12-1
- Update to 1.0.12 release

* Fri Dec 17 2010 Daniel P. Berrange <berrange@redhat.com> - 1.0.11-1
- Update to 1.0.11 release

* Mon Jan 11 2010 Daniel P. Berrange <berrange@redhat.com> - 1.0.7-3
- Fix ignoring of cflags (rhbz #547500)

* Tue Dec  8 2009 Daniel P. Berrange <berrange@redhat.com> - 1.0.7-2
- Change use of 'define' to 'global'

* Mon Dec  7 2009 Daniel P. Berrange <berrange@redhat.com> - 1.0.7-1
- Initial Fedora package
