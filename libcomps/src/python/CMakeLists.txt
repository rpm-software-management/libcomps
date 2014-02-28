include(tests/tests.cmake)
#include(pycopy.cmake)
include(pversion.cmake)
set(pycopy "py${pversion}-copy")

set (pycomps_TESTSDIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/")


set(PYCOMPS_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/python${pversion}")

add_subdirectory(src)
add_subdirectory(docs)
