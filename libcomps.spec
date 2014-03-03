%global commit 13f2c8dab64f6d8dbdded8266a583c286d5e0b98

%if 0%{?rhel} && 0%{?rhel} <= 6
%define python3_build 0
#%{!?__python2: %global __python2 /usr/bin/python2}
#%{!?python2_sitelib: %global python2_sitelib %(%{__python2} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")}
#%{!?python2_sitearch: %global python2_sitearch %(%{__python2} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}
%else
%define python3_build 1
%endif


Name:           libcomps
Version:        0.1.6
Release:        3%{?dist}
Summary:        Comps XML file manipulation library

Group:          Development/Libraries
License:        GPLv2+
URL:            https://github.com/midnightercz/libcomps/
Source0:        https://github.com/midnightercz/libcomps/archive/%{commit}/libcomps-%{commit}.tar.gz
BuildRequires:  libxml2-devel
BuildRequires:  check-devel
BuildRequires:  expat-devel
#%if 0%{?rhel} == 6
#BuildRequires:  cmake28
#%else
BuildRequires:  cmake
#%endif

%description
Libcomps is library for structure-like manipulation with content of
comps XML files. Supports read/write XML file, structure(s) modification.

%package doc
Summary:        Documentation files for libcomps library
Group:          Documentation
Requires:       %{name} = %{version}-%{release}
BuildArch:      noarch
BuildRequires:  doxygen

%description doc
Documentation files for libcomps library

%package -n python-libcomps-doc
Summary:        Documentation files for python bindings libcomps library
Group:          Documentation
Requires:       %{name} = %{version}-%{release}
BuildArch:      noarch
BuildRequires:  python-sphinx

%description -n python-libcomps-doc
Documentation files for python bindings libcomps library

%package devel
Summary:        Development files for libcomps library
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Development files for libcomps library

%package -n python-libcomps
Summary:        Python2 bindings for libcomps library
Group:          Development/Libraries
BuildRequires:  python-devel
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n python-libcomps
Python2 bindings for libcomps library

%if %python3_build
%package -n python3-libcomps
Summary:        Python3 bindings for libcomps library
Group:          Development/Libraries
BuildRequires:  python3-devel
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n python3-libcomps
Python3 bindings for libcomps library
%endif

%prep
%setup -qn %{name}-%{commit}

%if %python3_build == 1
rm -rf py3
mkdir ../py3
cp -a . ../py3/
mv ../py3 ./
%endif

%build
%cmake -DPYTHON_DESIRED:STRING=2 libcomps/
make %{?_smp_mflags}
make %{?_smp_mflags} docs
make %{?_smp_mflags} pydocs

%if %python3_build == 1
pushd py3
%cmake -DPYTHON_DESIRED:STRING=3 libcomps/
make %{?_smp_mflags}
popd
%endif


%check
make test
%if %{python3_build}
pushd py3
make pytest
popd
%endif

%install
make install DESTDIR=%{buildroot}

%if %{python3_build}
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

%files -n python-libcomps-doc
%doc src/python/docs/html

%files -n python-libcomps
%{_libdir}/python2*
%exclude %{_libdir}/python2/libcomps/__pycache__

%if %{python3_build}
%files -n python3-libcomps
%{_libdir}/python3*
%exclude %{_libdir}/python3/libcomps/__pycache__
%endif

%changelog
* Wed Jan 29 2014 Jindrich Luza <jluza@redhat.com> 0.1.6
- version bumped
- added libcomps.MDict.keys()
-         libcomps.MDict.values()
-         libcomps.MDict.items()
-         libcomps.MDict.clear()
-         libcomps.MDict.update()
-         libcomps.MDict.copy()
- COMPS_List replaced with COMPS_HSList
- added missing basearchonly to DocGroupPackage
- python3/CMakeLists.txt fixed
- added explicit attributes support for xml options
- added arch_filter test for python
- insert method in libcomps.Sequence
- Unioning is now accomplished with replace x append policy
- Weaker package equality check (comparing only name now)
- Fixed leeks in unioning
- modified test_merge_comps test_libcomps
- dictionaries are now storing keys in alphabetical order
- comps parser redesigned
- change python/tests directory composition
- added elem attributes check in parser
- xml output '_arch' attribute support
- parser and xml output defaults options for specify defaults values
- comps object validation in python
- added validity checker before append/set object to list (python only)
- .validate() method
- added libcomps.Dict.keys
-         libcomps.Dict.values
-         libcomps.Dict.items
-         libcomps.Dict.clear
-         libcomps.Dict.update
-         libcomps.Dict.copy
- added xml output options (comps.xml_str([options = {}]), comps.xml_f(options = {}))

* Wed Oct 23 2013 Jindrich Luza <jluza@redhat.com> 0.1.4-4
- group.uservisible is true by default now.
- fixed comps_mobjradix parent node problem
- implemented bindings for blacklist, whiteout and langpacks
- COMPS_Logger redesigned

* Tue Oct 08 2013 Jindrich Luza <jluza@redhat.com> 0.1.5
- version bump
- PyCOMPS_Sequence.__getitem__["objectid"] implemented for libcomps.GroupList, libcomps.CategoryList, libcomps.EnvList
- added missing files
- missing display_order fix for libcomps.Environment

* Tue Oct 01 2013 Jindrich Luza <jluza@redhat.com> 0.1.4
- added missing files
- architectural redesign finished
- fixed #1003986 by Gustavo Luiz Duarte guidelines (but not tested on ppc)
- fixed bug #1000449
- fixed bug #1000442
- added GroupId.default test
- some minor unreported bugs discovered during testing fixed
- finished default attribute support in groupid object
- Comps.get_last_parse_errors and Comps.get_last_parse_log has been renamed
-   as Comps.get_last_errors and Comps.get_last_log
- version bumped. Python bindings is now easier.
- added missing files

* Tue Aug 20 2013 Jindrich Luza <jluza@redhat.com> 0.1.3
- finished default attribute support in groupid object
- Comps.get_last_parse_errors and Comps.get_last_parse_log has been renamed
-   as Comps.get_last_errors and Comps.get_last_log
- finished default attribute support in groupid object
- Comps.get_last_parse_errors and Comps.get_last_parse_log has been renamed
-   as Comps.get_last_errors and Comps.get_last_log

* Thu Jul 18 2013 Jindrich Luza <jluza@redhat.com> 0.1.2
- automatic changelog system
- fixed issue #14
- libcomps.Dict is now behave more like python dict. Implemented iter(libcomps.Dict)
- libcomps.iteritems() and libcomps.itervalues()
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
- prop system complete
- fixed issue 1
- fixed issue 3
- added <packagereq requires=...> support
- new prop system in progress....
- separated doc package
- some minor fixes in CMakeFiles
- improved integrated tests

* Tue Jun 25 2013 Jindrich Luza <jluza@redhat.com> 0.1.1-1
- Automatic commit of package [libcomps] release [0.1.1-1].

