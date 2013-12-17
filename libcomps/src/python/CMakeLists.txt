include(tests/tests.cmake)

if (NOT PYTHON_DESIRED)
    set (PYTHON_DESIRED "2")
endif()

set (pycomps_TESTSDIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/")

if (${PYTHON_DESIRED} STREQUAL "2")
    message("Building for python2")
    set(pversion 2)
else()
    message("Building for python3")
    set(pversion 3)
endif()

set(PYCOMPS_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/python${pversion}")

add_subdirectory(src)
add_subdirectory(docs)
