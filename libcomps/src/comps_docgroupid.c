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

#include "comps_docgroupid.h"
void comps_docgroupid_create(COMPS_DocGroupId* groupid, COMPS_Object **args) {

    (void)args;
    groupid->name = NULL;
    groupid->def = 0;
}
COMPS_CREATE_u(docgroupid, COMPS_DocGroupId)

void comps_docgroupid_copy(COMPS_DocGroupId *gid_dst,
                                         COMPS_DocGroupId *gid_src) {
    gid_dst->name = (COMPS_Str*)comps_object_copy((COMPS_Object*)gid_src->name);
    gid_dst->def = gid_src->def;
}
COMPS_COPY_u(docgroupid, COMPS_DocGroupId)    /*comps_utils.h macro*/

void comps_docgroupid_destroy(COMPS_DocGroupId *gid) {
    free(gid->name);
}
COMPS_DESTROY_u(docgroupid, COMPS_DocGroupId) /*comps_utils.h macro*/


void comps_docgroupid_set_name(COMPS_DocGroupId *gid, char *name) {
    if (gid->name)
        comps_object_destroy((COMPS_Object*)gid->name);
    gid->name = comps_str(name);
}
void comps_docgroupid_set_default(COMPS_DocGroupId *gid, int def) {
    gid->def = (def != 0);
}

char __comps_docgroupid_cmp_set(void *gid1, void *gid2) {
    if (!comps_object_cmp(((COMPS_DocGroupId*)gid1)->name,
                          ((COMPS_DocGroupId*)gid2)->name)) return 0;
    if (((COMPS_DocGroupId*)gid1)->type != ((COMPS_DocGroupId*)gid2)->type)
        return 0;
    return 1;
}

signed char comps_docgroupid_cmp_u(COMPS_Object *gid1, COMPS_Object *gid2) {
    #define _gid1 ((COMPS_DocGroupId*)gid1)
    #define _gid2 ((COMPS_DocGroupId*)gid2)

    if (!comps_object_cmp(_gid1->name, _gid2->name)) return 0;
    if (_gid1->def != _gid2->def) return 0;
    return 1;

    #undef _gid1
    #undef _gid2
}

extern COMPS_ObjectInfo COMPS_DocGroupId_ObjInfo = {
    .obj_size = sizeof(COMPS_DocGroupId),
    .constructor = &comps_docgroupid_create_u,
    .destructor = &comps_docgroupid_destroy_u,
    .deep_copy = &comps_docgroupid_copy_u,
    .obj_cmp = &comps_docgroupid_cmp_u
};

