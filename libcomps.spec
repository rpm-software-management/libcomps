%global with_python 1
%global with_python3 1

%global commit 27a3ba47bf08a51e674ebd255837f7a3aecde57d
%global shortcommit 27a3ba4
%global name libcomps


Name:           libcomps
Version:        0.1.1
Release:        1%{?dist}
Summary:        Alternative for yum.comps writen in C

Group:          Development/Libraries
License:        GPLv2+
URL:            https://github.com/midnightercz/libcomps/
Source0:        https://github.com/midnightercz/libcomps/archive/%{commit}/libcomps-%{commit}.tar.gz

BuildRequires:  libxml2-devel
BuildRequires:  check-devel
BuildRequires:  expat-devel
BuildRequires:  doxygen
BuildRequires:  cmake


%description -n libcomps
Alternative for yum.comps written in C

%if %{with_python}
%package -n python-libcomps
Summary:        libcomps bindings for python2
Group:          Development/Libraries
BuildRequires:  python-devel
Requires:       python2

%description -n python-libcomps
libcomps bindings for python2
%endif

%if %{with_python3}
%package -n python3-libcomps
Summary:        libcomps bindings for python3
Group:          Development/Libraries
BuildRequires:  python3-devel
Requires:       python3

%description -n python3-libcomps
libcomps bindings for python3
%endif


%prep -n libcomps
%setup -qn %{name}-%{commit}
%if %{with_python3}
    rm -rf py3
    mkdir ../py3
    cp -a . ../py3/
    mv ../py3 ./
%endif


%build -n libcomps
# order matters - py3 needs to be build first; why?
%if %{with_python}
    %cmake -DPYTHON_DESIRED:STRING=2 libcomps/
    make %{?_smp_mflags}
%endif
%if %{with_python3}
    pushd py3
    %cmake -DPYTHON_DESIRED:STRING=3 libcomps/
    make %{?_smp_mflags}
    popd
%else
    %cmake -DPYTHON_DESIRED:STRING=NO libcomps/
    make %{?_smp_mflags}
%endif



%install
rm -rf $buildroot
make install DESTDIR=%{buildroot}
%if %{with_python3}
pushd py3
make install DESTDIR=%{buildroot}
popd
%endif


%files
%doc docs/html
%{_libdir}/*
%exclude %{_libdir}/python*
%{_includedir}/*

%if %{with_python}
%files -n python-libcomps
%doc docs/*
%{_libdir}/python2*
%endif

%if %{with_python3}
%files -n python3-libcomps
%doc docs/*
%{_libdir}/python3*
%endif


%changelog
* Tue Jun 25 2013 Jindrich Luza <jluza@redhat.com> 0.1.1-1
- starting use tito tool


