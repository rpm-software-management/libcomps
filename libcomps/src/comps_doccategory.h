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

#ifndef COMPS_DOCCATEGORY_H
#define COMPS_DOCCATEGORY_H

#include "comps_obj.h"
#include "comps_objdict.h"
#include "comps_objlist.h"
#include "comps_utils.h"
#include "comps_docgroupid.h"
#include "comps_validate.h"
#include "comps_radix.h"

#include <stddef.h>
#include <assert.h>

typedef struct {
    COMPS_Object_HEAD
    COMPS_ObjDict *properties;
    COMPS_ObjDict *name_by_lang;
    COMPS_ObjDict *desc_by_lang;
    COMPS_ObjList *group_ids;
} COMPS_DocCategory;

//HEAD_COMPS_CREATE_u(doccategory, COMPS_DocCategory)  /*comps_utils.h macro*/
//HEAD_COMPS_COPY_u(doccategory, COMPS_DocCategory)  /*comps_utils.h macro*/
//HEAD_COMPS_DESTROY_u(doccategory, COMPS_DocCategory)  /*comps_utils.h macro*/

HEAD_COMPS_STRPROP_SETTER(category, COMPS_DocCategory, id) /*comps_utils.h macro*/
HEAD_COMPS_STRPROP_SETTER(category, COMPS_DocCategory, name) /*comps_utils.h macro*/
HEAD_COMPS_STRPROP_SETTER(category, COMPS_DocCategory, desc) /*comps_utils.h macro*/
HEAD_COMPS_NUMPROP_SETTER(category, COMPS_DocCategory, display_order) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, id) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER_OBJ(category, id) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, name) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, desc) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, display_order) /*comps_utils.h macro*/

HEAD_COMPS_DOCOBJ_GETOBJLIST(doccategory, COMPS_DocCategory, group_ids, group_ids)
HEAD_COMPS_DOCOBJ_SETOBJLIST(doccategory, COMPS_DocCategory, group_ids, group_ids)

char __comps_doccategory_idcmp(void *c1, void *c2);
signed char comps_doccategory_cmp_u(COMPS_Object *cat1, COMPS_Object *cat2);

void comps_doccategory_add_groupid(COMPS_DocCategory *group,
                                   COMPS_DocGroupId *gid);

COMPS_DocCategory* comps_doccategory_union(COMPS_DocCategory *c1,
                                           COMPS_DocCategory *c2);
COMPS_DocCategory* comps_doccategory_intersect(COMPS_DocCategory *c1,
                                               COMPS_DocCategory *c2);

signed char comps_doccategory_xml(COMPS_DocCategory *category,
                                  xmlTextWriterPtr writer,
                                  COMPS_Log *log);

extern COMPS_ObjectInfo COMPS_DocCategory_ObjInfo;

#endif
