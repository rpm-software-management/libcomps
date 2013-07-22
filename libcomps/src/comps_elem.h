/* libcomps - C alternative to yum.comps library
 * Copyright (C) 2013 Jindrich Luza
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to  Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA
 */

#ifndef COMPS_ELEM_H
#define COMPS_ELEM_H

#include <stdlib.h>

#include "comps_dict.h"

typedef enum {COMPS_ELEM_UNKNOWN,
                COMPS_ELEM_DOC,
                COMPS_ELEM_GROUP,
                COMPS_ELEM_ID,
                COMPS_ELEM_NAME,
                COMPS_ELEM_DESC,
                COMPS_ELEM_DEFAULT,
                COMPS_ELEM_LANGONLY,
                COMPS_ELEM_USERVISIBLE,
                COMPS_ELEM_PACKAGELIST,
                COMPS_ELEM_PACKAGEREQ,
                COMPS_ELEM_CATEGORY,
                COMPS_ELEM_GROUPLIST,
                COMPS_ELEM_GROUPID,
                COMPS_ELEM_DISPLAYORDER,
                COMPS_ELEM_ENV,
                COMPS_ELEM_OPTLIST,
                //COMPS_ELEM_OPTID,
                COMPS_ELEM_IGNOREDEP,
                COMPS_ELEM_WHITEOUT,
                COMPS_ELEM_BLACKLIST,
                COMPS_ELEM_PACKAGE,
                COMPS_ELEM_LANGPACKS,
                COMPS_ELEM_MATCH,
                COMPS_ELEM_NONE} COMPS_ElemType;

extern const char comps_xml_elems_required[];

typedef struct {
    char *name;
    char *val;
} COMPS_ElemAttr;

typedef struct {
    char *name;
    COMPS_Dict *attrs;
    COMPS_ElemType type;
} COMPS_Elem;

char * comps_elem_get_name(const COMPS_ElemType type);
void comps_elem_attr_destroy(void *attr);
COMPS_ElemAttr * comps_elem_attr_create(const char *name, const char *val);
COMPS_Elem* comps_elem_create(const char * s, const char ** attrs,
                              COMPS_ElemType type);
void comps_elem_destroy(void * elem);

#endif
