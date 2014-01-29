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

/** \file comps_docgroupid.h
 * \brief COMPS_DocGroupId header file
 */

#ifndef COMPS_DOCGROUPID_H
#define COMPS_DOCGROUPID_H

#include <stdbool.h>

#include "comps_utils.h"
#include "comps_log.h"
#include "comps_validate.h"
#include "comps_default.h"
#include "comps_obj.h"

/** COMPS_Object derivate representing group_id element in comps.xml file */
typedef struct {
    COMPS_Object_HEAD;
    COMPS_Str *name;
    /**< name of GroupId */
    bool def;
    /**< GroupId default attribute */
    COMPS_ObjList *arches;
} COMPS_DocGroupId;
COMPS_Object_TAIL(COMPS_DocGroupId);

//HEAD_COMPS_CREATE_u(docgroupid, COMPS_DocGroupId)  /*comps_utils.h macro*/
//HEAD_COMPS_COPY_u(docgroupid, COMPS_DocGroupId)  /*comps_utils.h macro*/
//HEAD_COMPS_DESTROY_u(docgroupid, COMPS_DocGroupId)  /*comps_utils.h macro*/

char __comps_docgroupid_cmp_set(void *gid1, void *gid2);

/** COMPS_DocGroupId name getter
 * @param gid COMPS_DocGroupId object
 * @return COMPS_Str object representing GroupId name with incremented
 * reference counter
 */
COMPS_Object* comps_docgroupid_get_name(COMPS_DocGroupId *gid);

/** COMPS_DocGroupId name setter
 * @param gid COMPS_DocGroupId object
 * @param name new name of COMPS_DocGroupId object. Old name object's reference
 * @param copy deprecated parameter
 * counter will be decremented
 */
void comps_docgroupid_set_name(COMPS_DocGroupId *gid, char *name, char copy);

/** COMPS_DocGroupId default getter
 * @param gid COMPS_DocGroupId object
 * @return COMPS_Num object representing GroupId default with incremented
 * reference counter
 */
COMPS_Object* comps_docgroupid_get_default(COMPS_DocGroupId *gid);

/** COMPS_DocGroupId name setter
 * @param gid COMPS_DocGroupId object
 * @param def COMPS_DocGroupId default value.
 *
 * Old defaut objects reference counter will be decremented
 */
void comps_docgroupid_set_default(COMPS_DocGroupId *gid, int def);

COMPS_ObjList* comps_docgroupid_arches(COMPS_DocGroupId *gid);
void comps_docgroupid_set_arches(COMPS_DocGroupId *gid,
                                 COMPS_ObjList *arches);

signed char comps_docgroupid_xml(COMPS_DocGroupId *groupid,
                                  xmlTextWriterPtr writer,
                                  COMPS_Log *log, COMPS_XMLOptions *options,
                                  COMPS_DefaultsOptions *def_options);

extern COMPS_ValRuleGeneric* COMPS_DocGroupId_ValidateRules[];

#endif
