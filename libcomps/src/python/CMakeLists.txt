macro(LIST_REPLACE _LIST INDEX VALUE)
    list(INSERT ${_LIST} ${INDEX} ${VALUE})
    MATH(EXPR __INDEX "${INDEX} + 1")
    list (REMOVE_AT ${_LIST} ${__INDEX})
endmacro(LIST_REPLACE)

set (pycomps_SRC pycomps.c pycomps_sequence.c
     pycomps_envs.c pycomps_categories.c pycomps_groups.c
     pycomps_gids.c pycomps_utils.c pycomps_dict.c pycomps_mdict.c
     pycomps_hash.c pycomps_exc.c pycomps_lbw.c)

set (pycomps_HEADERS pycomps_23macros.h pycomps_sequence.h
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

set (pycomps_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT PYTHON_DESIRED)
    set (PYTHON_DESIRED "2")
endif()
if (${PYTHON_DESIRED} STREQUAL "2")
    message("Building for python2")
    add_subdirectory(python2)
else()
    message("Building for python3")
    add_subdirectory(python3)
endif()
