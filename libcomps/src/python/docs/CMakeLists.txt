configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc-sources/conf.py.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/doc-sources/conf.py")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


#include(../pycopy.cmake)

#find_package(Doxygen)
#if(DOXYGEN_FOUND)
# configure_file(Doxyfile.user.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.user)
# add_custom_target (pydocs COMMAND ${DOXYGEN_EXECUTABLE}
#                         ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.user
#                         SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.user)
#endif(DOXYGEN_FOUND)

find_package(Sphinx)

set(pycopy pydocs)
add_custom_target(pydocs)
add_dependencies(pydocs pycomps)

include(../pycopy.cmake)
add_custom_command(TARGET pydocs PRE_BUILD COMMAND set -E $ENV{LD_LIBRARY_PATH} "${LIBCOMPS_OUT}:$ENV{LD_LIBRARY_PATH}")

add_custom_command(TARGET pydocs POST_BUILD
                                 COMMAND ${PYTHON_EXECUTABLE} ${SPHINX_EXECUTABLE} -E -b html
                                  "${CMAKE_CURRENT_SOURCE_DIR}/doc-sources/"
                                  "${CMAKE_CURRENT_BINARY_DIR}/html/"
                                 COMMENT "LDLP $ENV{LD_LIBRARY_PATH}")
