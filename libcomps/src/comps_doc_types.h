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
#include "comps_obj.h"

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
    COMPS_Object_HEAD
    COMPS_ObjDict *objects;
    COMPS_Logger *log;
    COMPS_Str *encoding;
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
    COMPS_Object_HEAD
    COMPS_ObjDict *properties;
    COMPS_ObjDict *name_by_lang;
    COMPS_ObjDict *desc_by_lang;
    COMPS_ObjList *group_ids;

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
    COMPS_Object_HEAD
    COMPS_ObjDict *properties;
    COMPS_ObjDict *name_by_lang;
    COMPS_ObjDict *desc_by_lang;
    COMPS_ObjList *packages;
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
    COMPS_Object_HEAD
    COMPS_Dict *properties;
    COMPS_Dict *name_by_lang;
    COMPS_Dict *desc_by_lang;
    COMPS_List *group_list;
    COMPS_List *option_list;
    void *reserved;
} COMPS_DocEnv;


typedef struct {
    COMPS_Object_HEAD
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
    COMPS_Object_HEAD
    //COMPS_Dict *properties;
    COMPS_PackageType type;
    UNFO_Str *name;
    UNFO_Str *requires;
    //char *name;
    //char *requires;
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
