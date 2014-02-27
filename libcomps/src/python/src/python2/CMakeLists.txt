find_package (PythonLibs 2.7)
find_package (PythonInterp 2.7 REQUIRED)
add_custom_target(py2-copy)

execute_process (COMMAND ${PYTHON_EXECUTABLE} -c "from sys import stdout; from distutils import sysconfig; stdout.write(sysconfig.get_python_lib(True))" OUTPUT_VARIABLE PYTHON_INSTALL_DIR)

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${LIBCOMPS_INCLUDE_PATH})

configure_file(${pycomps_TESTSDIR}/run_tests.sh.in
               ./libcomps/run_tests.sh)

list(LENGTH TESTS_FILES len)
math (EXPR len "${len} - 1")
foreach(x RANGE 0 ${len})
    list(GET TESTS_FILES ${x} val)
    add_custom_command(TARGET py2-copy PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
                        copy ${pycomps_TESTSDIR}/${val} ./libcomps/)
    #file(COPY ${pycomps_TESTSDIR}/${val} DESTINATION ./libcomps/)
endforeach()

list(LENGTH COMPS_FILES len)
math (EXPR len "${len} - 1")
foreach(x RANGE 0 ${len})
    list(GET COMPS_FILES ${x} val)
    add_custom_command(TARGET py2copy PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
            copy ${pycomps_TESTSDIR}/comps/${val} ./libcomps/comps/)
    #file(COPY ${pycomps_TESTSDIR}/comps/${val} DESTINATION ./libcomps/comps/)
endforeach()
add_custom_command(TARGET py2-copy PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
        copy ${pycomps_SRCDIR}/__init__.py ./libcomps/)
#file(COPY ${pycomps_SRCDIR}/__init__.py DESTINATION ./libcomps/)

add_library(pycomps SHARED ${pycomps_SRC})
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
set_target_properties(pycomps PROPERTIES PREFIX "")
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")
add_dependencies(pycomps py2-copy)

target_link_libraries(pycomps libcomps)
target_link_libraries(pycomps ${EXPAT_LIBRARIES})
target_link_libraries(pycomps ${LIBXML2_LIBRARIES})
target_link_libraries(pycomps ${PYTHON_LIBRARIES})


add_custom_target(pytest_run COMMAND ./run_tests.sh
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                   DEPENDS pycomps
                   COMMENT "Running python binding tests")

INSTALL(FILES ${pycomps_SRCDIR}/__init__.py DESTINATION ${PYTHON_INSTALL_DIR}/libcomps)
#INSTALL(FILES ${pycomps_SRCDIR}/tests/__test.py DESTINATION
#              ${PYTHON_INSTALL_DIR}/libcomps/)
#INSTALL(FILES ${pycomps_SRCDIR}/tests/fedora_comps.xml DESTINATION
#              ${PYTHON_INSTALL_DIR}/libcomps/)
#INSTALL(FILES ${pycomps_SRCDIR}/tests/sample_comps.xml DESTINATION
#              ${PYTHON_INSTALL_DIR}/libcomps/)
#INSTALL(FILES ${pycomps_SRCDIR}/tests/main_comps.xml DESTINATION
#              ${PYTHON_INSTALL_DIR}/libcomps/)
#INSTALL(FILES ${pycomps_SRCDIR}/tests/main_comps2.xml DESTINATION
#              ${PYTHON_INSTALL_DIR}/libcomps/)
INSTALL (TARGETS pycomps LIBRARY DESTINATION ${PYTHON_INSTALL_DIR}/libcomps)
