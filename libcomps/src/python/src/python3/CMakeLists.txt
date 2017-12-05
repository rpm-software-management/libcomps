find_package (PythonLibs 3.0)
find_package (PythonInterp 3.0)
#add_custom_target(py3-copy)

EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "
from sys import stdout
from distutils import sysconfig
path=sysconfig.get_python_lib(True, prefix='${CMAKE_INSTALL_PREFIX}')
stdout.write(path)"
OUTPUT_VARIABLE PYTHON_INSTALL_DIR)

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${LIBCOMPS_INCLUDE_PATH})

configure_file(${pycomps_TESTSDIR}/run_tests.sh.in
               ./libcomps/run_tests.sh)

include(../../pycopy.cmake)

add_library(pycomps SHARED ${pycomps_SRC})
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
set_target_properties(pycomps PROPERTIES PREFIX "")
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")
add_dependencies(pycomps src-copy)

target_link_libraries(pycomps libcomps)
target_link_libraries(pycomps ${EXPAT_LIBRARIES})
target_link_libraries(pycomps ${LIBXML2_LIBRARIES})
target_link_libraries(pycomps ${PYTHON_LIBRARIES})

add_custom_target(pytest_run COMMAND ./run_tests.sh
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                   COMMENT "Running python binding tests")
add_dependencies(pycomps pycomps py3-copy)


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
