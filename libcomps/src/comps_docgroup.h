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

#ifndef COMPS_DOCGROUP_H
#define COMPS_DOCGROUP_H

#include "comps_utils.h"
#include "comps_obj.h"
#include "comps_objdict.h"
#include "comps_objlist.h"
#include "comps_docpackage.h"

#include <stddef.h>
#include <assert.h>

typedef struct {
    COMPS_Object_HEAD
    COMPS_ObjDict *properties;
    COMPS_ObjDict *name_by_lang;
    COMPS_ObjDict *desc_by_lang;
    COMPS_ObjList *packages;
} COMPS_DocGroup;

HEAD_COMPS_CREATE_u(docgroup, COMPS_DocGroup)  /*comps_utils.h macro*/
HEAD_COMPS_COPY_u(docgroup, COMPS_DocGroup)  /*comps_utils.h macro*/
HEAD_COMPS_DESTROY_u(docgroup, COMPS_DocGroup)  /*comps_utils.h macro*/

HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, id) /*comps_utils.h macro*/
HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, name) /*comps_utils.h macro*/
HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, desc) /*comps_utils.h macro*/
HEAD_COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, def) /*comps_utils.h macro*/
HEAD_COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, uservisible) /*comps_utils.h macro*/
HEAD_COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, display_order) /*comps_utils.h macro*/
HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, langonly) /*comps_utils.h macro*/

signed char comps_docgroup_cmp_u(COMPS_Object *group1, COMPS_Object *group2);

void comps_docgroup_add_package(COMPS_DocGroup *group,
                                COMPS_DocGroupPackage *package);

COMPS_DocGroup* comps_docgroup_union(COMPS_DocGroup *g1, COMPS_DocGroup *g2);

void comps_docgroup_xml(COMPS_DocGroup *group, xmlTextWriterPtr writer,
                        COMPS_Logger *log);

extern COMPS_ObjectInfo COMPS_DocGroup_ObjInfo;

#endif
