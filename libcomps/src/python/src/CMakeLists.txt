macro(LIST_REPLACE _LIST INDEX VALUE)
    list(INSERT ${_LIST} ${INDEX} ${VALUE})
    MATH(EXPR __INDEX "${INDEX} + 1")
    list (REMOVE_AT ${_LIST} ${__INDEX})
endmacro(LIST_REPLACE)

set (pycomps_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR})

set (pycomps_SRC pycomps.c pycomps_sequence.c
     pycomps_envs.c pycomps_categories.c pycomps_groups.c
     pycomps_gids.c pycomps_utils.c pycomps_dict.c pycomps_mdict.c
     pycomps_hash.c pycomps_exc.c pycomps_lbw.c)

set (pycomps_HEADERS pycomps_23macros.h pycomps_sequence.h
     pycomps_envs.h pycomps_categories.h pycomps_groups.h
     pycomps_gids.h pycomps_utils.h pycomps_dict.h pycomps_mdict.h
     pycomps_hash.h pycomps_exc.h pycomps_lbw.h
     pycomps_types.h)

#set(TEST_FILES ../__init__.py __test.py test_merge_comps.py test_libcomps.py
#               utest.py)
#
#set(TEST_COMPS  fedora_comps.xml sample_comps.xml
#                sample_comps2.xml sample_comps2.xml.gz main_comps.xml
#                main_comps2.xml main_arches.xml f21-rawhide-comps.xml
#                comps-f21.xml.in
#                comps/comps_part1.xml  comps/comps_part2.xml
#                comps/merged_comps_1_2.xml  comps/merged_comps_2_1.xml
#                comps/comps-f21.xml)

list (LENGTH pycomps_SRC pycomps_SRCLEN)
math (EXPR pycomps_SRCLEN "${pycomps_SRCLEN} - 1")
foreach(i RANGE 0 ${pycomps_SRCLEN})
    list (GET pycomps_SRC ${i} val)
    LIST_REPLACE(pycomps_SRC ${i} "${CMAKE_CURRENT_SOURCE_DIR}/${val}")
endforeach(i)

list (LENGTH pycomps_HEADERS pycomps_SRCLEN)
math (EXPR pycomps_SRCLEN "${pycomps_SRCLEN} - 1")
foreach(i RANGE 0 ${pycomps_SRCLEN})
    list (GET pycomps_HEADERS ${i} val)
    LIST_REPLACE(pycomps_HEADERS ${i} "${CMAKE_CURRENT_SOURCE_DIR}/${val}")
endforeach(i)

if (${pversion} EQUAL 2)
    find_package (PythonLibs 2.6)
    find_package (PythonInterp 2.6 REQUIRED)
else ()
    find_package (PythonLibs 3.0)
    find_package (PythonInterp 3.0)
    #add_custom_target(py3-copy)
endif ()

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

if (${pversion} EQUAL 2)
    add_library(pycomps SHARED ${pycomps_SRC})
    set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
    set_target_properties(pycomps PROPERTIES PREFIX "_lib")
    #set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")
    #add_dependencies(pycomps src-copy)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    SET(CMAKE_SKIP_BUILD_RPATH  TRUE)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
else ()
    include(../pycopy.cmake)

    add_library(pycomps SHARED ${pycomps_SRC})
    set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
    set_target_properties(pycomps PROPERTIES PREFIX "")
    set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")
    add_dependencies(pycomps src-copy)
endif ()

target_link_libraries(pycomps libcomps)
target_link_libraries(pycomps ${EXPAT_LIBRARIES})
target_link_libraries(pycomps ${LIBXML2_LIBRARIES})
target_link_libraries(pycomps ${PYTHON_LIBRARIES})

if (${pversion} EQUAL 2)
    set(pycopy pytest_run)

    add_custom_target(pytest_run)
    add_dependencies(pytest_run pycomps)

    include(../../pycopy.cmake)

    add_custom_command(TARGET pytest_run COMMAND ./run_tests.sh
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                    COMMENT "Running python binding tests")
endif ()

add_custom_target(pytest_run COMMAND ./run_tests.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                COMMENT "Running python binding tests")

if (${pversion} EQUAL 2)
    add_dependencies(pytest pytest_run)
else ()
    add_dependencies(pycomps pycomps py3-copy)
endif ()

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
