libcomps
========

Libcomps is alternative for yum.comps library. It's written in pure C as library
with Python bindings.

Building
--------

### Requirements

for automatic build system:

*   cmake ( >= 2.6) http://www.cmake.org/
*   make http://ftp.gnu.org/gnu/make/

for libcomps library:

*   zlib    http://www.zlib.net/
*   libxml2 http://www.xmlsoft.org/
*   expat   http://expat.sourceforge.net/
*   gcc     http://gcc.gnu.org/

for python bindings:

*   python http://python.org/

for C library tests:

*   check http://check.sourceforge.net/

for documentation build:

*   doxygen http://www.stack.nl/~dimitri/doxygen/

for rpm building:

*   tito https://github.com/dgoodwin/tito


### Building
1. clone this repository
        git clone https://github.com/rpm-software-management/libcomps.git
2. from the checkout dir:

        mkdir build
        cd build/
        cmake ../libcomps
        make
3. building the documentation:

        make docs
        make pydocs

### Building rpm package
You can use tito for building rpm package. From checkout dir:

        tito build --rpm --test

## Building Python package
To create a binary "wheel" distribution, use:

    python setup.py bdist_wheel

To create a source distribution, use:

    python setup.py sdist

Installing source distributions require the installer of the package to have all of the build dependencies installed on their system, since they compile the code during installation. Binary distributions are pre-compiled, but they are likely not portable between substantially different systems, e.g. Fedora and Ubuntu.

Note: if you are building a bdist or installing the sdist on a system with an older version of Pip, you may need to install the ```scikit-build``` Python package first.

To install either of these packages, use:

    pip install dist/{{ package name }}

To create an "editable" install of libcomps, use:

    python setup.py develop

Note: To recompile the libraries and binaries, you muse re-run this command.

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

Contribution
------------

Here's the most direct way to get your work merged into the project.

1. Fork the project
1. Clone down your fork
1. Implement your feature or bug fix and commit changes
1. If the change fixes a bug at [Red Hat bugzilla](https://bugzilla.redhat.com/), or if it is important to the end user, add the following block to the commit message:

       = changelog =
       msg:           message to be included in the changelog
       type:          one of: bugfix/enhancement/security (this field is required when message is present)
       resolves:      URLs to bugs or issues resolved by this commit (can be specified multiple times)
       related:       URLs to any related bugs or issues (can be specified multiple times)

   * For example::

         = changelog =
         msg: Do not skip type=mandatory in xml output
         type: bugfix
         resolves: https://bugzilla.redhat.com/show_bug.cgi?id=1771224

   * For your convenience, you can also use git commit template by running the following command in the top-level directory of this project:

         git config commit.template ./.git-commit-template

1. Push the branch to your fork
1. Send a pull request for your branch

