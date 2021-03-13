set (libcomps_SOURCES comps_doc.c comps_docgroup.c comps_doccategory.c
                      comps_docenv.c comps_docpackage.c comps_docgroupid.c
     comps_obj.c comps_mm.c
     #comps_list.c
     comps_hslist.c comps_dict.c
     comps_objradix.c comps_objmradix.c comps_objdict.c comps_objlist.c
     comps_elem.c comps_radix.c comps_mradix.c comps_bradix.c comps_set.c
     comps_parse.c comps_log.c comps_default.c
     comps_utils.c comps_validate.c
     comps_log_codes.c
     comps_types.c
     )
set (libcomps_HEADERS comps_doc.h comps_docgroup.h comps_doccategory.h
                      comps_docenv.h comps_docpackage.h comps_docgroupid.h
     comps_obj.h comps_mm.h
     #comps_list.h
     comps_hslist.h comps_dict.h
     comps_objradix.h comps_objmradix.h comps_objdict.h comps_objlist.h
     comps_elem.h comps_radix.h comps_mradix.h comps_bradix.h comps_set.h
     comps_parse.h comps_log.h comps_default.h
     comps_utils.h comps_validate.h
     comps_log_codes.h
    )
add_custom_target(src-copy)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/libcomps_config.h.in"
                "${PROJECT_SOURCE_DIR}/src/libcomps/libcomps_config.h")

include_directories(${LIBXML2_INCLUDE_DIR})
file(GLOB files "${PROJECT_SOURCE_DIR}/src/*.h")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/libcomps")

foreach(file ${files})
    add_custom_command(TARGET src-copy PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
                copy ${file} "${PROJECT_SOURCE_DIR}/src/libcomps/")
    #file(COPY ${file} DESTINATION "${PROJECT_SOURCE_DIR}/src/libcomps/")
endforeach()


if (BUILD_LIBCOMPS_SHARED)
  set (libcomps_library_type SHARED)
else ()
  set (libcomps_library_type STATIC)
endif ()
add_library(libcomps ${libcomps_library_type} ${libcomps_SOURCES})
target_link_libraries(libcomps ${EXPAT_LIBRARY})
target_link_libraries(libcomps ${LIBXML2_LIBRARIES})
target_link_libraries(libcomps ${ZLIB_LIBRARIES})
target_link_libraries(libcomps m)
set_target_properties(libcomps PROPERTIES OUTPUT_NAME "comps")
set_target_properties(libcomps PROPERTIES SOVERSION ${libcomps_VERSION_MAJOR})

add_dependencies(libcomps src-copy)

if (ENABLE_DEVELOPMENT OR "${libcomps_library_type}" STREQUAL "SHARED")
    install (
        TARGETS libcomps
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RuntimeLibraries
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RuntimeLibraries
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    )
endif ()

if (ENABLE_DEVELOPMENT)
  configure_file("${PROJECT_SOURCE_DIR}/../libcomps.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libcomps.pc" @ONLY)

  install (FILES ${libcomps_HEADERS} DESTINATION include/libcomps)
  install (TARGETS libcomps LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install (FILES "${CMAKE_CURRENT_BINARY_DIR}/libcomps.pc"
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig/")
endif ()

add_subdirectory(python)

