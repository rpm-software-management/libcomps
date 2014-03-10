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

#ifndef COMPS_DOCGROUP_H
#define COMPS_DOCGROUP_H

#include "comps_utils.h"
#include "comps_obj.h"
#include "comps_objdict.h"
#include "comps_objlist.h"
#include "comps_docpackage.h"
#include "comps_validate.h"
#include "comps_radix.h"
#include "comps_default.h"

#include <stddef.h>
#include <assert.h>

/** \file comps_docgroup.h
 * \brief COMPS_DocGroup header file
 * @see COMPS_DocGroup_prop_setters
 * @see COMPS_DocGroup_prop_getters
 * @see COMPS_DocGroup_list_setters
 * @see COMPS_DocGroup_list_getters
 *
 * COMPS_DocGroup object support union operation. Read more about
 * @link doc_unioning Libcomps objects unioning
 * @endlink
 *
 */

/** COMPS_Object derivate representing group element in comps.xml file */
typedef struct {
    COMPS_Object_HEAD;
    COMPS_ObjDict *properties;
    /**< properties of group */
    COMPS_ObjDict *name_by_lang;
    /**< language localization of name attribute */
    COMPS_ObjDict *desc_by_lang;
    /**< language localization of description attribute */
    COMPS_ObjList *packages;
    /**< list of packages in group */
} COMPS_DocGroup;

//HEAD_COMPS_CREATE_u(docgroup, COMPS_DocGroup)  /*comps_utils.h macro*/
//HEAD_COMPS_COPY_u(docgroup, COMPS_DocGroup)  /*comps_utils.h macro*/
//HEAD_COMPS_DESTROY_u(docgroup, COMPS_DocGroup)  /*comps_utils.h macro*/

/**
 * \defgroup COMPS_DocGroup_prop_setters COMPS_DocGroup properties setters
 * @{
 **/
/** COMPS_DocGroup id setter
 * @param obj COMPS_DocGroup object
 * @param id COMPS_Str object representing id
 * \warning setter doesn't increment reference counter of id object
 */
HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, id) /*comps_utils.h macro*/

/** COMPS_DocGroup name setter
 * @param obj COMPS_DocGroup object
 * @param name COMPS_Str object representing name
 * \warning setter doesn't increment reference counter of name object
 */
HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, name) /*comps_utils.h macro*/

/** COMPS_DocGroup description setter
 * @param obj COMPS_DocGroup object
 * @param desc COMPS_Str object representing description
 * \warning setter doesn't increment reference counter of desc object
 */
HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, desc) /*comps_utils.h macro*/

/** COMPS_DocGroup default setter
 * @param obj COMPS_DocGroup object
 * @param def COMPS_Num object representing def
 * \warning setter doesn't increment reference counter of def object
 */
HEAD_COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, def) /*comps_utils.h macro*/

/** COMPS_DocGroup uservisible setter
 * @param obj COMPS_DocGroup object
 * @param uservisible COMPS_NUm object representing uservisible
 * \warning setter doesn't increment reference counter of uservisible object
 */
HEAD_COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, uservisible) /*comps_utils.h macro*/

/** COMPS_DocGroup biarchonly setter
 * @param obj COMPS_DocGroup object
 * @param uservisible COMPS_NUm object representing biarchonly
 * \warning setter doesn't increment reference counter of biarchonly object
 */
HEAD_COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, biarchonly) /*comps_utils.h macro*/

/** COMPS_DocGroup display_order setter
 * @param obj COMPS_DocGroup object
 * @param display_order COMPS_Num object representing display_order
 * \warning setter doesn't increment reference counter of display_order object
 */
HEAD_COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, display_order) /*comps_utils.h macro*/

/** COMPS_DocGroup langonly setter
 * @param obj COMPS_DocGroup object
 * @param langonly COMPS_Str object representing langonly
 * \warning setter doesn't increment reference counter of langonly object
 */
HEAD_COMPS_STRPROP_SETTER(group, COMPS_DocGroup, langonly) /*comps_utils.h macro*/
/**@}*/

/**
 * \defgroup COMPS_DocGroup_prop_getters COMPS_DocGroup properties getters
 * @{
 **/

/** COMPS_DocGroup id getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Str object representing group id with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, id) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER_OBJ(group, id) /*comps_utils.h macro*/

