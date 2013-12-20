configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc-sources/conf.py.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/doc-sources/conf.py")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

#find_package(Doxygen)
#if(DOXYGEN_FOUND)
# configure_file(Doxyfile.user.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.user)
# add_custom_target (pydocs COMMAND ${DOXYGEN_EXECUTABLE}
#                         ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.user
#                         SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.user)
#endif(DOXYGEN_FOUND)

find_package(Sphinx)
add_custom_target (pydocs COMMAND ${SPHINX_EXECUTABLE} -E -b html
                                  "${CMAKE_CURRENT_SOURCE_DIR}/doc-sources/"
                                  "${CMAKE_CURRENT_BINARY_DIR}/html/}"
                          DEPENDS pycomps)
