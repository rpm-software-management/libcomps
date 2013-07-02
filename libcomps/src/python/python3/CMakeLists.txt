find_package (PythonLibs 3.0)
find_package (PythonInterp 3.0)

execute_process (COMMAND ${PYTHON_EXECUTABLE} -c "from sys import stdout; from distutils import sysconfig; stdout.write(sysconfig.get_python_lib(True))" OUTPUT_VARIABLE PYTHON_INSTALL_DIR)

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${LIBCOMPS_INCLUDE_PATH})

configure_file(${pycomps_SRCDIR}/tests/run_tests.sh.in
               ${pycomps_SRCDIR}/tests/run_tests.sh)

FILE(COPY ${pycomps_SRCDIR}/__init__.py DESTINATION ./libcomps/)
FILE(COPY ${pycomps_SRCDIR}/tests/__test.py DESTINATION ./libcomps/)
FILE(COPY ${pycomps_SRCDIR}/tests/fedora_comps.xml DESTINATION ./libcomps/)
FILE(COPY ${pycomps_SRCDIR}/tests/sample_comps.xml DESTINATION ./libcomps/)
FILE(COPY ${pycomps_SRCDIR}/tests/main_comps.xml DESTINATION ./libcomps/)

add_library(pycomps SHARED ${pycomps_SRC})
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
set_target_properties(pycomps PROPERTIES PREFIX "")
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")

target_link_libraries(pycomps libcomps)
target_link_libraries(pycomps ${EXPAT_LIBRARIES})
target_link_libraries(pycomps ${LIBXML2_LIBRARIES})
target_link_libraries(pycomps ${PYTHON_LIBRARIES})

add_custom_target(test ALL COMMAND ./run_tests.sh
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                   COMMENT "Running python binding tests")

INSTALL(FILES ${pycomps_SRCDIR}/__init__.py DESTINATION ${PYTHON_INSTALL_DIR}/libcomps)
INSTALL(FILES ${pycomps_SRCDIR}/tests/__test.py DESTINATION
              ${PYTHON_INSTALL_DIR}/libcomps/)
INSTALL(FILES ${pycomps_SRCDIR}/tests/fedora_comps.xml DESTINATION
              ${PYTHON_INSTALL_DIR}/libcomps/)
INSTALL(FILES ${pycomps_SRCDIR}/tests/sample_comps.xml DESTINATION
              ${PYTHON_INSTALL_DIR}/libcomps/)
INSTALL(FILES ${pycomps_SRCDIR}/tests/main_comps.xml DESTINATION
              ${PYTHON_INSTALL_DIR}/libcomps/)
INSTALL (TARGETS pycomps LIBRARY DESTINATION ${PYTHON_INSTALL_DIR}/libcomps)
