include (${CMAKE_SOURCE_DIR}/libcomps/version.cmake)
MATH( EXPR libcomps_RELEASE "${libcomps_RELEASE} +1")
configure_file(${CMAKE_SOURCE_DIR}/libcomps/version.cmake.in
               ${CMAKE_SOURCE_DIR}/libcomps/version.cmake)
