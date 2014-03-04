project(libcomps)

cmake_minimum_required (VERSION 2.6)
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
include (${CMAKE_SOURCE_DIR}/version.cmake)
#set (CMAKE_CXX_COMPILER g++)

set (CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} --pedantic -std=c99 -Wall -Wextra -g -Wno-missing-field-initializers -O2 -fno-strict-aliasing -g")
set (CMAKE_C_FLAGS_DEBUG    "${CMAKE_C_FLAGS} -ggdb -O0 -Wall -Wextra")

set (VERSION "${libcomps_VERSION_MAJOR}.${libcomps_VERSION_MINOR}.${libcomps_VERSION_PATCH}")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
#include_directories("${PROJECT_SOURCE_DIR}/src/libcomps")

FIND_LIBRARY(CHECK_LIBRARY NAMES check)
FIND_LIBRARY(EXPAT_LIBRARY NAMES expat)

find_package(LibXml2 REQUIRED)
find_package(EXPAT REQUIRED)

include_directories(${CHECK_INCLUDE_DIR})
include_directories(${EXPAT_INCLUDE_DIR})
include_directories(${LIBXML2_INCLUDE_DIR})

#add_custom_target(pytest DEPENDS pytest_run)
#add_custom_target(ctest DEPENDS test_comps_run test_parse_run)
#add_custom_target(test DEPENDS ctest pytest)
#add_custom_command(TARGET test POST_BUILD COMMAND ./run_tests.sh
#                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
#                   COMMENT "Running python binding tests")

add_custom_target(pytest)
add_custom_target(ctest)
add_custom_target(test)
add_dependencies(test ctest)
add_dependencies(test pytest)

add_subdirectory (src)
add_subdirectory (tests)
add_subdirectory (docs EXCLUDE_FROM_ALL)
