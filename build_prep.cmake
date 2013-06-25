include (${CMAKE_SOURCE_DIR}/libcomps/version.cmake)

set(VERSION ${libcomps_VERSION_MAJOR}.${libcomps_VERSION_MINOR}.${libcomps_VERSION_PATCH})

exec_program("git" ARGS rev-parse --short HEAD OUTPUT_VARIABLE GITREV)
exec_program("git" ARGS rev-parse HEAD OUTPUT_VARIABLE GITREVLONG)


set(GITARG archive ${GITREV} "--format=zip" "--prefix=libcomps-${GITREVLONG}/")
set(GITCMD "git")
execute_process(COMMAND ${GITCMD} ${GITARG} OUTPUT_FILE "libcomps-${GITREVLONG}.tar.gz")

configure_file(libcomps.spec.in libcomps.spec)

