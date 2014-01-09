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

#ifndef COMPS_DOCPACKAGE_H
#define COMPS_DOCPACKAGE_H

#include "comps_utils.h"
#include "comps_obj.h"
#include "comps_log.h"

#include "comps_validate.h"
#include "comps_default.h"

/** \file comps_docpackage.h
 * \brief COMPS_DocPackage header file
 */

/** COMPS_DocGroupPackage type
 */
typedef enum {COMPS_PACKAGE_DEFAULT, COMPS_PACKAGE_OPTIONAL,
              COMPS_PACKAGE_CONDITIONAL, COMPS_PACKAGE_MANDATORY,
              COMPS_PACKAGE_UNKNOWN} COMPS_PackageType;

/** COMPS_Object derivate representing packagereq element in comps.xml structure*/
typedef struct {
    COMPS_Object_HEAD;
    COMPS_PackageType type; /**< package type */
    COMPS_Str *name; /**< name of package */
    COMPS_Str *requires; /**< packagereq requires attribute */
    COMPS_Num *basearchonly;
    COMPS_ObjList *arches;
} COMPS_DocGroupPackage;


//HEAD_COMPS_CREATE_u(docpackage, COMPS_DocGroupPackagePackage)  /*comps_utils.h macro*/
//HEAD_COMPS_COPY_u(docpackage, COMPS_DocGroupPackagePackage)  /*comps_utils.h macro*/
//HEAD_COMPS_DESTROY_u(docpackage, COMPS_DocGroupPackagePackage)  /*comps_utils.h macro*/

signed char comps_docpackage_cmp_u(COMPS_Object *pkg1, COMPS_Object *pkg2);
char comps_docpackage_cmp_set(void *pkg1, void *pkg2);

/** COMPS_DocGroupPackage name getter
 * @param pkg COMPS_DocGroupPackage object
 * @return COMPS_Str object typed as COMPS_Object representating package's name
 * with incremented reference counter
 */
COMPS_Object* comps_docpackage_get_name(COMPS_DocGroupPackage *pkg);

/** COMPS_DocGroupPackage name setter
 * @param pkg COMPS_DocGroupPackage object
 * @param name new name of package
 * @param copy deprecated argument
 *
 * Old name object's reference counter will be decremented
 */
void comps_docpackage_set_name(COMPS_DocGroupPackage *pkg, char *name, char copy);

/** COMPS_DocGroupPackage requires getter
 * @param pkg COMPS_DocGroupPackage object
 * @return COMPS_Str object typed as COMPS_Object representating package's
 * requires attribute with incremented reference counter
 */
COMPS_Object* comps_docpackage_get_requires(COMPS_DocGroupPackage *pkg);

/** COMPS_DocGroupPackage requires setter
 * @param pkg COMPS_DocGroupPackage object
 * @param requires new requries attribute value
 * @param copy deprecated argument
 *
 * Old requires object's reference counter will be decremented
 */
void comps_docpackage_set_requires(COMPS_DocGroupPackage *pkg, char *requires, char copy);

/** COMPS_DocGroupPackage type getter
 * @param pkg COMPS_DocGroupPackage object
 * @return COMPS_Num object typed as COMPS_Object representating package's
 * type as number with incremented reference counter
 */
COMPS_Object* comps_docpackage_get_type(COMPS_DocGroupPackage *pkg);

/** COMPS_DocGroupPackage type setter
 * @param pkg COMPS_DocGroupPackage object
 * @param type package type
 *
 * old object with stored type will be decremented
 */
void comps_docpackage_set_type(COMPS_DocGroupPackage *pkg,
                                   COMPS_PackageType type,
                                   bool unset);

/** COMPS_DocGroupPackage type setter same as comps_docpackage_set_type
 * @param pkg COMPS_DocGroupPackage object
 * @param type package type as integer
 */
void comps_docpackage_set_type_i(COMPS_DocGroupPackage *pkg, int type, bool unset);

/** return package type as string
 * @param type package type as COMPS_PackageType
 * @return string representation of type
 * */
const char* comps_docpackage_type_str(COMPS_PackageType type);

/** set package basearchonly attribute
 * @param type package type as COMPS_PckageType
 * @param basearchonly basearchonly attribute
 * */
void comps_docpackage_set_basearchonly(COMPS_DocGroupPackage *pkg,
                                       int basearchonly, bool unset);

/** return package basearchonly attrinute
 * @param type package type as COMPS_PackageType
 * @return COMPS_Num basearchonly attribute
 * */
COMPS_Object* comps_docpackage_get_basearchonly(COMPS_DocGroupPackage *pkg);

char __comps_docpackage_idcmp(void *pkg1, void *pkg2);
COMPS_ObjList* comps_docpackage_arches(COMPS_DocGroupPackage *pkg);
void comps_docpackage_set_arches(COMPS_DocGroupPackage *pkg,
                                 COMPS_ObjList *arches);

signed char comps_docpackage_xml(COMPS_DocGroupPackage *pkg,
                                 xmlTextWriterPtr writer,
                                 COMPS_Log *log, COMPS_XMLOptions *xml_options,
                                 COMPS_DefaultsOptions *def_options);

extern COMPS_ObjectInfo COMPS_DocGroupPackage_ObjInfo;
extern COMPS_ValRuleGeneric* COMPS_DocGroupPackage_ValidateRules[];

#endif

