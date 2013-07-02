find_package(Doxygen)
if(DOXYGEN_FOUND)
 configure_file(Doxyfile.user.in
                ${PROJECT_BINARY_DIR}/docs/Doxyfile.user)
 add_custom_target (docs COMMAND ${DOXYGEN_EXECUTABLE}
                         ${PROJECT_BINARY_DIR}/docs/Doxyfile.user
                         SOURCES ${PROJECT_BINARY_DIR}/docs/Doxyfile.user)
endif(DOXYGEN_FOUND)
#message("dir ",${PROJECT_BINARY_DIR}})
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/docs/libcomps-py-doc")

#add_custom_target(Docs-sphinx ALL
#                            COMMAND sphinx-build -b html . sphinx/build/html
#                                WORKING_DIRECTORY .
#                                DEPENDS Docs
#                                COMMENT "sphinx docs"
#)
