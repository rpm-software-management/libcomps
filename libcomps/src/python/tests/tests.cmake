
message (${CMAKE_CURRENT_SOURCE_DIR})
#FILE(GLOB TESTS_FILES "*.py")
set(TESTS_DIR "${PROJECT_SOURCE_DIR}/src/python/tests/")
FILE(GLOB TESTS_FILES RELATIVE "${TESTS_DIR}" "${TESTS_DIR}*.py")
FILE(GLOB COMPS_FILES RELATIVE "${TESTS_DIR}comps/" 
                               "${TESTS_DIR}/comps/*.xml*")

list(LENGTH COMPS_FILES len)
math (EXPR len "${len} - 1")

foreach(x RANGE  0 ${len})
    list(GET COMPS_FILES ${x} val)
endforeach()

#foreach(x ${TEST_FILES})
#    message("tests:" ${x})
#endforeach()
