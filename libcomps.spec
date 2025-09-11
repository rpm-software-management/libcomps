%define __cmake_in_source_build 1

Name:           libcomps
Version:        0.1.23
Release:        1%{?dist}
Summary:        Comps XML file manipulation library

License:        GPL-2.0-or-later
URL:            https://github.com/rpm-software-management/libcomps
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  cmake >= 3.10
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
BuildRequires:  python3-sphinx


%description -n python-%{name}-doc
Documentation files for python bindings libcomps library.

%package -n python3-%{name}
Summary:        Python 3 bindings for libcomps library
BuildRequires:  python3-devel
BuildRequires:  python3-setuptools
BuildRequires:  make
%{?python_provide:%python_provide python3-%{name}}
Requires:       %{name}%{?_isa} = %{version}-%{release}
Obsoletes:      platform-python-%{name} < %{version}-%{release}

%description -n python3-%{name}
Python3 bindings for libcomps library.

%prep
%autosetup -p1 -n %{name}-%{version}

mkdir build-py3
mkdir build-doc

%build
pushd build-py3
  %cmake ../libcomps/
  %cmake_build
popd

pushd build-doc
  %cmake ../libcomps/
  %cmake_build -t docs
  %cmake_build -t pydocs
popd

%install
pushd build-py3
  %cmake_install
popd

%check
pushd build-py3
  %cmake_build -t test
  %cmake_build -t pytest
popd

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

%files -n python3-%{name}
%{python3_sitearch}/%{name}/
%{python3_sitearch}/%{name}-%{version}-py%{python3_version}.egg-info

%changelog
