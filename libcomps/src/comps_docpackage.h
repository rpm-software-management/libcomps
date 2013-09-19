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

typedef enum {COMPS_PACKAGE_DEFAULT, COMPS_PACKAGE_OPTIONAL,
              COMPS_PACKAGE_CONDITIONAL, COMPS_PACKAGE_MANDATORY,
              COMPS_PACKAGE_UNKNOWN} COMPS_PackageType;

typedef struct {
    COMPS_Object_HEAD
    COMPS_PackageType type;
    COMPS_Str *name;
    COMPS_Str *requires;
} COMPS_DocGroupPackage;


HEAD_COMPS_CREATE_u(docpackage, COMPS_DocGroupPackagePackage)  /*comps_utils.h macro*/
HEAD_COMPS_COPY_u(docpackage, COMPS_DocGroupPackagePackage)  /*comps_utils.h macro*/
HEAD_COMPS_DESTROY_u(docpackage, COMPS_DocGroupPackagePackage)  /*comps_utils.h macro*/

signed char comps_docpackage_cmp_u(COMPS_Object *pkg1, COMPS_Object *pkg2);
char comps_docpackage_cmp_set(void *pkg1, void *pkg2);

COMPS_Object* comps_docpackage_get_name(COMPS_DocGroupPackage *pkg);
void comps_docpackage_set_name(COMPS_DocGroupPackage *pkg, char *name, char copy);
COMPS_Object* comps_docpackage_get_requires(COMPS_DocGroupPackage *pkg);
void comps_docpackage_set_requires(COMPS_DocGroupPackage *pkg, char *requires, char copy);
COMPS_Object* comps_docpackage_get_type(COMPS_DocGroupPackage *pkg);
void comps_docpackage_set_type(COMPS_DocGroupPackage *pkg,
                                   COMPS_PackageType type);
void comps_docpackage_set_type_i(COMPS_DocGroupPackage *pkg, int type);
const char* comps_docpackage_type_str(COMPS_PackageType type);

void comps_docpackage_xml(COMPS_DocGroupPackage *package,
                          xmlTextWriterPtr writer,
                          COMPS_Logger *log);


extern COMPS_ObjectInfo COMPS_DocGroupPackage_ObjInfo;

#endif

