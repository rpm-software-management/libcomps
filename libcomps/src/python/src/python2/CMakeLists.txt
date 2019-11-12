find_package (PythonLibs 2.6)
find_package (PythonInterp 2.6 REQUIRED)

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

add_library(pycomps SHARED ${pycomps_SRC})
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
set_target_properties(pycomps PROPERTIES PREFIX "_lib")
#set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")
#add_dependencies(pycomps src-copy)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_SKIP_BUILD_RPATH  TRUE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

target_link_libraries(pycomps libcomps)
target_link_libraries(pycomps ${EXPAT_LIBRARIES})
target_link_libraries(pycomps ${LIBXML2_LIBRARIES})
target_link_libraries(pycomps ${PYTHON_LIBRARIES})

set(pycopy pytest_run)

add_custom_target(pytest_run)
add_dependencies(pytest_run pycomps)

include(../../pycopy.cmake)

add_custom_command(TARGET pytest_run COMMAND ./run_tests.sh
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                   COMMENT "Running python binding tests")

add_dependencies(pytest pytest_run)

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

# Version has to be passed as last argument.
INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/../setup_for_python_metadata.py install_egg_info --install-dir \$ENV{DESTDIR}/${PYTHON_INSTALL_DIR} ${VERSION})")
