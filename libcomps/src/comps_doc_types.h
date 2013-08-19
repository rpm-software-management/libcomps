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

#ifndef COMPS_DOCTYPES_H
#define COMPS_DOCTYPES_H

#include "comps_dict.h"
#include "comps_list.h"
#include "comps_set.h"
#include "comps_logger.h"

/**
 * COMPS Document
 *\par COMPS_Doc creating/destroying
 * @see comps_doc_create
 * @see comps_doc_init
 * @see comps_doc_clear
 * @see comps_doc_destroy
 * \par COMPS_Doc adding content
 * @see comps_doc_add_group
 * @see comps_doc_add_category
 * @see comps_doc_add_env
 * \par COMPS_Doc receiving content
 * @see comps_doc_get_groups
 * @see comps_doc_get_categories
 * @see comps_doc_get_envs
 * \par COMPS_Doc removing content
 * @see comps_doc_del_group
 * @see comps_doc_del_category
 * @see comps_doc_del_env
 * \par COMPS_Doc set operations
 * @see comps_doc_union
 * @see comps_doc_intersect
 * \par COMPS_Doc xml conversion
 * @see comps2xml_f
 * @see comps2xml_str
 */
typedef struct {
    /** List of document groups @see COMPS_DocGroup*/
    COMPS_Dict *lobjects;
    COMPS_Dict *dobjects;
    COMPS_Dict *mdobjects;

    //COMPS_List *groups;
    /** List of document categories @see COMPS_DocCategory*/
    //COMPS_List *categories;
    /** List of document environemnts @see COMPS_DocEnv*/
    //COMPS_List *envs;
    /** Logger for logging xml convert process @see COMPS_Logger*/
    void *reserved;
    COMPS_Logger *log;
    const char *encoding;
    } COMPS_Doc;


/**
 * COMPS Document category
 * \par COMPS_DocCategory create/destroy
 * @see comps_doccategory_create
 * @see comps_doccategory_destroy
 * @see comps_doccategory_clone
 * \par COMPS_DocCategory properties setters
 * @see comps_doccategory_set_id
 * @see comps_doccategory_set_name
 * @see comps_doccategory_set_desc
 * @see comps_doccategory_set_displayorder
 * \par COMPS_DocCategory groupid adding
 * @see comps_doccategory_add_groupid
 * \par COMPS_DocCategory set operations
 * @see comps_doccategory_union
 * @see comps_doccategory_intersect
 */
typedef struct {
    COMPS_Dict *properties;
    /** category id */
    //char *id;
    /** category name*/
    //char *name;
    COMPS_Dict *name_by_lang;
    /** category description */
    //char *desc;
    COMPS_Dict *desc_by_lang;
    /** category display order */
    //unsigned display_order;
    /** category groups ids*/
    COMPS_List *group_ids;

    void *reserved;
} COMPS_DocCategory;

/**
 * COMPS Document Group
 * \par COMPS_DocGroup create/destroy
 * @see comps_docgroup_create
 * @see comps_docgroup_destroy
 * @see comps_docgroup_clone
 * \par COMPS_DocGroup properties setters
 * @see comps_docgroup_set_id
 * @see comps_docgroup_set_name
 * @see comps_docgroup_set_desc
 * @see comps_docgroup_set_default
 * @see comps_docgroup_set_uservisible
 * @see comps_docgroup_set_langonly
 * \par COMPS_DocGroup package management
 * @see comps_docgroup_add_package
 * @see comps_docgroup_get_packages
 * \par COMPS_DocGroup set operations
 * @see comps_docgroup_union
 * @see comps_docgroup_intersect
 */
typedef struct {
    COMPS_Dict *properties;
    /** group id*/
    //char *id;
    /** group name*/
    //char *name;
    COMPS_Dict *name_by_lang;
    /** group description*/
    //char *desc;
    COMPS_Dict *desc_by_lang;
    /** group default*/
    //unsigned def;
    /** group lang only*/
    //char *lang_only;
    /**group user visible*/
    //unsigned uservisible;
    /** group display order*/
    //unsigned display_order;
    /** group list of packages. @see COMPS_DocGroupPackage*/
    COMPS_List *packages;
    void *reserved;
} COMPS_DocGroup;

/**
 * COMPS Document Environment
 *\par COMPS_DocEnv create/destroy
 * @see comps_docenv_create
 * @see comps_docenv_clone
 * @see comps_docenv_destroy
 * \par COMPS_DocEnv properties setters
 * @see comps_docenv_set_id
 * @see comps_docenv_set_name
 * @see comps_docenv_set_desc
 * @see comps_docenv_set_displayorder
 * \par COMPS_DocEnv content adding
 * @see comps_docenv_add_optionid
 * @see comps_docenv_add_groupid
 * \par COMPS_DocEnv set operations
 * @see comps_docenv_union
 * @see comps_docenv_intersect
 */
typedef struct {
    COMPS_Dict *properties;
    /** environment id*/
    //char *id;
    /** environment name*/
    //char *name;
    COMPS_Dict *name_by_lang;
    /** environment description*/
    //char *desc;
    COMPS_Dict *desc_by_lang;
    /** environment display order*/
    //unsigned display_order;
    /** environment grouid list*/
    COMPS_List *group_list;
    /** environment optionid list*/
    COMPS_List *option_list;
    void *reserved;
} COMPS_DocEnv;


typedef struct {
    char *name;
    char def;
    void *reserved;
} COMPS_DocGroupId;

typedef enum {COMPS_PACKAGE_DEFAULT, COMPS_PACKAGE_OPTIONAL,
              COMPS_PACKAGE_CONDITIONAL, COMPS_PACKAGE_MANDATORY,
              COMPS_PACKAGE_UNKNOWN} COMPS_PackageType;

/**
 * COMPS Document package
 * @see comps_docpackage_cmp
 * \par COMPS_DocGroupPackage create/destroy
 * @see comps_docpackage_create
 * @see comps_docpackage_clone
 * @see comps_docpackage_destroy
 * \par COMPS_DocGroupPackage properties setters
 * @see comps_docpackage_set_name
 * @see comps_docpackage_set_type
 */
typedef struct {
    COMPS_Dict *properties;
    /** package type @see COMPS_PackageType */
    COMPS_PackageType type;
    /** name of package */
    char *name;
    char *requires;
    void *reserved;
} COMPS_DocGroupPackage;

typedef enum {COMPS_PROP_STR, COMPS_PROP_NUM} COMPS_PropType;
typedef struct {
    COMPS_PropType prop_type;
    union {
        char *str;
        int num;
    } prop;
} COMPS_Prop;

#endif
