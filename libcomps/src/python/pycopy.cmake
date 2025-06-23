
include(${PROJECT_SOURCE_DIR}/src/python/tests/tests.cmake)

list(LENGTH TESTS_FILES len)
math (EXPR len "${len} - 1")

#set(pycopy "py${pversion}-copy")

if (NOT TARGET ${pycopy})
    add_custom_target(${pycopy} DEPENDS pycomps)
endif()

set (pycomps_SRCDIR "${PROJECT_SOURCE_DIR}/src/python/src/")
set (pycomps_TESTDIR "${PROJECT_SOURCE_DIR}/src/python/tests/")
set (pycomps_LIBPATH ${PYCOMPS_LIB_PATH})#"${PROJECT_BINARY_DIR}/src/python/src/python${pversion}")

#add_custom_command(TARGET pycopy PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
#                    make_directory "${CP_DST}")

add_custom_command(TARGET ${pycopy} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                    make_directory ${pycomps_LIBPATH}/libcomps/comps/)

foreach(x RANGE 0 ${len})
    list(GET TESTS_FILES ${x} val)
    add_custom_command(TARGET ${pycopy} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                        copy ${pycomps_TESTSDIR}${val}
                             ${pycomps_LIBPATH}/libcomps/
                        COMMENT "copy ${pycomps_TESTSDIR}/${val} to ${pycomps_LIBPATH}/libcomps/")
endforeach()

list(LENGTH COMPS_FILES len)
math (EXPR len "${len} - 1")
foreach(x RANGE 0 ${len})
    list(GET COMPS_FILES ${x} val)
    add_custom_command(TARGET ${pycopy} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                       copy ${pycomps_TESTSDIR}/comps/${val}
                            ${pycomps_LIBPATH}/libcomps/comps/
                        COMMENT "${pycomps_TESTSDIR}/comps/${val} to ${pycomps_LIBPATH}/libcomps/comps/")
    #file(COPY ${pycomps_TESTSDIR}/comps/${val} DESTINATION ./libcomps/comps/)
endforeach()
add_custom_command(TARGET ${pycopy} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
        copy ${pycomps_SRCDIR}/libcomps/__init__.py ${pycomps_LIBPATH}/libcomps/
                        COMMENT "copy ${pycomps_SRCDIR}/libcomps/__init__.py to ${pycomps_LIBPATH}/libcomps/")

configure_file(${pycomps_TESTSDIR}run_tests.sh.in ${pycomps_LIBPATH}/libcomps/run_tests.sh)
#endif (NOT TARGET ${pycopy})
