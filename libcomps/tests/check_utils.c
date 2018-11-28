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

#include "check_utils.h"

const char * pkg_type2str(COMPS_PackageType type) {
    switch(type) {
        case COMPS_PACKAGE_DEFAULT:
            return "default";
        case COMPS_PACKAGE_OPTIONAL:
            return "optional";
        case COMPS_PACKAGE_MANDATORY:
            return "mandatory";
        case COMPS_PACKAGE_CONDITIONAL:
            return "conditional";
        case COMPS_PACKAGE_UNKNOWN:
            return "unknown";
        break;
    }
    return NULL;
}

/*void print_list_item(COMPS_ListItem * it, unsigned int num, void * user_data)
{
    (void) user_data;
    printf("%d. %d\n", num, *((int*)it->data));
}*/

void print_package(COMPS_Object *obj) {
    char *str;
    str = comps_object_tostr((COMPS_Object*)
                             ((COMPS_DocGroupPackage*)obj)->name);
    printf("%s type=", str);
    free(str);
    printf("%s\n", pkg_type2str(((COMPS_DocGroupPackage*)obj)->type));
}

void print_gid(COMPS_Object *obj) {
    char *str;
    str = comps_object_tostr((COMPS_Object*)
                             ((COMPS_DocGroupId*)obj)->name);
    printf("%s default=", str);
    free(str);
    printf("%s\n",(((COMPS_DocGroupId*)obj)->def)?"true":"false");
}

void print_group(COMPS_Object *obj) {
    char *id, *name, *desc;
    int disp_ord, def, uservis;
    COMPS_Object *prop;

    prop = comps_docgroup_get_id((COMPS_DocGroup*)obj);
    id = (prop)?comps_object_tostr(prop):"NULL";
    prop = comps_docgroup_get_name((COMPS_DocGroup*)obj);
    name = (prop)?comps_object_tostr(prop):"NULL";
    prop = comps_docgroup_get_desc((COMPS_DocGroup*)obj);
    desc = (prop)?comps_object_tostr(prop):"NULL";
    prop = comps_docgroup_get_display_order((COMPS_DocGroup*)obj);
    disp_ord = (prop)?((COMPS_Num*)prop)->val:0;
    prop = comps_docgroup_get_uservisible((COMPS_DocGroup*)obj);
    uservis = (prop)?((COMPS_Num*)prop)->val:0;
    prop = comps_docgroup_get_def((COMPS_DocGroup*)obj);
    def = (prop)?((COMPS_Num*)prop)->val:0;
    
    printf("id: %s\n", id);
    printf("name: %s\n", name);
    printf("desc: %s\n", desc);
    printf("default: %d\n", def);
    printf("uservisible: %d\n", uservis);
    printf("display_order: %d\n", disp_ord);
    COMPS_ObjListIt *it = ((COMPS_DocGroup*)obj)->packages->first;
    while (comps_objlist_walk(&it, &prop)) {
        print_package(prop);
    }
}

void print_category(COMPS_Object *obj) {
    char *id, *name, *desc;
    int disp_ord;
    COMPS_Object *prop;

    prop = comps_doccategory_get_id((COMPS_DocCategory*)obj);
    id = (prop)?comps_object_tostr(prop):NULL;
    COMPS_OBJECT_DESTROY(prop);
    prop = comps_doccategory_get_name((COMPS_DocCategory*)obj);
    name = (prop)?comps_object_tostr(prop):NULL;
    COMPS_OBJECT_DESTROY(prop);
    prop = comps_doccategory_get_desc((COMPS_DocCategory*)obj);
    desc = (prop)?comps_object_tostr(prop):NULL;
    COMPS_OBJECT_DESTROY(prop);
    prop = comps_doccategory_get_display_order((COMPS_DocCategory*)obj);
    disp_ord = (prop)?((COMPS_Num*)prop)->val:0;
    COMPS_OBJECT_DESTROY(prop);
    
    printf("id: %s\n", id ? id : "(null)");
    printf("name: %s\n", name ? name : "(null)");
    printf("desc: %s\n", desc ? desc : "(null)");
    printf("display_order: %d\n", disp_ord);
    free(id);
    free(name);
    free(desc);
    COMPS_ObjListIt * it = ((COMPS_DocCategory*)obj)->group_ids->first;
    while (comps_objlist_walk(&it, &prop)) {
        print_gid(prop);
    }
}
