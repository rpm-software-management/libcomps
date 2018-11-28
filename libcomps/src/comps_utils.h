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

#ifndef COMPS_UTILS_H
#define COMPS_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "comps_obj.h"
#include "comps_objlist.h"
#include "comps_set.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>


#define CONCAT(A,B) CONCAT2(A, B)
#define CONCAT2(A, B) A ## B

#define COMPS_PROP_CMP(OBJNAME, PROPNAME, GETTER)\
char CONCAT(CONCAT(CONCAT(CONCAT(__comps_, OBJNAME),_),PROPNAME),cmp) (void *obj1,\
                                                                       void *obj2){\
    COMPS_Prop *prop1, *prop2;\
    prop1 = GETTER(obj1, #PROPNAME);\
    prop2 = GETTER(obj2, #PROPNAME);\
    if (prop1 == NULL && prop2 == NULL) return 1;\
    if (prop1 == NULL || prop2 == NULL) return 0;\
    if (prop1->prop_type != COMPS_PROP_STR ||\
        prop2->prop_type != COMPS_PROP_STR) return 0;\
    return __comps_strcmp(prop1->prop.str, prop2->prop.str);\
}

#define COMPS_STRPROP_SETTER(OBJNAME, OBJTYPE, PROPNAME)\
inline void CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _set_), PROPNAME)(OBJTYPE *OBJNAME,\
                                                                        char *PROPNAME,\
                                                                        char copy) {\
    (void)copy;\
    if (PROPNAME) {\
        COMPS_Object *str;\
        str = (COMPS_Object*)comps_str(PROPNAME);\
        comps_objdict_set_x(OBJNAME->properties, #PROPNAME, str);\
    }\
}

#define HEAD_COMPS_STRPROP_SETTER(OBJNAME, OBJTYPE, PROPNAME)\
void CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _set_), PROPNAME)(OBJTYPE *OBJNAME,\
                                                                 char *PROPNAME,\
                                                                 char copy);

#define COMPS_NUMPROP_SETTER(OBJNAME, OBJTYPE, PROPNAME)\
inline void CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _set_), PROPNAME)(OBJTYPE *OBJNAME,\
                                                                        int PROPNAME,\
                                                                        bool unset){\
        if (unset) {\
            comps_objdict_set_x(OBJNAME->properties, #PROPNAME, NULL);\
            return;\
        } \
        COMPS_Object *num;\
        num = (COMPS_Object*)comps_num(PROPNAME);\
        comps_objdict_set_x(OBJNAME->properties, #PROPNAME, num);\
}
#define HEAD_COMPS_NUMPROP_SETTER(OBJNAME, OBJTYPE, PROPNAME)\
void CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _set_), PROPNAME)(OBJTYPE *OBJNAME,\
                                                                 int PROPNAME,\
                                                                 bool unset);


#define COMPS_PROP_GETTER(OBJNAME, OBJTYPE, PROPNAME)\
inline COMPS_Object* CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _get_), PROPNAME)\
                                                         (OBJTYPE *OBJNAME){\
    return comps_objdict_get(OBJNAME->properties, #PROPNAME);\
}
#define HEAD_COMPS_PROP_GETTER(OBJNAME, OBJTYPE, PROPNAME)\
COMPS_Object* CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _get_), PROPNAME)\
                                                            (OBJTYPE *OBJNAME);
#define COMPS_PROP_GETTER_OBJ(OBJNAME, OBJTYPE, PROPNAME)\
inline COMPS_Object* CONCAT(CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME),\
                                           _get_), PROPNAME), _obj)\
                                                      (COMPS_Object *OBJNAME){\
    return CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _get_), PROPNAME)\
                                                        ((OBJTYPE*)OBJNAME);\
}
#define HEAD_COMPS_PROP_GETTER_OBJ(OBJNAME, PROPNAME)\
COMPS_Object* CONCAT(CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME),\
                                   _get_), PROPNAME), _obj)\
                                                        (COMPS_Object *OBJNAME);


#define COMPS_CREATE_u(NAME, TYPE) void CONCAT(CONCAT(comps_, NAME), _create_u)\
                                                        (COMPS_Object *uobj,\
                                                         COMPS_Object **args) {\
    CONCAT(CONCAT(comps_, NAME),_create)((TYPE*)uobj, args);\
}
#define HEAD_COMPS_CREATE_u(NAME, TYPE) void CONCAT(CONCAT(comps_, NAME), _create_u)\
                                                        (COMPS_Object *uobj,\
                                                         COMPS_Object **args);

