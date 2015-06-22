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
#include "comps_log.h"
#include "comps_types.h"
#include "comps_docgroup.h"
#include "comps_doccategory.h"
#include "comps_docenv.h"
#include "comps_validate.h"
#include "comps_default.h"

/** \file comps_doc.h
 * \brief COMPS_Doc header file
 *
 * COMPS_Doc object support union operation. Read more about
 * @link doc_unioning Libcomps objects unioning
 * @endlink
 * @see COMPS_Doc_getters @see COMPS_Doc_setters @see COMPS_Doc_adders
 */

/** @cond NOTMET */
#define COMPS_DOC_GETOBJLIST(OBJS) COMPS_ObjList* CONCAT(comps_doc_, OBJS)\
                                                           (COMPS_Doc *doc){\
    COMPS_ObjList *ret;\
    ret = (COMPS_ObjList*)comps_objdict_get(doc->objects, #OBJS);\
    if (!ret) {\
        ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);\
        comps_objdict_set_x(doc->objects, #OBJS, (COMPS_Object*)ret);\
        ret = (COMPS_ObjList*)comps_object_incref((COMPS_Object*)ret);\
    }\
    return ret;\
}
/** <@hideinititalizer */

#define HEAD_COMPS_DOC_GETOBJLIST(OBJS) COMPS_ObjList* CONCAT(comps_doc_, OBJS)\
                                                           (COMPS_Doc *doc);
/** <@hideinititalizer */

#define COMPS_DOC_SETOBJLIST(OBJS) void CONCAT(comps_doc_set_, OBJS)\
                                                       (COMPS_Doc *doc,\
                                                        COMPS_ObjList *list){\
    comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)list);\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_SETOBJLIST(OBJS) void CONCAT(comps_doc_set_, OBJS)\
                                                   (COMPS_Doc *doc,\
                                                    COMPS_ObjList *list);
/** <@hideinititalizer */

#define COMPS_DOC_GETOBJDICT(OBJNAME) COMPS_ObjDict* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc){\
    COMPS_ObjDict *ret;\
    ret = (COMPS_ObjDict*)comps_objdict_get(doc->objects, #OBJNAME);\
    if (!ret) {\
        ret = COMPS_OBJECT_CREATE(COMPS_ObjDict, NULL);\
        comps_objdict_set_x(doc->objects, #OBJNAME, (COMPS_Object*)ret);\
        ret = (COMPS_ObjDict*)comps_object_incref((COMPS_Object*)ret);\
    }\
    return ret;\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_GETOBJDICT(OBJNAME) COMPS_ObjDict* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc);
/** <@hideinititalizer */

#define COMPS_DOC_GETOBJMDICT(OBJNAME) COMPS_ObjMDict* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc){\
    COMPS_ObjMDict *ret;\
    ret = (COMPS_ObjMDict*)comps_objdict_get(doc->objects, #OBJNAME);\
    if (!ret) {\
        ret = COMPS_OBJECT_CREATE(COMPS_ObjMDict, NULL);\
        comps_objdict_set_x(doc->objects, #OBJNAME, (COMPS_Object*)ret);\
        ret = (COMPS_ObjMDict*)comps_object_incref((COMPS_Object*)ret);\
    }\
    return ret;\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_GETOBJMDICT(OBJNAME) COMPS_ObjMDict* CONCAT(comps_doc_, OBJNAME)\
                                                           (COMPS_Doc *doc);
/** <@hideinititalizer */

#define COMPS_DOC_SETOBJDICT(OBJS) void CONCAT(comps_doc_set_, OBJS)\
                                                       (COMPS_Doc *doc,\
                                                        COMPS_ObjDict *dict){\
    comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)dict);\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_SETOBJDICT(OBJS) void CONCAT(comps_doc_set_, OBJS)\
                                                   (COMPS_Doc *doc,\
                                                    COMPS_ObjDict *dict);
/** <@hideinititalizer */

#define COMPS_DOC_SETOBJMDICT(OBJS) void CONCAT(comps_doc_set_, OBJS)\
                                                       (COMPS_Doc *doc,\
                                                        COMPS_ObjMDict *dict){\
    comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)dict);\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_SETOBJMDICT(OBJS) void CONCAT(comps_doc_set_, OBJS)\
                                                   (COMPS_Doc *doc,\
                                                    COMPS_ObjMDict *dict);
/** <@hideinititalizer */


#define COMPS_DOC_ADDOBJLIST(OBJS, OBJNAME, OBJTYPE) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            OBJTYPE *obj){\
    COMPS_ObjList *ret;\
    ret = (COMPS_ObjList*)comps_objdict_get(doc->objects, #OBJS);\
    if (!ret) {\
        ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);\
        comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)ret);\
    } else {\
    }\
    comps_objlist_append_x(ret, (COMPS_Object*)obj);\
    COMPS_OBJECT_DESTROY(ret);\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_ADDOBJLIST(OBJNAME, OBJTYPE) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            OBJTYPE *obj);
