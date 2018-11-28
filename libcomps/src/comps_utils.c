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

inline int __comps_xml_prop(char *key, char *val,
                             xmlTextWriterPtr writer) {
    int retc;
    retc = xmlTextWriterStartElement(writer, BAD_CAST key) >= 0 ? 1 : 0;
    retc &= xmlTextWriterWriteString(writer, BAD_CAST val) >= 0 ? 1 : 0;
    retc &= xmlTextWriterEndElement(writer) >= 0 ? 1 : 0;
    return retc;
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


bool __comps_objlist_intersected(COMPS_ObjList *list1, COMPS_ObjList *list2) {
    for (COMPS_ObjListIt *it = list1->first; it != NULL; it = it->next) {
        for (COMPS_ObjListIt *it2 = list2->first; it2 != NULL; it2 = it2->next){
            if (comps_object_cmp(it->comps_obj, it2->comps_obj))
                return true;
        }
    }
    return false;
}

char* __comps_xml_arch_str(COMPS_Object *archlist) {
    size_t x, total_len = 0;
    COMPS_ObjListIt *it;
    char *arches_str;

    if (!archlist || !((COMPS_ObjList*)archlist)->len) {
        arches_str = malloc(sizeof(char));
        arches_str[0] = 0;
        return arches_str;
    }

    char *arches[((COMPS_ObjList*)archlist)->len];
    for (x=0, it = ((COMPS_ObjList*)archlist)->first; it != NULL; it = it->next, x++) {
        arches[x] = comps_object_tostr(it->comps_obj);
        total_len += strlen(arches[x])+1;
    }
    arches_str = malloc(sizeof(char) * (total_len));
    arches_str[0]=0;

    x = 0;
    for (; x < (size_t)(((COMPS_ObjList*)archlist)->len-1); x++) {
        strcat(arches_str, arches[x]);
        free(arches[x]);
        strcat(arches_str, " ");
    }
    strcat(arches_str, arches[x]);
    free(arches[x]);
    return arches_str;
}

int __comps_xml_arch(COMPS_Object *archlist, xmlTextWriterPtr writer) {
    if (archlist && ((COMPS_ObjList*)archlist)->len != 0) {
        char * str = __comps_xml_arch_str(archlist);
        int ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "arch",
                                              BAD_CAST str);
        free(str);
        return ret;
    } else return 0; 
}


char* __comps_strcpy(char *str) {
    char *ret;
    ret = malloc(sizeof(char)*(strlen(str)+1));
    return memcpy(ret, str, sizeof(char)*(strlen(str)+1));
}

char* __comps_strcat(char *str1, char *str2) {
    char *ret;
    ret = malloc(sizeof(char)*(strlen(str1)+strlen(str2)+1));
    memcpy(ret, str1, sizeof(char)*(strlen(str1)+1));
    strcat(ret, str2);
    return ret;
}

inline int __comps_check_xml_get(int retcode, COMPS_Object * log) {
    if (retcode<0) {
        comps_log_error((COMPS_Log*)log, COMPS_ERR_XMLGEN, 0);
        return -1;
    } return 0;
}
