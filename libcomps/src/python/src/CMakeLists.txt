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

set (pycomps_HEADERS pycomps_macros.h pycomps_sequence.h
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

if (NOT SKBUILD)
    find_package (PythonInterp 3 EXACT REQUIRED)
    find_package (PythonLibs 3 EXACT)
endif(NOT SKBUILD)

include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${LIBCOMPS_INCLUDE_PATH})

if (ENABLE_TESTS)
    configure_file(${pycomps_TESTSDIR}/run_tests.sh.in ./libcomps/run_tests.sh)
endif ()

include(../pycopy.cmake)

add_library(pycomps SHARED ${pycomps_SRC})
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
set_target_properties(pycomps PROPERTIES PREFIX "")
set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")
add_dependencies(pycomps src-copy)

target_link_libraries(pycomps libcomps)
target_link_libraries(pycomps ${EXPAT_LIBRARIES})
target_link_libraries(pycomps ${LIBXML2_LIBRARIES})

if (NOT SKBUILD)
    # manylinux links statically, no libpythonX.so available
    # https://github.com/pypa/manylinux/issues/255#issuecomment-450761634
    target_link_libraries(pycomps ${PYTHON_LIBRARIES})
endif (NOT SKBUILD)

if (ENABLE_TESTS)
    set(pycopy pytest_run)

    add_custom_target(pytest_run COMMAND ./run_tests.sh
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                      COMMENT "Running python binding tests")
    add_dependencies(pytest_run pycomps py3-copy)
endif ()

IF (SKBUILD)
    INSTALL(FILES libcomps/__init__.py DESTINATION libcomps/src/python/src/libcomps)
    INSTALL(TARGETS pycomps LIBRARY DESTINATION libcomps/src/python/src/libcomps)
ELSE ()
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "from sys import stdout; from sysconfig import get_path; stdout.write(get_path('platlib'))" OUTPUT_VARIABLE PYTHON_INSTALL_DIR)

    INSTALL(FILES ${pycomps_SRCDIR}/libcomps/__init__.py DESTINATION ${PYTHON_INSTALL_DIR}/libcomps)
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
    INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/../setup_for_python_metadata.py install_egg_info --install-dir \$ENV{DESTDIR}/${PYTHON_INSTALL_DIR}/ ${VERSION})")
ENDIF (SKBUILD)
