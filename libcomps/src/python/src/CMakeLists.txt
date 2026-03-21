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

include_directories(${LIBCOMPS_INCLUDE_PATH})

if (ENABLE_TESTS)
    configure_file(${pycomps_TESTSDIR}/run_tests.sh.in ./libcomps/run_tests.sh)
endif ()

include(../pycopy.cmake)

IF (SKBUILD)
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
    Python_add_library(pycomps MODULE ${pycomps_SRC})
    set_target_properties(pycomps PROPERTIES OUTPUT_NAME "_libpycomps")
ELSE ()
    find_package (PythonInterp 3 EXACT REQUIRED)
    find_package (PythonLibs 3 EXACT)
    include_directories(${PYTHON_INCLUDE_DIR})

    add_library(pycomps SHARED ${pycomps_SRC})
    set_target_properties(pycomps PROPERTIES PREFIX "")
    set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_NAME "_libpycomps")
    # manylinux links statically, no libpythonX.so available
    # https://github.com/pypa/manylinux/issues/255#issuecomment-450761634
    target_link_libraries(pycomps PRIVATE ${PYTHON_LIBRARIES})
ENDIF ()

set_target_properties(pycomps PROPERTIES LIBRARY_OUTPUT_DIRECTORY "./libcomps")
add_dependencies(pycomps src-copy)

target_link_libraries(pycomps PRIVATE libcomps)
target_link_libraries(pycomps PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(pycomps PRIVATE ${LIBXML2_LIBRARIES})

if (ENABLE_TESTS)
    set(pycopy pytest_run)

    add_custom_target(pytest_run COMMAND ./run_tests.sh
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libcomps/
                      COMMENT "Running python binding tests")
    add_dependencies(pytest_run pycomps py3-copy)
endif ()

IF (SKBUILD)
    INSTALL(FILES libcomps/__init__.py DESTINATION libcomps)
    INSTALL(TARGETS pycomps LIBRARY DESTINATION libcomps)
ELSE ()
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "from sys import stdout; from sysconfig import get_path; stdout.write(get_path('platlib'))" OUTPUT_VARIABLE PYTHON_INSTALL_DIR)

    INSTALL(FILES ${pycomps_SRCDIR}/libcomps/__init__.py DESTINATION ${PYTHON_INSTALL_DIR}/libcomps)
    INSTALL (TARGETS pycomps LIBRARY DESTINATION ${PYTHON_INSTALL_DIR}/libcomps)
    # Install dist-info metadata so distro packaging tools can discover this package
    SET(_DIST_INFO_DIR "${PYTHON_INSTALL_DIR}/libcomps-${VERSION}.dist-info")
    INSTALL(CODE "
        file(MAKE_DIRECTORY \$ENV{DESTDIR}/${_DIST_INFO_DIR})
        file(WRITE \$ENV{DESTDIR}/${_DIST_INFO_DIR}/METADATA \"Metadata-Version: 2.1\\nName: libcomps\\nVersion: ${VERSION}\\n\")
        file(WRITE \$ENV{DESTDIR}/${_DIST_INFO_DIR}/INSTALLER \"cmake\\n\")
        file(WRITE \$ENV{DESTDIR}/${_DIST_INFO_DIR}/RECORD \"\")
    ")
ENDIF ()