#define COMPS_COPY_u(NAME, TYPE) void CONCAT(CONCAT(comps_,NAME),_copy_u)\
                                                            (COMPS_Object* obj_dst,\
                                                             COMPS_Object* obj_src){\
    CONCAT(CONCAT(comps_, NAME),_copy) ((TYPE*)obj_dst, (TYPE*)obj_src);\
}
#define HEAD_COMPS_COPY_u(NAME, TYPE) void CONCAT(CONCAT(comps_,NAME),_copy_u)\
                                                        (COMPS_Object* obj_dst,\
                                                         COMPS_Object* obj_src);

#define COMPS_DESTROY_u(NAME, TYPE)\
static void CONCAT(CONCAT(comps_, NAME), _destroy_u)(COMPS_Object* obj){\
    CONCAT(CONCAT(comps_, NAME),_destroy) ((TYPE*)obj);\
}
#define HEAD_COMPS_DESTROY_u(NAME, TYPE)\
static void CONCAT(CONCAT(comps_, NAME), _destroy_u)(COMPS_Object* obj);

#define COMPS_CMP_u(NAME, TYPE) signed char CONCAT(CONCAT(comps_,NAME),_cmp_u)\
                                                            (COMPS_Object* obj_dst,\
                                                             COMPS_Object* obj_src){\
    return CONCAT(CONCAT(comps_, NAME),_cmp) ((TYPE*)obj_dst, (TYPE*)obj_src);\
}
#define HEAD_COMPS_CMP_u(NAME, TYPE) void CONCAT(CONCAT(comps_,NAME),_cmp_u)\


#define COMPS_DOCOBJ_GETOBJLIST(OBJ, OBJTYPE, MEMBER, OBJS)\
COMPS_ObjList* CONCAT(CONCAT(CONCAT(comps_, OBJ), _), OBJS) (OBJTYPE *obj){\
    return obj->MEMBER;\
}
#define HEAD_COMPS_DOCOBJ_GETOBJLIST(OBJ, OBJTYPE, MEMBER, OBJS)\
COMPS_ObjList* CONCAT(CONCAT(CONCAT(comps_, OBJ), _), OBJS) (OBJTYPE *obj);

#define COMPS_DOCOBJ_SETOBJLIST(OBJ, OBJTYPE, MEMBER, OBJS)\
void CONCAT(CONCAT(CONCAT(comps_, OBJ), _set_), OBJS) (OBJTYPE *obj,\
                                                   COMPS_ObjList *list){\
    COMPS_OBJECT_DESTROY(obj->MEMBER);\
    obj->MEMBER = (COMPS_ObjList*)comps_object_incref((COMPS_Object*)list);\
}
#define HEAD_COMPS_DOCOBJ_SETOBJLIST(OBJ, OBJTYPE, MEMBER, OBJS)\
void CONCAT(CONCAT(CONCAT(comps_, OBJ), _set_), OBJS) (OBJTYPE *obj,\
                                                   COMPS_ObjList *list);

#define COMPS_DOCOBJ_GETARCHES(OBJ, OBJTYPE)\
COMPS_ObjList* CONCAT(CONCAT(comps_, OBJ), _arches) (OBJTYPE *obj){\
    return (COMPS_ObjList*)comps_objdict_get(obj->properties, "arches");\
}
#define HEAD_COMPS_DOCOBJ_GETARCHES(OBJ, OBJTYPE)\
COMPS_ObjList* CONCAT(CONCAT(comps_, OBJ), _arches)(OBJTYPE *obj);

#define COMPS_DOCOBJ_SETARCHES(OBJ, OBJTYPE)\
void CONCAT(CONCAT(comps_, OBJ), _set_arches)(OBJTYPE *obj,\
                                              COMPS_ObjList *list){\
    comps_objdict_set_x(obj->properties, "arches", (COMPS_Object*)list);\
}
#define HEAD_COMPS_DOCOBJ_SETARCHES(OBJ, OBJTYPE)\
void CONCAT(CONCAT(comps_, OBJ), _set_arches)(OBJTYPE *obj,\
                                              COMPS_ObjList *list);


#define COMPS_XMLRET_CHECK(free_code) if (ret == -1) {\
    free_code;\
    comps_log_error(log, COMPS_ERR_XMLGEN, 0);\
    return -1;\
}

char __comps_strcmp(void *s1, void *s2);
char* __comps_strcpy(char *str);
char* __comps_strcat(char *str1, char *str2);
void* __comps_str_clone(void *str);
int __comps_xml_prop(char *key, char *val, xmlTextWriterPtr writer);
char* __comps_num2boolstr(COMPS_Object* obj);
unsigned int digits_count(unsigned int x);
bool __comps_objlist_intersected(COMPS_ObjList *list1, COMPS_ObjList *list2);
char* __comps_xml_arch_str(COMPS_Object *arches);
int __comps_xml_arch(COMPS_Object *archlist, xmlTextWriterPtr writer);

int __comps_check_xml_get(int retcode, COMPS_Object * log);
#endif