/** <@hideinititalizer */

#define COMPS_DOC_ADDOBJDICT(OBJS, OBJNAME) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            char *key,\
                                                            COMPS_Str *obj){\
    COMPS_ObjDict *ret;\
    ret = (COMPS_ObjDict*)comps_objdict_get(doc->objects, #OBJS);\
    if (!ret) {\
        ret = COMPS_OBJECT_CREATE(COMPS_ObjDict, NULL);\
        comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)ret);\
    }\
    comps_objdict_set_x(ret, key, (COMPS_Object*)obj);\
    COMPS_OBJECT_DESTROY(ret);\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_ADDOBJDICT(OBJNAME) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            char *key,\
                                                            COMPS_Str *obj);
/** <@hideinititalizer */


#define COMPS_DOC_ADDOBJMDICT(OBJS, OBJNAME) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            char *key,\
                                                            COMPS_Str *obj){\
    COMPS_ObjMDict *ret;\
    ret = (COMPS_ObjMDict*)comps_objdict_get(doc->objects, #OBJS);\
    if (!ret) {\
        ret = COMPS_OBJECT_CREATE(COMPS_ObjMDict, NULL);\
        comps_objdict_set(doc->objects, #OBJS, (COMPS_Object*)ret);\
    }\
    comps_objmdict_set_x(ret, key, (COMPS_Object*)obj);\
    COMPS_OBJECT_DESTROY(ret);\
}
/** <@hideinititalizer */
#define HEAD_COMPS_DOC_ADDOBJMDICT(OBJNAME) void CONCAT(comps_doc_add_,\
                                                           OBJNAME)\
                                                           (COMPS_Doc *doc,\
                                                            char *key,\
                                                            COMPS_Str *obj);
/** <@hideinititalizer */

#define COMPS_DOC_GETPROP(OBJ,TYPE) CONCAT(TYPE,CONCAT(* ,CONCAT(comps_doc_, OBJ)))\
                                                           (COMPS_Doc *doc){\
    TYPE *ret;\
    ret = (COMPS_Str*)comps_objdict_get(doc->objects, #OBJ);\
    if (!ret) {\
        ret = COMPS_OBJECT_CREATE(TYPE, NULL);\
        comps_objdict_set_x(doc->objects, #OBJ, (COMPS_Object*)ret);\
        ret = (TYPE*)comps_object_incref((COMPS_Object*)ret);\
    }\
    return ret;\
}
/** <@hideinititalizer */

#define HEAD_COMPS_DOC_GETPROP(OBJ, TYPE) CONCAT(TYPE,CONCAT(*,CONCAT(comps_doc_, OBJS)))\
                                                           (COMPS_Doc *doc);
/** <@hideinititalizer */


#define COMPS_DOC_SETPROP(OBJ, TYPE) void CONCAT(comps_doc_set_, OBJ)\
                                                       (COMPS_Doc *doc,\
                                                        TYPE *value){\
    comps_objdict_set(doc->objects, #OBJ, (COMPS_Object*)value);\
}

/** <@hideinititalizer */


#define HEAD_COMPS_DOC_SETPROP(OBJ, TYPE) void CONCAT(comps_doc_set_, OBJ)\
                                                   (COMPS_Doc *doc,\
                                                    TYPE *value);

/** <@hideinititalizer */
/** @endcond*/

/** COMPS_Object derivate containing whole comps.xml document.
 */
typedef struct {
    COMPS_Object_HEAD;
    COMPS_ObjDict *objects; /**< dictionary of comps subobjects */
    COMPS_Log *log;
    /**< COMPS_Log object to store log messages evoked
     * by parsing and xml generating */
    COMPS_Str *encoding;   /**< comps.xml document encoding */
    COMPS_Str *doctype_name;
    COMPS_Str *doctype_sysid;
    COMPS_Str *doctype_pubid;
    COMPS_Str *lang;
    } COMPS_Doc;
COMPS_Object_TAIL(COMPS_Doc);

//HEAD_COMPS_CREATE_u(doc, COMPS_Doc)  /*comps_utils.h macro*/
//HEAD_COMPS_COPY_u(doc, COMPS_Doc)  /*comps_utils.h macro*/
//HEAD_COMPS_DESTROY_u(doc, COMPS_Doc)  /*comps_utils.h macro*/

/** constructor callback for COMPS_Doc object. COMPS_Doc is COMPS_Object
 * derivate. Use comps_object_create() or COMPS_OBJECT_CREATE for construction
 * instead
 *
 * @param doc allocated COMPS_Doc object
 * @param args array of constructor arguments. COMPS_Doc constructor accepts
 * encoding argument as COMPS_Str object only so array need not end with
 * sentinel item*/
void comps_doc_create(COMPS_Doc* doc, COMPS_Object **args);

/** copy callback for COMPS_Doc object*/
void comps_doc_copy(COMPS_Doc *doc_dst, COMPS_Doc *doc_src);

/** destructor callback for COMPS_Doc object. COMPS_Doc is COMPS_Object
 * derivate. Use comps_object_create() for construction instead*/
void comps_doc_destroy(COMPS_Doc *doc);

/** comparator callback for COMPS_Doc object */
signed char comps_doc_cmp_u(COMPS_Object *obj1, COMPS_Object *obj2);

/** \defgroup COMPS_Doc_getters COMPS_Doc getters
 * @{
 */

/** comps group list getter
 * @param doc COMPS_Doc instance
 * @return COMPS_ObjList object of COMPS_DocGroup*/
HEAD_COMPS_DOC_GETOBJLIST(groups) /*comps_doc.h macro*/

/** comps category list getter
 * @param doc COMPS_Doc instance
 * @return COMPS_ObjList object of COMPS_DocCategory*/
HEAD_COMPS_DOC_GETOBJLIST(categories) /*comps_doc.h macro*/

/** comps environment list getter
 * @param doc COMPS_Doc instance
 * @return COMPS_ObjList object of COMPS_DocEnv*/
HEAD_COMPS_DOC_GETOBJLIST(environments) /*comps_doc.h macro*/

/** comps langpack dictionary getter
 * @param doc COMPS_Doc instance
 * @return COMPS_ObjDict object of COMPS_Str*/
HEAD_COMPS_DOC_GETOBJDICT(langpacks) /*comps_doc.h macro*/

/** comps blacklist dictionary getter
 * @param doc COMPS_Doc instance
 * @return COMPS_ObjMDict object of COMPS_Str*/
HEAD_COMPS_DOC_GETOBJMDICT(blacklist) /*comps_doc.h macro*/

/** comps whiteout dictionary getter
 * @param doc COMPS_Doc instance
 * @return COMPS_ObjMDict object of COMPS_Str*/
HEAD_COMPS_DOC_GETOBJMDICT(whiteout) /*comps_doc.h macro*/

/**@}*/

/** \defgroup COMPS_Doc_setters COMPS_Doc setters
 * @{
 */

/** comps group list setter
 * @param doc COMPS_Doc instance
 * @param list COMPS_ObjList of COMPS_DocGroup items
 * \warning make sure of correct items type. Setter doesn't provide any
 * additional control routines
 */
HEAD_COMPS_DOC_SETOBJLIST(groups) /*comps_doc.h macro*/

/** comps category list setter
 * @param doc COMPS_Doc instance
 * @param list COMPS_ObjList of COMPS_DocCategory items
 * \warning make sure of correct items type. Setter doesn't provide any
 * additional control routines
 */
HEAD_COMPS_DOC_SETOBJLIST(categories) /*comps_doc.h macro*/

/** comps environments list setter
 * @param doc COMPS_Doc instance
 * @param list COMPS_ObjList of COMPS_DocEnv items
 * \warning make sure of correct items type. Setter doesn't provide any
 * additional control routines
 */
HEAD_COMPS_DOC_SETOBJLIST(environments) /*comps_doc.h macro*/

/** comps lankpack dict setter
 * @param doc COMPS_Doc instance
 * @param dict COMPS_ObjDict of COMPS_Str items
 * \warning make sure of correct items type. Setter doesn't provide any
 * additional control routines
 */
HEAD_COMPS_DOC_SETOBJDICT(langpacks) /*comps_doc.h macro*/

/** comps blacklist multi-dict setter
 * @param doc COMPS_Doc instance
 * @param dict COMPS_ObjMDict of COMPS_Str items
 * \warning make sure of correct items type. Setter doesn't provide any
 * additional control routines
 */
HEAD_COMPS_DOC_SETOBJMDICT(blacklist) /*comps_doc.h macro*/

/** comps whiteout multi-dict setter
 * @param doc COMPS_Doc instance
 * @param dict COMPS_ObjMDict of COMPS_Str items
 * \warning make sure of correct items type. Setter doesn't provide any
 * additional control routines
 */
HEAD_COMPS_DOC_SETOBJMDICT(whiteout) /*comps_doc.h macro*/

/**@}*/

/** \defgroup COMPS_Doc_adders COMPS_Doc adders
 * @{
 */

/** COMPS_DocGroup adder to group list in COMPS_Doc
 * @param doc COMPS_Doc instance
 * @param obj COMPS_DocGroup object
 * append COMPS_DocGroup object to group list in COMPS_Doc structure
 * \warning function doesn't increment COMPS_DocGroup object reference count.
 */
HEAD_COMPS_DOC_ADDOBJLIST(group, COMPS_DocGroup) /*comps_doc.h macro*/

/** COMPS_DocCategory adder to category list in COMPS_Doc
 * @param doc COMPS_Doc instance
 * @param obj COMPS_DocCategory object
 * append COMPS_DocCategory object to category list in COMPS_Doc structure
 * \warning function doesn't increment COMPS_DocCategory object reference count.
 */
HEAD_COMPS_DOC_ADDOBJLIST(category, COMPS_DocCategory) /*comps_doc.h macro*/

/** COMPS_DocEnv adder to environment list in COMPS_Doc
 * @param doc COMPS_Doc instance
 * @param obj COMPS_DocEnv object
 * append COMPS_DocEnv object to environment list in COMPS_Doc structure
 * \warning function doesn't increment COMPS_DocEnv object reference count.
 */
HEAD_COMPS_DOC_ADDOBJLIST(environment, COMPS_DocEnv) /*comps_doc.h macro*/

/** Langpack adder to langpack dict in COMPS_Doc
 * @param doc COMPS_Doc instance
 * @param key COMPS_Str dictionary key of langpack
 * @param obj COMPS_Str langpack
 * add langpack string to langpack dict in COMPS_Doc structure. If There's
 * allready langpack string with same key, will be overwritten.
 * \warning function doesn't increment obj param reference count.
 */
HEAD_COMPS_DOC_ADDOBJDICT(langpack) /*comps_doc.h macro*/

/** Blacklist adder to blacklist multi-dict in COMPS_Doc
 * @param doc COMPS_Doc instance
 * @param key COMPS_Str dictionary key of blacklist
 * @param obj COMPS_Str blacklist item
 * append blacklist item object to blacklist in COMPS_Doc structure. Items 
 * with same key are grouped in COMPS_ObjList object.
 * \warning function doesn't increment obj param reference count.
 */
HEAD_COMPS_DOC_ADDOBJMDICT(blacklist) /*comps_doc.h macro*/

/** whiteout adder to whitetout multi-dict in COMPS_Doc
 * @param doc COMPS_Doc instance
 * @param key COMPS_Str dictionary key of whiteout
 * @param obj COMPS_Str whiteout item
 * append whiteout item object to blacklist in COMPS_Doc structure. Items 
 * with same key are grouped in COMPS_ObjList object.
 * \warning function doesn't increment obj param reference count.
 */
HEAD_COMPS_DOC_ADDOBJMDICT(whiteout) /*comps_doc.h macro*/



/** whiteout adder to whitetout multi-dict in COMPS_Doc
 * @param doc COMPS_Doc instance
 * @param obj COMPS_Str language value
 * Set language to comps object and all subobjects
 */

HEAD_COMPS_DOC_SETPROP(lang, COMPS_Str) /*comps_doc.h macro*/

/**@}*/

/** \defgroup COMPS_Doc_filters COMPS_Doc filters
 * @{
 */

COMPS_ObjList* comps_doc_get_groups(COMPS_Doc *doc, char *id, char *name,
                                    char *desc, char *lang, int flags);
COMPS_ObjList* comps_doc_get_categories(COMPS_Doc *doc, char *id, char *name,
                                        char *desc, char *lang, int flags);
COMPS_ObjList* comps_doc_get_envs(COMPS_Doc *doc, char *id, char *name,
                                  char *desc, char *lang, int flags);

/**@}*/

//char* comps_doc_xml_str(COMPS_Doc* doc, char *enc, COMPS_Log *log);


//static signed char comps_doc_xml(COMPS_Doc *doc, xmlTextWriterPtr writer);

/** Write XML representation to file
 * @param doc COMPS_Doc object
 * @param filename filename where to write
 * @param stdoutredirect in non-zero all warning and error messages will
 * be redirected to stdout, otherwise will be stored in doc->log only
 * @return 0 if there wasn't any errors, 1 if there was non-fatal errors
 * -1 if fatal error emerge during xml generation
 */
signed char comps2xml_f(COMPS_Doc * doc, char *filename, char stdoutredirect,
                        COMPS_XMLOptions *xml_options,
                        COMPS_DefaultsOptions *def_options);

/** Generate XML string representating COMPS_Doc structure
 * @param doc COMPS_Doc object
 * @return XML string
 */
char* comps2xml_str(COMPS_Doc *doc, COMPS_XMLOptions *options,
                    COMPS_DefaultsOptions *def_options);

/** Union two COMPS_Doc structures
 * COMPS_Doc structures are unioned as unioning it's subparts
 * (group, categories, environments). Object with same 'id' attribute
 * are regarded as equal and unioned by with each other
 *
 * @param c1 COMPS_Doc object
 * @param c2 COMPS_Doc object
 */
COMPS_Doc* comps_doc_union(COMPS_Doc *c1, COMPS_Doc *c2);
COMPS_Doc* comps_doc_intersect(COMPS_Doc *c1, COMPS_Doc *c2);

COMPS_Doc* comps_doc_arch_filter(COMPS_Doc *source, COMPS_ObjList *arches);

COMPS_Str* comps_doc_doctype_name_get(COMPS_Doc* doc);
COMPS_Str* comps_doc_doctype_pubid_get(COMPS_Doc* doc);
COMPS_Str* comps_doc_doctype_sysid_get(COMPS_Doc* doc);
void comps_doc_doctype_name_set(COMPS_Doc* doc, COMPS_Str *val);
void comps_doc_doctype_sysid_set(COMPS_Doc* doc, COMPS_Str *val);
void comps_doc_doctype_pubid_set(COMPS_Doc* doc, COMPS_Str *val);

//extern COMPS_ObjectInfo COMPS_Doc_ObjInfo;
extern COMPS_ValRuleGeneric* COMPS_Doc_ValidateRules[];

#endif //COMPS_DOC_H

