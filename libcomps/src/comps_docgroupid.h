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

#ifndef COMPS_DOCGROUPID_H
#define COMPS_DOCGROUPID_H

#include "comps_utils.h"

typedef struct {
    COMPS_Object_HEAD
    COMPS_Str *name;
    char def;
} COMPS_DocGroupId;

HEAD_COMPS_CREATE_u(docgroupid, COMPS_DocGroupId)  /*comps_utils.h macro*/
HEAD_COMPS_COPY_u(docgroupid, COMPS_DocGroupId)  /*comps_utils.h macro*/
HEAD_COMPS_DESTROY_u(docgroupid, COMPS_DocGroupId)  /*comps_utils.h macro*/

char __comps_docgroupid_cmp_set(void *gid1, void *gid2);

COMPS_Object* comps_docgroupid_get_name(COMPS_DocGroupId *gid);
void comps_docgroupid_set_name(COMPS_DocGroupId *gid, char *name, char copy);

void comps_docgroupid_set_name(COMPS_DocGroupId *gid, char *name, char copy);
void comps_docgroupid_set_default(COMPS_DocGroupId *gid, int def);

extern COMPS_ObjectInfo COMPS_DocGroupId_ObjInfo;

#endif
