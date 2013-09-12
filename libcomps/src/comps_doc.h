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

#ifndef COMPS_DOC_H
#define COMPS_DOC_H

#include "comps_obj.h"
#include "comps_objdict.h"
#include "comps_objlist.h"
#include "comps_utils.h"
#include "comps_logger.h"
#include "comps_types.h"
#include "comps_docgroup.h"
#include "comps_doccategory.h"
#include "comps_docenv.h"

#define COMPS_DOC_GETOBJLIST(OBJNAME) COMPS_ObjList* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc){\
    COMPS_ObjList *ret;\
    ret = (COMPS_ObjList*)comps_objdict_get(doc->objects, #OBJNAME);\
    if (!ret) {\
        ret = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo, NULL);\
        comps_objdict_set(doc->objects, #OBJNAME, (COMPS_Object*)ret);\
    }\
    return ret;\
}
#define HEAD_COMPS_DOC_GETOBJLIST(OBJNAME) COMPS_ObjList* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc);

#define COMPS_DOC_GETOBJDICT(OBJNAME) COMPS_ObjDict* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc){\
    COMPS_ObjDict *ret;\
    ret = (COMPS_ObjDict*)comps_objdict_get(doc->objects, #OBJNAME);\
    if (!ret) {\
        ret = (COMPS_ObjDict*)comps_object_create(&COMPS_ObjDict_ObjInfo, NULL);\
        comps_objdict_set(doc->objects, #OBJNAME, (COMPS_Object*)ret);\
    }\
    return ret;\
}
#define HEAD_COMPS_DOC_GETOBJDICT(OBJNAME) COMPS_ObjDict* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc);

#define COMPS_DOC_ADDOBJLIST(OBJS, OBJNAME, OBJTYPE) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            OBJTYPE *obj){\
    COMPS_ObjList *ret;\
    ret = (COMPS_ObjList*)comps_objdict_get(doc->objects, #OBJS);\
    if (!ret) {\
        ret = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo, NULL);\
        comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)ret);\
    }\
    comps_objlist_append(ret, (COMPS_Object*)obj);\
}
#define HEAD_COMPS_DOC_ADDOBJLIST(OBJNAME, OBJTYPE) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            OBJTYPE *obj);

#define COMPS_DOC_ADDOBJDICT(OBJS, OBJNAME) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            char *key,\
                                                            COMPS_Str *obj){\
    COMPS_ObjDict *ret;\
    ret = (COMPS_ObjDict*)comps_objdict_get(doc->objects, #OBJS);\
    if (!ret) {\
        ret = (COMPS_ObjDict*)comps_object_create(&COMPS_ObjDict_ObjInfo, NULL);\
        comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)ret);\
    }\
    comps_objdict_set(ret, key, (COMPS_Object*)obj);\
}
#define HEAD_COMPS_DOC_ADDOBJDICT(OBJNAME) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            char *key,\
                                                            COMPS_Str *obj);


typedef struct {
    COMPS_Object_HEAD
    COMPS_ObjDict *objects;
    COMPS_Logger *log;
    COMPS_Str *encoding;
    } COMPS_Doc;

HEAD_COMPS_CREATE_u(doc, COMPS_Doc)  /*comps_utils.h macro*/
HEAD_COMPS_COPY_u(doc, COMPS_Doc)  /*comps_utils.h macro*/
HEAD_COMPS_DESTROY_u(doc, COMPS_Doc)  /*comps_utils.h macro*/

void comps_doc_create(COMPS_Doc* doc, COMPS_Object **args);
void comps_doc_copy(COMPS_Doc *doc_dst, COMPS_Doc *doc_src);
void comps_doc_destroy(COMPS_Doc *doc);
signed char comps_doc_cmp_u(COMPS_Object *obj1, COMPS_Object *obj2);

HEAD_COMPS_DOC_GETOBJLIST(groups) /*comps_doc.h macro*/
HEAD_COMPS_DOC_GETOBJLIST(categories) /*comps_doc.h macro*/
HEAD_COMPS_DOC_GETOBJLIST(environments) /*comps_doc.h macro*/
HEAD_COMPS_DOC_GETOBJDICT(langpacks) /*comps_doc.h macro*/
HEAD_COMPS_DOC_GETOBJDICT(blacklist) /*comps_doc.h macro*/
HEAD_COMPS_DOC_GETOBJDICT(whiteout) /*comps_doc.h macro*/

HEAD_COMPS_DOC_ADDOBJLIST(group, COMPS_DocGroup) /*comps_doc.h macro*/
HEAD_COMPS_DOC_ADDOBJLIST(category, COMPS_DocCategory) /*comps_doc.h macro*/
HEAD_COMPS_DOC_ADDOBJLIST(environment, COMPS_DocEnv) /*comps_doc.h macro*/

HEAD_COMPS_DOC_ADDOBJDICT(langpack) /*comps_doc.h macro*/
HEAD_COMPS_DOC_ADDOBJDICT(blacklist) /*comps_doc.h macro*/
HEAD_COMPS_DOC_ADDOBJDICT(whiteout) /*comps_doc.h macro*/

void comps_doc_xml_f(COMPS_Doc* doc,  char *filename, char *enc, COMPS_Logger *log);
char* comps_doc_xml_str(COMPS_Doc* doc, char *enc, COMPS_Logger *log);
void comps_doc_xml(COMPS_Doc *obj, xmlTextWriterPtr writer);

extern COMPS_ObjectInfo COMPS_Doc_ObjInfo;

#endif //COMPS_DOC_H

