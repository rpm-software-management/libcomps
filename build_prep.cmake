MACRO(LIST_CONTAINS var value)
    SET(${var} FALSE)
    FOREACH (value2 ${ARGN})
        IF (${value} STREQUAL ${value2})
            SET(${var} TRUE)
        ENDIF (${value} STREQUAL ${value2})
    ENDFOREACH (value2 ${ARGN})
ENDMACRO(LIST_CONTAINS)

MACRO(FORMAT_LOG_ENTRY formated entry)
    string(REPLACE "\n" ";" entry_list ${entry})
    set(FMT_FINAL "")
    foreach(line ${entry_list})
        string(REGEX MATCH "^[ \n\t]+$" matched ${line})
        if (NOT ${line} STREQUAL "\n")
            string(REGEX REPLACE "^([^-])(.+)" "- \\1\\2" fmt_line ${line})
            set(FMT_FINAL "${FMT_FINAL}${fmt_line}\n")
        else()
        endif(NOT ${line} STREQUAL "\n")
    endforeach(line ${entry_list})
    set(formated ${FMT_FINAL})
ENDMACRO(FORMAT_LOG_ENTRY)

MACRO(MAKE_TAG_LOGENTRY tag_log_entry tag last_tag)
    exec_program("git" ARGS log ${tag} -n 1 --format="format:%ci%n"
                        OUTPUT_VARIABLE date)
    exec_program("date" ARGS -d"${date}" "\"+%a %b %d %Y\""
                        OUTPUT_VARIABLE DATE)
    exec_program("git" ARGS log ${tag} -n 1 --date=short
                            --format="format: %cn <%ce>%n"
                        OUTPUT_VARIABLE AUTHOR)
    exec_program("git" ARGS log ${tag} -n 1 --date=short
                            --format="format:%B%n"
                        OUTPUT_VARIABLE LOG)
    FORMAT_LOG_ENTRY(FMT_FINAL ${LOG})

    string(REPLACE "\n" "" mtl_tag_stripped ${tag})
    string(REPLACE "\n" "" mtl_lasttag_stripped ${last_tag})
    execute_process(COMMAND "git" describe --tags --exact-match ${mtl_tag_stripped}
                        OUTPUT_VARIABLE describe_out ERROR_VARIABLE describe_err)

    if (NOT ${describe_err} STREQUAL "")
        #message("notag ${describe_err} ${describe_out}")
        unset(describe_out)
        execute_process(COMMAND "git" describe --tags --exact-match ${mtl_lasttag_stripped}
                            OUTPUT_VARIABLE describe_out)
        execute_process(COMMAND "git" rev-parse --short ${mtl_tag_stripped}
                            OUTPUT_VARIABLE suffix)
        set(describe_out "${describe_out}-${suffix}")
    endif(NOT ${describe_err} STREQUAL "")
    string(REPLACE "\n" "" TAG ${describe_out})

    unset(describe_err)
    unset(describe_out)

    string(REGEX REPLACE "^[la-z]+(-)?" "" mtl_tag_fmt ${TAG})
    string(REPLACE "\n" "" mtl_tag_out ${tag_out})

    set(tag_log_entry "* ${DATE} ${AUTHOR} - ${mtl_tag_fmt}\n${FMT_FINAL}")
ENDMACRO(MAKE_TAG_LOGENTRY)

MACRO(MAKE_LOGENTRY log_entry commit)
    exec_program("git" ARGS log ${commit} -n 1 --date=short
                            --format="format:%B%n"
                        OUTPUT_VARIABLE LOG)
    FORMAT_LOG_ENTRY(FMT_FINAL ${LOG})
    set(log_entry "${FMT_FINAL}")
ENDMACRO(MAKE_LOGENTRY)

include (${CMAKE_SOURCE_DIR}/libcomps/version.cmake)

SET(changelog_commits 946584c5a01d83bf9ec4c26d3f9d73e37bfb5456;
                      76764059f1085abbd0eeaeab64f45ec48a58ba4d;
                      4e16e2a76d3ea6f3fd4d162b177fd8e45a641bf9;
                      9a3e8428587ecb787d0febcf11d299f4c62488ed;
                      5500d1275ce9288a763c957a23525aae25f283fb;
                      e401de99222bac95e61b10836b230367a92f5e4f;
                      f243b574c801244447b873c8e77b82b3318c09d4;
                      5500d1275ce9288a763c957a23525aae25f283fb;
                      997834ec69b72ded9acbd785502c5f3bbf888be8)
set(CHANGELOG "")

execute_process(COMMAND "git" rev-parse --tags
                    OUTPUT_VARIABLE tags)
string(REPLACE "\n" ";" tags ${tags})
execute_process(COMMAND "git" "rev-parse" ${TOP_COMMIT}
                    OUTPUT_VARIABLE top_commit)
set(tags ${tags} "${top_commit}")
list(GET tags 0 last)
string(REPLACE "\n" "" last_tag ${last})

foreach(tag ${tags})
    string(REPLACE "\n" "" tag_out ${tag})
    MAKE_TAG_LOGENTRY(tag_log_entry ${tag_out} ${last_tag})
    execute_process(COMMAND "git" "rev-list" ${tag_out}
                        OUTPUT_VARIABLE commits)
    string(REPLACE "\n" ";" commits ${commits})
    foreach(commit ${commits})
        LIST_CONTAINS(contains1 ${commit} ${tags})
        LIST_CONTAINS(contains2 ${commit} ${changelog_commits})
        if (NOT contains1 AND contains2)
            MAKE_LOGENTRY(log_entry ${commit})
            set(tag_log_entry ${tag_log_entry}${log_entry})
        endif(NOT contains1 AND contains2)
    endforeach(commit ${commits})

    set(CHANGELOG "${tag_log_entry}\n${CHANGELOG}")
    set(last_tag ${tag_out})
endforeach(tag ${tags})

IF (CMAKE_SIZEOF_VOID_P MATCHES "8")
    SET (LIB_SUFFIX "64")
ENDIF (CMAKE_SIZEOF_VOID_P MATCHES "8")

set(VERSION ${libcomps_VERSION_MAJOR}.${libcomps_VERSION_MINOR}.${libcomps_VERSION_PATCH})

exec_program("git" ARGS rev-parse --short HEAD OUTPUT_VARIABLE GITREV)
exec_program("git" ARGS rev-parse HEAD OUTPUT_VARIABLE GITREVLONG)


set(GITARG archive ${GITREV} "--format=zip" "--prefix=libcomps-${GITREVLONG}/")
set(GITCMD "git")
execute_process(COMMAND ${GITCMD} ${GITARG} OUTPUT_FILE "libcomps-${GITREVLONG}.tar.gz")

configure_file(libcomps.spec.in libcomps.spec)
configure_file(libcomps.pc.in libcomps.pc @ONLY)

