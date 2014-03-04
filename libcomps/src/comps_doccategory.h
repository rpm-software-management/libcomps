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

#ifndef COMPS_DOCCATEGORY_H
#define COMPS_DOCCATEGORY_H

#include "comps_obj.h"
#include "comps_objdict.h"
#include "comps_objlist.h"
#include "comps_utils.h"
#include "comps_docgroupid.h"
#include "comps_validate.h"
#include "comps_radix.h"
#include "comps_log.h"
#include "comps_default.h"

#include <stddef.h>
#include <assert.h>

/** \file comps_doccategory.h
 * \brief COMPS_DocCategory header file
 * @see COMPS_DocCategory_prop_setters
 * @see COMPS_DocCategory_prop_getters
 * @see COMPS_DocCategory_list_setters
 * @see COMPS_DocCategory_list_getters
 *
 * COMPS_DocCategory object support union operation. Read more about
 * @link doc_unioning Libcomps objects unioning
 * @endlink
 */

/** COMPS_Object derivate representing category element in comps.xml structure*/
typedef struct {
    COMPS_Object_HEAD;
    COMPS_ObjDict *properties; /**< properties of category */
    COMPS_ObjDict *name_by_lang; /**<language localization of name attribute*/
    COMPS_ObjDict *desc_by_lang;
    /**<language localization of description attribute */
    COMPS_ObjList *group_ids;
    /**< list of group_ids */
} COMPS_DocCategory;
COMPS_Object_TAIL(COMPS_DocCategory);


//HEAD_COMPS_CREATE_u(doccategory, COMPS_DocCategory)  /*comps_utils.h macro*/
//HEAD_COMPS_COPY_u(doccategory, COMPS_DocCategory)  /*comps_utils.h macro*/
//HEAD_COMPS_DESTROY_u(doccategory, COMPS_DocCategory)  /*comps_utils.h macro*/

/**
 * \defgroup COMPS_DocCategory_prop_setters COMPS_DocCategory properties setters
 * @{
 **/

/** COMPS_DocCategory id setter
 * @param obj COMPS_DocCategory object
 * @param id COMPS_Str object representing id
 * \warning setter doesn't increment reference counter of id object
 */
HEAD_COMPS_STRPROP_SETTER(category, COMPS_DocCategory, id) /*comps_utils.h macro*/

/** COMPS_DocCategory name setter
 * @param obj COMPS_DocCategory object
 * @param name COMPS_Str object representing name
 * \warning setter doesn't increment reference counter of name object
 */
HEAD_COMPS_STRPROP_SETTER(category, COMPS_DocCategory, name) /*comps_utils.h macro*/

/** COMPS_DocCategory description setter
 * @param obj COMPS_DocCategory object
 * @param desc COMPS_Str object representing description
 * \warning setter doesn't increment reference counter of desc object
 */
HEAD_COMPS_STRPROP_SETTER(category, COMPS_DocCategory, desc) /*comps_utils.h macro*/

/** COMPS_DocCategory display order setter
 * @param obj COMPS_DocCategory object
 * @param display_order COMPS_Num object representing display order
 * \warning setter doesn't increment reference counter of display_order object
 */
HEAD_COMPS_NUMPROP_SETTER(category, COMPS_DocCategory,
                          display_order) /*comps_utils.h macro*/
/**@}*/

/**
 * \defgroup COMPS_DocCategory_prop_getters COMPS_CategoryGroup properties getters
 * @{
 **/

/** COMPS_DocCategory id getter
 * @param obj COMPS_DocCategory object
 * @return COMPS_Str object representing category id with incremented reference 
 * count
 */
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, id) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER_OBJ(category, id) /*comps_utils.h macro*/

/** COMPS_DocCategory name getter
 * @param obj COMPS_DocCategory object
 * @return COMPS_Str object representing category name with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, name) /*comps_utils.h macro*/

/** COMPS_DocCategory description getter
 * @param obj COMPS_DocCategory object
 * @return COMPS_Str object representing category description with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, desc) /*comps_utils.h macro*/

/** COMPS_DocCategory display order getter
 * @param obj COMPS_DocCategory object
 * @return COMPS_Str object representing category display order with
 * incremented reference count
 */
HEAD_COMPS_PROP_GETTER(category, COMPS_DocCategory, display_order) /*comps_utils.h macro*/
/**@}*/

/**
 * \defgroup COMPS_DocCategory_list_getters COMPS_DocCategory list getters
 * @{
 **/

/** COMPS_DocCategory group_ids list getter
 * @param obj COMPS_DocCategory object
 * @return COMPS_ObjList object with group_ids items. Reference of object isn't
 * incremented
 */
HEAD_COMPS_DOCOBJ_GETOBJLIST(doccategory, COMPS_DocCategory, group_ids, group_ids)
/**@}*/

/**
 * \defgroup COMPS_DocCategory_list_setters COMPS_DocCategory list setters
 * @{
 **/

/** COMPS_DocCategory group_ids list setter
 * @param obj COMPS_DocCategory object
 * @param list COMPS_ObjList object with group_ids items
 * \warning existing group_ids list object reference count will be decremented. 
 * Setter doesn't provides any additional items type checking
 */
HEAD_COMPS_DOCOBJ_SETOBJLIST(doccategory, COMPS_DocCategory, group_ids, group_ids)
/**@}*/

HEAD_COMPS_DOCOBJ_GETARCHES(doccategory, COMPS_DocCategory)
HEAD_COMPS_DOCOBJ_SETARCHES(doccategory, COMPS_DocCategory)

char __comps_doccategory_idcmp(void *c1, void *c2);

/** COMPS_DocCategory compare callback
 * @param cat1 COMPS_DocCategory object
 * @param cat2 COMPS_DocCategory object
 * @return non-zero if objects are equal, otherwise 0
 */
signed char comps_doccategory_cmp_u(COMPS_Object *cat1, COMPS_Object *cat2);

/** add group_id to group_ids list in category
 * @param cat COMPS_DocCategory object
 * @param gid COMPS_DocGroupId object
 * \warning COMPS_DocGroupId reference counter isn't incremented
 */
void comps_doccategory_add_groupid(COMPS_DocCategory *cat,
                                   COMPS_DocGroupId *gid);

/** union two categories into one and return new COMPS_DocCategory object
 * @param c1 COMPS_DocCategory object
 * @param c2 COMPS_DocCategory object
 * @return new COMPS_DocCategory object
 */
COMPS_DocCategory* comps_doccategory_union(COMPS_DocCategory *c1,
                                           COMPS_DocCategory *c2);

/** intersect two categories into one and return new COMPS_DocCategory object
 * @param c1 COMPS_DocCategory object
 * @param c2 COMPS_DocCategory object
 * @return new COMPS_DocCategory object
 */
COMPS_DocCategory* comps_doccategory_intersect(COMPS_DocCategory *c1,
                                               COMPS_DocCategory *c2);

signed char comps_doccategory_xml(COMPS_DocCategory *category,
                                  xmlTextWriterPtr writer, COMPS_Log *log,
                                  COMPS_XMLOptions *xml_options,
                                  COMPS_DefaultsOptions *def_options);
COMPS_DocCategory* comps_doccategory_arch_filter(COMPS_DocCategory *source,
                                                 COMPS_ObjList *arches);

extern COMPS_ValRuleGeneric* COMPS_DocCategory_ValidateRules[];
#endif
