if (ENABLE_TESTS)
    include(tests/tests.cmake)
endif ()

#include(pycopy.cmake)
set(pycopy "py3-copy")

set (pycomps_TESTSDIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/")


set(PYCOMPS_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/")

add_subdirectory(src)

if (ENABLE_DOCS)
    add_subdirectory (docs)
endif ()
