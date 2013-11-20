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
    groupid->arches = NULL;
}
COMPS_CREATE_u(docgroupid, COMPS_DocGroupId)

void comps_docgroupid_copy(COMPS_DocGroupId *gid_dst,
                           COMPS_DocGroupId *gid_src) {
    gid_dst->name = (COMPS_Str*)comps_object_copy((COMPS_Object*)gid_src->name);
    //gid_dst->arches = (COMPS_ObjList*)comps_object_copy((COMPS_Object*)gid_src->arches);
    gid_dst->arches = NULL;
    gid_dst->def = gid_src->def;
}
COMPS_COPY_u(docgroupid, COMPS_DocGroupId)    /*comps_utils.h macro*/

void comps_docgroupid_destroy(COMPS_DocGroupId *gid) {
    COMPS_OBJECT_DESTROY(gid->name);
    COMPS_OBJECT_DESTROY(gid->arches);
}
COMPS_DESTROY_u(docgroupid, COMPS_DocGroupId) /*comps_utils.h macro*/


COMPS_Object* comps_docgroupid_get_name(COMPS_DocGroupId *gid) {
    return comps_object_incref((COMPS_Object*)gid->name);
}
void comps_docgroupid_set_name(COMPS_DocGroupId *gid, char *name, char copy) {
    (void)copy;
    if (gid->name)
        COMPS_OBJECT_DESTROY(gid->name);
    gid->name = comps_str(name);
}

COMPS_Object* comps_docgroupid_get_default(COMPS_DocGroupId *gid) {
    return (COMPS_Object*)comps_num(gid->def);
}
void comps_docgroupid_set_default(COMPS_DocGroupId *gid, int def) {
    gid->def = (def != 0);
}

COMPS_ObjList* comps_docgroupid_arches(COMPS_DocGroupId *gid) {
    return (COMPS_ObjList*)comps_object_incref((COMPS_Object*)gid->arches);
}
void comps_docgroupid_set_arches(COMPS_DocGroupId *gid,
                                 COMPS_ObjList *arches) {
    COMPS_OBJECT_DESTROY(gid->arches);
    gid->arches = arches;
}

signed char comps_docgroupid_cmp_u(COMPS_Object *gid1, COMPS_Object *gid2) {
    #define _gid1 ((COMPS_DocGroupId*)gid1)
    #define _gid2 ((COMPS_DocGroupId*)gid2)

    if (!COMPS_OBJECT_CMP(_gid1->name, _gid2->name)) return 0;
    if (_gid1->def != _gid2->def) return 0;
    return 1;

    #undef _gid1
    #undef _gid2
}

char __comps_docgroupid_cmp_set(void *gid1, void *gid2) {
    return comps_docgroupid_cmp_u((COMPS_Object*)gid1,
                                  (COMPS_Object*)gid2);
}
char* comps_docgroupid_str_u(COMPS_Object* docgroupid) {
    const int len = strlen("<COMPS_DocGroupId name='' default=''>");
    char *name = comps_object_tostr((COMPS_Object*)((COMPS_DocGroupId*)docgroupid)->name);
    char *def = ((COMPS_DocGroupId*)docgroupid)->def?"True":"False";
    char *ret = malloc(sizeof(char)*(len+strlen(name)+strlen(def)+1));
    sprintf(ret, "<COMPS_DocGroupId name='%s' default='%s'>", name, def);
    free(name);
    return ret;
}

signed char comps_docgroupid_xml(COMPS_DocGroupId *groupid,
                                  xmlTextWriterPtr writer,
                                  COMPS_Log *log, COMPS_XMLOptions *options) {
    char *str;
    bool default_def = false;
    int ret;

    ret = xmlTextWriterStartElement(writer, BAD_CAST "groupid");
    if (options->arch_output) {
        COMPS_Object *obj = (COMPS_Object*)groupid->arches;
        str = __comps_xml_arch_str(obj);
        ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "_arch",
                                          BAD_CAST str);
        free(str);
        COMPS_XMLRET_CHECK
    }
    COMPS_XMLRET_CHECK
    if (options->gid_default_explicit) {
        if (groupid->def)
            ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                              BAD_CAST "true");
        else
            ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                              BAD_CAST "false");
        COMPS_XMLRET_CHECK
    } else if (groupid->def != default_def) {
        if (groupid->def)
            ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                              BAD_CAST "true");
        else
            ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                              BAD_CAST "false");
    }
    str = comps_object_tostr((COMPS_Object*)groupid->name);
    ret = xmlTextWriterWriteString(writer, BAD_CAST str);
    COMPS_XMLRET_CHECK
    free(str);
    ret = xmlTextWriterEndElement(writer);
    COMPS_XMLRET_CHECK
    return 0;
}

COMPS_ObjectInfo COMPS_DocGroupId_ObjInfo = {
    .obj_size = sizeof(COMPS_DocGroupId),
    .constructor = &comps_docgroupid_create_u,
    .destructor = &comps_docgroupid_destroy_u,
    .copy = &comps_docgroupid_copy_u,
    .obj_cmp = &comps_docgroupid_cmp_u,
    .to_str = &comps_docgroupid_str_u
};

