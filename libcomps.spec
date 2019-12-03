# Do not build python3 bindings for RHEL <= 7
%if 0%{?rhel} && 0%{?rhel} <= 7
%bcond_with python3
%else
%bcond_without python3
%endif

# Do not build python2 bindings for RHEL > 7 and Fedora > 29
%if 0%{?rhel} > 7 || 0%{?fedora} > 29
%bcond_with python2
%else
%bcond_without python2
%endif

Name:           libcomps
Version:        0.1.14
Release:        1%{?dist}
Summary:        Comps XML file manipulation library

License:        GPLv2+
URL:            https://github.com/rpm-software-management/libcomps
Source0:        %{url}/archive/%{name}-%{version}/%{name}-%{version}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  cmake
BuildRequires:  gcc
BuildRequires:  libxml2-devel
BuildRequires:  check-devel
BuildRequires:  expat-devel
BuildRequires:  zlib-devel

%description
Libcomps is library for structure-like manipulation with content of
comps XML files. Supports read/write XML file, structure(s) modification.

%package devel
Summary:        Development files for libcomps library
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Development files for libcomps library.

%package doc
Summary:        Documentation files for libcomps library
Requires:       %{name} = %{version}-%{release}
BuildArch:      noarch
BuildRequires:  doxygen

%description doc
Documentation files for libcomps library.

%package -n python-%{name}-doc
Summary:        Documentation files for python bindings libcomps library
Requires:       %{name} = %{version}-%{release}
BuildArch:      noarch
%if %{with python3}
BuildRequires:  python3-sphinx
%endif
%if %{with python2}
%if 0%{?rhel} && 0%{?rhel} <= 7
BuildRequires:  python-sphinx
%else
BuildRequires:  python2-sphinx
%endif
%endif

%description -n python-%{name}-doc
Documentation files for python bindings libcomps library.

%if %{with python2}
%package -n python2-%{name}
Summary:        Python 2 bindings for libcomps library
%{?python_provide:%python_provide python2-%{name}}
BuildRequires:  python2-devel
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n python2-%{name}
Python 2 bindings for libcomps library.
%endif

%if %{with python3}
%package -n python3-%{name}
Summary:        Python 3 bindings for libcomps library
BuildRequires:  python3-devel
%{?python_provide:%python_provide python3-%{name}}
Requires:       %{name}%{?_isa} = %{version}-%{release}
Obsoletes:      platform-python-%{name} < %{version}-%{release}

%description -n python3-%{name}
Python3 bindings for libcomps library.
%endif

%prep
%autosetup -n %{name}-%{name}-%{version}

%if %{with python2}
mkdir build-py2
%endif
%if %{with python3}
mkdir build-py3
%endif
mkdir build-doc

%build
%if %{with python2}
pushd build-py2
  %cmake ../libcomps/ -DPYTHON_DESIRED:STRING=2
  %make_build
popd
%endif

%if %{with python3}
pushd build-py3
  %cmake ../libcomps/ -DPYTHON_DESIRED:STRING=3
  %make_build
popd
%endif

pushd build-doc
%if %{with python2}
  %cmake ../libcomps/ -DPYTHON_DESIRED:STRING=2
%else
%if %{with python3}
  %cmake ../libcomps/ -DPYTHON_DESIRED:STRING=3
%endif
%endif
  make %{?_smp_mflags} docs
  make %{?_smp_mflags} pydocs
popd

%install
%if %{with python2}
pushd build-py2
  %make_install
popd
%endif

%if %{with python3}
pushd build-py3
  %make_install
popd
%endif

%check
%if %{with python2}
pushd build-py2
  make test
  make pytest
popd
%endif

%if %{with python3}
pushd build-py3
  make test
  make pytest
popd
%endif

%if %{undefined ldconfig_scriptlets}
%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
%else
%ldconfig_scriptlets
%endif

%files
%license COPYING
%doc README.md
%{_libdir}/%{name}.so.*

%files devel
%{_libdir}/%{name}.so
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/%{name}/

%files doc
%doc build-doc/docs/libcomps-doc/html

%files -n python-%{name}-doc
%doc build-doc/src/python/docs/html

%if %{with python2}
%files -n python2-%{name}
%{python2_sitearch}/%{name}/
%{python2_sitearch}/%{name}-%{version}-py%{python2_version}.egg-info
%endif

%if %{with python3}
%files -n python3-%{name}
%{python3_sitearch}/%{name}/
%{python3_sitearch}/%{name}-%{version}-py%{python3_version}.egg-info
%endif

%changelog
