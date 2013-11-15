
message (${CMAKE_CURRENT_SOURCE_DIR})
#FILE(GLOB TESTS_FILES "*.py")
FILE(GLOB TESTS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/tests/"
                      "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py")
FILE(GLOB COMPS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/tests/comps/"
                      "${CMAKE_CURRENT_SOURCE_DIR}/tests/comps/*.xml*")

list(LENGTH COMPS_FILES len)
math (EXPR len "${len} - 1")

foreach(x RANGE  0 ${len})
    list(GET COMPS_FILES ${x} val)
endforeach()

#foreach(x ${TEST_FILES})
#    message("tests:" ${x})
#endforeach()
