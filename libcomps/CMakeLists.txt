cmake_minimum_required (VERSION 3.10)
project(libcomps C)

cmake_policy(SET CMP0148 OLD)
cmake_policy(SET CMP0175 NEW)

include (GNUInstallDirs)
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
include (${CMAKE_SOURCE_DIR}/version.cmake)
#set (CMAKE_CXX_COMPILER g++)

OPTION (ENABLE_TESTS "Build test?" ON)
OPTION (ENABLE_DOCS "Build docs?" ON)

set(LIBCOMPS_OUT "${CMAKE_CURRENT_BINARY_DIR}/src")

set (CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} --pedantic -std=c99 -Wall -Wextra -g -Wno-missing-field-initializers -O2 -fno-strict-aliasing -g")
set (CMAKE_C_FLAGS_DEBUG    "${CMAKE_C_FLAGS} -ggdb -O0 -Wall -Wextra")

set (VERSION "${libcomps_VERSION_MAJOR}.${libcomps_VERSION_MINOR}.${libcomps_VERSION_PATCH}")
#set (libcomps_SOURCE comps_list.c)
#set (libcomps_HEADERS comps_list.h)

option(BUILD_LIBCOMPS_SHARED "Build libcomps as a shared library?" ON)
if(NOT BUILD_LIBCOMPS_SHARED)
  set(CMAKE_POSITION_INDEPENDENT_CODE 1)
endif()

option(ENABLE_DEVELOPMENT "Install development files?" ON)
option(ENABLE_DOCS "Build docs?" ON)
option(ENABLE_TESTS "Build test?" ON)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
#include_directories("${PROJECT_SOURCE_DIR}/src/libcomps")

if (ENABLE_TESTS)
  find_library(CHECK_LIBRARY REQUIRED NAMES check)
endif()
find_library(EXPAT_LIBRARY NAMES expat)

find_package(ZLIB REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(EXPAT REQUIRED)

include_directories(${CHECK_INCLUDE_DIR})
include_directories(${EXPAT_INCLUDE_DIR})
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS})

#add_custom_target(pytest DEPENDS pytest_run)
#add_custom_target(ctest DEPENDS test_comps_run test_parse_run)
#add_custom_target(test DEPENDS ctest pytest)
#add_custom_command(TARGET test POST_BUILD COMMAND ./run_tests.sh
#                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
#                   COMMENT "Running python binding tests")

if (ENABLE_TESTS)
  add_custom_target(pytest)
  add_custom_target(ctest)
  add_custom_target(test)
  add_dependencies(test ctest)
  add_dependencies(test pytest)
endif ()

add_subdirectory (src)

if (ENABLE_TESTS)
  add_subdirectory (tests)
endif ()

if (ENABLE_DOCS)
    add_subdirectory (docs EXCLUDE_FROM_ALL)
endif (ENABLE_DOCS)
