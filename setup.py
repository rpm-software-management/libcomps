import json

from skbuild import setup


with open('version.json', 'r+') as version_file:
    version_dict = json.loads(version_file.read())
    # build version string
    version = '{major}.{minor}.{patch}-{release}'.format(
        major=version_dict['libcomps_VERSION_MAJOR'],
        minor=version_dict['libcomps_VERSION_MINOR'],
        patch=version_dict['libcomps_VERSION_PATCH'],
        release=version_dict['libcomps_RELEASE']
    )

with open('README.md') as readme:
    README = readme.read()

setup(
    name='libcomps',
    description='Comps XML file manipulation library',
    long_description_content_type='text/markdown',
    long_description=README,
    version=version,
    license='GPLv2+',
    author='RPM Software Management',
    author_email='rpm-ecosystem@lists.rpm.org',
    url='https://github.com/rpm-software-management',
    classifiers=[
        'License :: OSI Approved :: GNU General Public License v2 or later (GPLv2+)',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: C',
        'Topic :: System :: Software Distribution',
        'Topic :: System :: Systems Administration',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
    ],
    packages=['libcomps'],
    package_dir={
        'libcomps': 'libcomps/src/python/src/libcomps/'
    },
    cmake_args=[
        '-DBIN_INSTALL_DIR:PATH=libcomps/src/python/src/libcomps/data/bin',
        '-DBUILD_LIBCOMPS_SHARED:BOOL=OFF',
        '-DENABLE_DEVELOPMENT:BOOL=OFF',
        '-DENABLE_DOCS:BOOL=OFF',
        '-DENABLE_TESTS:BOOL=OFF'
    ],
    cmake_languages=['C'],
    cmake_source_dir="libcomps",
)
