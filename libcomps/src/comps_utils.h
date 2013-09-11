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

#include "comps_obj.h"
#include "comps_logger.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>


#define CONCAT(A,B) CONCAT2(A, B)
#define CONCAT2(A, B) A ## B

#define COMPS_DOCLIST_PREP(OBJ, LIST)\
    if (OBJ->LIST == NULL) {\
        OBJ->LIST = comps_list_create();\
        comps_list_init(OBJ->LIST);\
    }

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
        comps_objdict_set(OBJNAME->properties, #PROPNAME, str);\
    }\
}

#define HEAD_COMPS_STRPROP_SETTER(OBJNAME, OBJTYPE, PROPNAME)\
void CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _set_), PROPNAME)(OBJTYPE *OBJNAME,\
                                                                 char *PROPNAME,\
                                                                 char copy);

#define COMPS_NUMPROP_SETTER(OBJNAME, OBJTYPE, PROPNAME)\
inline void CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _set_), PROPNAME)(OBJTYPE *OBJNAME,\
                                                                        int PROPNAME){\
        COMPS_Object *num;\
        num = (COMPS_Object*)comps_num(PROPNAME);\
        comps_objdict_set(OBJNAME->properties, #PROPNAME, num);\
}

#define HEAD_COMPS_NUMPROP_SETTER(OBJNAME, OBJTYPE, PROPNAME)\
void CONCAT(CONCAT(CONCAT(comps_doc, OBJNAME), _set_), PROPNAME)(OBJTYPE *OBJNAME,\
                                                                        int PROPNAME);

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

#define COMPS_DESTROY_u(NAME, TYPE) void CONCAT(CONCAT(comps_, NAME), _destroy_u)\
                                                            (COMPS_Object* obj){\
    CONCAT(CONCAT(comps_, NAME),_destroy) ((TYPE*)obj);\
}
#define HEAD_COMPS_DESTROY_u(NAME, TYPE) void CONCAT(CONCAT(comps_, NAME), _destroy_u)\
                                                            (COMPS_Object* obj);

char __comps_strcmp(void *s1, void *s2);
void* __comps_str_clone(void *str);
void __comps_xml_prop(char *key, COMPS_Object *val);
#endif
