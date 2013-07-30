%global with_python 1
%global with_python3 1

%global commit 3af5bea29a169726a61a2d9ef1377669adae6ee3
%global name libcomps


Name:           libcomps
Version:        0.1.2
Release:        1%{?dist}
Summary:        Comps XML file manipulation library

Group:          Development/Libraries
License:        GPLv2+
URL:            https://github.com/midnightercz/libcomps/
Source0:        https://github.com/midnightercz/libcomps/archive/3af5bea29a169726a61a2d9ef1377669adae6ee3/libcomps-3af5bea29a169726a61a2d9ef1377669adae6ee3.tar.gz
BuildRequires:  libxml2-devel
BuildRequires:  check-devel
BuildRequires:  expat-devel
BuildRequires:  cmake >= 2.7

%description
Libcomps is library for structure-like manipulation with content of
comps XML files.

%package doc
Summary:        Documentation files for libcomps library
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}
BuildRequires:  doxygen

%description doc
Documentation files for libcomps library

%package devel
Summary:        Development files for libcomps library
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Development files for libcomps library


%if %{with_python}
%package -n python-libcomps
Summary:        Python2 bindings for libcomps library
Group:          Development/Libraries
BuildRequires:  python-devel
Requires:       python2
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n python-libcomps
Python2 bindings for libcomps library
%endif

%if %{with_python3}
%package -n python3-libcomps
Summary:        Python3 bindings for libcomps library
Group:          Development/Libraries
BuildRequires:  python3-devel
Requires:       python3
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n python3-libcomps
Python3 bindings for libcomps library
%endif


%prep
%setup -qn %{name}-%{commit}
%if %{with_python3}
    rm -rf py3
    mkdir ../py3
    cp -a . ../py3/
    mv ../py3 ./
%endif

%build
# order matters - py3 needs to be build first; why?
%if %{with_python}
    %cmake -DPYTHON_DESIRED:STRING=2 libcomps/
    make %{?_smp_mflags}
    make %{?_smp_mflags} docs
%endif
%if %{with_python3}
    pushd py3
    %cmake -DPYTHON_DESIRED:STRING=3 libcomps/
    make %{?_smp_mflags}
    make %{?_smp_mflags} docs
    popd
%else
    %cmake -DPYTHON_DESIRED:STRING=NO libcomps/
    make %{?_smp_mflags}
    make %{?_smp_mflags} docs
%endif


%check devel
    make ctest
%if %{with_python}
    make pytest
%endif

%install
make install DESTDIR=%{buildroot}
%if %{with_python3}
pushd py3
make install DESTDIR=%{buildroot}
popd
%endif

%clean
rm -rf $buildroot

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{_libdir}/libcomps.so.*
%doc README.md COPYING

%files devel
%{_libdir}/libcomps.so
%{_includedir}/*

%files doc
%doc docs/libcomps-doc/html

%if %{with_python}
%files -n python-libcomps
%{_libdir}/python2*
%exclude %{_libdir}/python2/libcomps/__pycache__
%endif

%if %{with_python3}
%files -n python3-libcomps
%{_libdir}/python3*
%exclude %{_libdir}/python3/libcomps/__pycache__
%endif


%changelog
* Tue Jul 30 2013 Jindrich Luza <jluza@redhat.com> 0.1.2.git3af5bea
- spec fixed

* Thu Jul 18 2013 Jindrich Luza <jluza@redhat.com> 0.1.2
- automatic changelog system
- fixed issue #14
- libcomps.Dict is now behave more like python dict. Implemented iter(libcomps.Dict)
- libcomps.iteritems() and libcomps.itervalues()
- added <packagereq requires=...> support
- remaked error reporting system.
-     libcomps.Comps.fromxml_f and libcomps.Comps.fromxml_str now return
-     -1, 0 or 1. 0 means parse procedure completed without any problem,
-     1 means there's some errors or warnings but not fatal. -1 indicates
-     fatal error problem (some results maybe given, but probably incomplete
-     and invalid)
- errors catched during parsing can be obtained by calling
-     libcomps.Comps.get_last_parse_errors
- all log is given by
-     libcomps.Comps.get_last_parse_log
- improved integrated tests
- documentation is now isolated make target ('make docs')
- prop system complete
- fixed issue 1
- fixed issue 3
- new prop system in progress....
- separated doc package
- some minor fixes in CMakeFiles

* Tue Jun 25 2013 Jindrich Luza <jluza@redhat.com> 0.1.1-1
- Automatic commit of package [libcomps] release [0.1.1-1].


