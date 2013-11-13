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

#include "comps_utils.h"
#include "comps_log.h"

void* __comps_str_clone(void *str) {
    char *ret;
    if (str == NULL)
        return NULL;
    ret = malloc(sizeof(char) * (strlen((char*)str)+1));
    if (ret==NULL) return NULL;
    memcpy(ret, str, sizeof(char) * (strlen((char*)str)+1));
    return ret;
}

inline char __comps_strcmp(void *s1, void *s2) {
    if (s1 == NULL && s2 == NULL) return 1;
    else if ((s1 == NULL && s2 != NULL) || (s1 != NULL && s2 == NULL)) return 0;
    return (strcmp((const char*)s1, (const char*)s2) == 0);
}

inline void __comps_xml_prop(char *key, char *val,
                             xmlTextWriterPtr writer) {
    xmlTextWriterStartElement(writer, BAD_CAST key);
    xmlTextWriterWriteString(writer, BAD_CAST val);
    xmlTextWriterEndElement(writer);
}

char* __comps_num2boolstr(COMPS_Object* obj) {
    char *ret;
    char *_bool;
    if (((COMPS_Num*)obj)->val) {
        _bool = "true";
    } else {
        _bool = "false";
    }
    ret = malloc(sizeof(char) * (strlen(_bool)+1));
    ret[0] = 0;
    strcat(ret, _bool);
    return ret;
}

unsigned int digits_count(unsigned int x) {
    //unsigned int ret = 1;
    if (x >= 100000000) return 9;
    if (x >= 10000000) return 8;
    if (x >= 1000000) return 7;
    if (x >= 100000) return 6;
    if (x >= 10000) return 5;
    if (x >= 1000) return 4;
    if (x >= 100) return 3;
    if (x >= 10) return 2;
    return 1;
}

signed char comps_id_check(COMPS_Object *obj, COMPS_Object *prop) {
    (void)obj;
    if (!prop)
        return 0;
    if (strcmp("", ((COMPS_Str*)prop)->val) == 0) return 0;
    return 1;
}

bool __comps_objlist_intersected(COMPS_ObjList *list1, COMPS_ObjList *list2) {
    for (COMPS_ObjListIt *it = list1->first; it != NULL; it = it->next) {
        for (COMPS_ObjListIt *it2 = list2->first; it2 != NULL; it2 = it2->next){
            if (comps_object_cmp(it->comps_obj, it2->comps_obj))
                return true;
        }
    }
    return false;
}

