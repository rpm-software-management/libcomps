#set (testlist_SOURCE check_list.c check_utils.c)
#set (testlist_HEADERS check_list.h check_utils.h)
add_custom_target(test-copy)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set (testobjrtree_SOURCE check_objrtree.c check_utils.c)
set (testobjrtree_HEADERS check_objrtree.h check_utils.h)

set (testrtree_SOURCE check_rtree.c check_utils.c)
set (testrtree_HEADERS check_rtree.h check_utils.h)

set (testbrtree_SOURCE check_brtree.c check_utils.c)
set (testbrtree_HEADERS check_brtree.h check_utils.h)

set (testparse_SOURCE check_parse.c check_utils.c)
set (testparse_HEADERS check_utils.h)

set (testcomps_SOURCE check_comps.c)

set (testvalidate_SOURCE check_validate.c)

#add_executable(test_list ${testlist_SOURCE})
add_executable(test_rtree ${testrtree_SOURCE})
add_executable(test_objrtree ${testobjrtree_SOURCE})
add_executable(test_brtree ${testbrtree_SOURCE})
#add_executable(test_mrtree ${testmrtree_SOURCE})
add_executable(test_parse ${testparse_SOURCE})
add_executable(test_comps ${testcomps_SOURCE})
add_executable(test_validate ${testvalidate_SOURCE})

#target_link_libraries(test_list libcomps)
#target_link_libraries(test_list ${CHECK_LIBRARY})

target_link_libraries(test_brtree libcomps)
target_link_libraries(test_brtree ${CHECK_LIBRARY})

target_link_libraries(test_rtree libcomps)
target_link_libraries(test_rtree ${CHECK_LIBRARY})

target_link_libraries(test_objrtree libcomps)
target_link_libraries(test_objrtree ${CHECK_LIBRARY})

target_link_libraries(test_validate libcomps)
target_link_libraries(test_validate ${CHECK_LIBRARY})

#target_link_libraries(test_rtree2 libcomps)
#target_link_libraries(test_rtree2 ${CHECK_LIBRARY})

#target_link_libraries(test_mrtree libcomps)
#target_link_libraries(test_mrtree ${CHECK_LIBRARY})

target_link_libraries(test_parse libcomps)
target_link_libraries(test_parse expat)
target_link_libraries(test_parse ${CHECK_LIBRARY})

target_link_libraries(test_comps expat)
target_link_libraries(test_comps ${CHECK_LIBRARY})
target_link_libraries(test_comps libcomps)
set_target_properties(test_comps PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -g")

add_dependencies(test_comps test-copy)
add_dependencies(test_parse test-copy)
add_dependencies(test_validate test-copy)


set(TEST_FILES fedora_comps.xml sample-comps.xml sample_comps.xml
               sample_comps_bad1.xml sample_comps_bad2.xml sample_comps_bad3.xml
               sample-bad-elem.xml comps.dtd dict-test.txt main_comps2.xml
               main_arches.xml f21-rawhide-comps.xml)
foreach(file ${TEST_FILES})
    add_custom_command(TARGET test-copy PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
                        copy ${CMAKE_CURRENT_SOURCE_DIR}/${file} ./)
    #file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${file} DESTINATION ./)
endforeach()

add_custom_target(test_comps_run
                   COMMAND export LD_LIBRARY_PATH="${LIBCOMPS_OUT}/:$LD_LIBRARY_PATH"
                           && ./test_comps
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS test_comps
                   COMMENT "Running comps test")

add_custom_target(test_parse_run
                   COMMAND export LD_LIBRARY_PATH="${LIBCOMPS_OUT}/:$LD_LIBRARY_PATH"
                           && ./test_parse
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS test_parse
                   COMMENT "Running comps_parse test")

add_dependencies(ctest test_comps_run test_parse_run)