/** COMPS_DocGroup name getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Str object representing group name with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, name) /*comps_utils.h macro*/

/** COMPS_DocGroup description getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Str object representing group description with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, desc) /*comps_utils.h macro*/

/** COMPS_DocGroup default getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Num object representing group default with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, def) /*comps_utils.h macro*/

/** COMPS_DocGroup uservisible getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Num object representing group uservisible with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, uservisible) /*comps_utils.h macro*/

/** COMPS_DocGroup biarchonly getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Num object representing group biarchonly with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, biarchonly) /*comps_utils.h macro*/

/** COMPS_DocGroup display_order getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Num object representing group display_order with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, display_order) /*comps_utils.h macro*/

/** COMPS_DocGroup langonly getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_Str object representing group langonly with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(group, COMPS_DocGroup, langonly) /*comps_utils.h macro*/
/**@}*/

/**
 * \defgroup COMPS_DocGroup_list_getters COMPS_DocGroup list getters
 * @{
 **/

/** COMPS_DocGroup package list getter
 * @param obj COMPS_DocGroup object
 * @return COMPS_ObjList with packages in group. Reference of list isn't
 * incremented
 */
HEAD_COMPS_DOCOBJ_GETOBJLIST(docgroup, COMPS_DocGroup, packages, packages)
/**@}*/

/**
 * \defgroup COMPS_DocGroup_list_setters COMPS_DocGroup list setters
 * @{
 **/

/** COMPS_DocGroup packages list setter
 * @param obj COMPS_DocGroup object
 * @param list COMPS_ObjList object with group_ids items
 * \warning existing packages list object reference count will be decremented.
 * Setter doesn't provides any additional items type checking
 */
HEAD_COMPS_DOCOBJ_SETOBJLIST(docgroup, COMPS_DocGroup, packages, packages)
/**@}*/

HEAD_COMPS_DOCOBJ_GETARCHES(docgroup, COMPS_DocGroup)
HEAD_COMPS_DOCOBJ_SETARCHES(docgroup, COMPS_DocGroup)

signed char comps_docgroup_cmp_u(COMPS_Object *group1, COMPS_Object *group2);
char __comps_docgroup_idcmp(void *g1, void *g2);

/** add package to packages list in group
 * @param cat COMPS_DocGroup object
 * @param package COMPS_DocGroupPackage object
 * \warning COMPS_DocGroupPackage reference counter isn't incremented
 */
void comps_docgroup_add_package(COMPS_DocGroup *group,
                                COMPS_DocGroupPackage *package);

/** return list of packages matching name and type
 * name or type could be NULL and then doens't affect search filter. Search
 * doesn't support any asterisk or dot notation like in regular expression
 * @param group COMPS_DocGroup object
 * @param name package name
 * @param type package type
 * @return list of filtered packages 
 */
COMPS_ObjList* comps_docgroup_get_packages(COMPS_DocGroup *group, char *name,
                                       COMPS_PackageType type);

/** union two groups into one and return new COMPS_DocGroup object
 * @param g1 COMPS_DocGroup object
 * @param g2 COMPS_DocGroup object
 * @return new COMPS_DocGroup object
 */
COMPS_DocGroup* comps_docgroup_union(COMPS_DocGroup *g1, COMPS_DocGroup *g2);

/** intersect two groups into one and return new COMPS_DocGroup object
 * @param c1 COMPS_DocGroup object
 * @param c2 COMPS_DocGroup object
 * @return new COMPS_DocGroup object
 */
COMPS_DocGroup* comps_docgroup_intersect(COMPS_DocGroup *g1,
                                         COMPS_DocGroup *g2);

signed char comps_docgroup_xml(COMPS_DocGroup *group, xmlTextWriterPtr writer,
                               COMPS_Log *log, COMPS_XMLOptions *xml_options,
                               COMPS_DefaultsOptions *def_options);

COMPS_DocGroup* comps_docgroup_arch_filter(COMPS_DocGroup *source,
                                           COMPS_ObjList *arches);

extern COMPS_ObjectInfo COMPS_DocGroup_ObjInfo;
extern COMPS_ValRuleGeneric* COMPS_DocGroup_ValidateRules[];

#endif
