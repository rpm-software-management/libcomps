libcomps
========

Libcomps is alternative for yum.comps library. It's written in pure C as library
and there's bindings for python2 and python3.

Building
--------

### Requirements

for automatic build system:

*   cmake ( >= 2.6) http://www.cmake.org/
*   make http://ftp.gnu.org/gnu/make/

for libcomps library:

*   libxml2 http://www.xmlsoft.org/
*   expat   http://expat.sourceforge.net/
*   gcc     http://gcc.gnu.org/

for python bindings:

*   python http://python.org/

for C library tests:

*   check http://check.sourceforge.net/

for documentation build:

*   doxygen http://www.stack.nl/~dimitri/doxygen/


### Building
1. clone this repository
        git clone https://github.com/midnightercz/libcomps.git
2. run cmake

    - for python2 bindings run cmake build system with no params:
            cd <PATH_TO_DIR_WHERE_YOU_WANT_TO_BUILD_IN>
            cmake <PATH_WHERE_YOU_CLONED_REPO>
    - for python3 bindings run cmake build system with PYTHON_DESIRED param:
            cd <PATH_TO_DIR_WHERE_YOU_WANT_TO_BUILD_IN>
            cmake -DPYTHON_DESIRED:str=3 <PATH_WHERE_YOU_CLONED_REPO>
3. run make
        make

### Building rpm package
1.  run build\_prep.sh for .spec file substitution and tarball creation:
            sh build_prep.sh
2.  copy libcomps.spec and libcomps-(git_commit_rev).tar.xz to SPECS and
    SOURCES dirs
            cp libcomps-*.tar.xz <PATH_TO_YOUR_RPMBUILD_SOURCES_DIR>/
            cp libcomps.spec <PATH_TO_YOUR_RPMBUILD_SPECS_DIR>/
3. run rpmbuild. If you want build bindings only for specified verion of python
   edit top of libcomps.spec file:
            %global with_python 1
            %global with_python3 1

### Installing
*   After successful build run:
            make install
*   Or install rpm package

### Testing
After build, you can find test for C library in __tests  directory__, starting with
`test_` prefix. Binding tests are at (for now) same directory as python binding
library. You can run `python __test.py` or `python3 __test.py` (even without
installing library) for tests bindings. Also this unittest serves as python
bindings documentation
(for now).

### Documentation
After build, documentation is in __docs directory__. Documentation is built
in html xml and latex format. Only uncomplete documentation for C library
is available at the moment. Look at bindings unittest `__test.py__` for python
bindings usage.

