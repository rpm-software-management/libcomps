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

#ifndef COMPS_ELEM_H
#define COMPS_ELEM_H

#include <stdlib.h>

#include "comps_dict.h"
#include "comps_parse.h"

typedef enum {COMPS_ELEM_UNKNOWN,
                COMPS_ELEM_DOC,
                COMPS_ELEM_GROUP,
                COMPS_ELEM_ID,
                COMPS_ELEM_NAME,
                COMPS_ELEM_DESC,
                COMPS_ELEM_DEFAULT,
                COMPS_ELEM_LANGONLY,
                COMPS_ELEM_USERVISIBLE,
                COMPS_ELEM_BIARCHONLY,  //RHEL-4.9
                COMPS_ELEM_PACKAGELIST,
                COMPS_ELEM_PACKAGEREQ,
                COMPS_ELEM_CATEGORY,
                COMPS_ELEM_GROUPLIST,
                COMPS_ELEM_GROUPID,
                COMPS_ELEM_DISPLAYORDER,
                COMPS_ELEM_ENV,
                COMPS_ELEM_OPTLIST,
                COMPS_ELEM_IGNOREDEP,
                COMPS_ELEM_WHITEOUT,
                COMPS_ELEM_BLACKLIST,
                COMPS_ELEM_PACKAGE,
                COMPS_ELEM_LANGPACKS,
                COMPS_ELEM_MATCH,
                COMPS_ELEM_NONE,
                COMPS_ELEM_SENTINEL} COMPS_ElemType;

typedef struct {
    char *name;
    char *val;
} COMPS_ElemAttr;

typedef struct COMPS_Elem COMPS_Elem;

struct COMPS_Elem{
    char *name;
    char valid;
    COMPS_Elem *ancestor;
    COMPS_ElemType type;
    COMPS_Dict *attrs;
};

typedef struct COMPS_ElemAttrInfo {
    char *name;
    signed char (*val_check)(const char*);
} COMPS_ElemAttrInfo;

typedef struct COMPS_ElemInfo {
    char *name;
    const COMPS_ElemType *ancestors;
    const COMPS_ElemAttrInfo **attributes;
    void (*preproc)(COMPS_Parsed*, COMPS_Elem *elem);
    void (*postproc)(COMPS_Parsed*, COMPS_Elem *elem);
} COMPS_ElemInfo;

extern const COMPS_ElemInfo* COMPS_ElemInfos[];

char * comps_elem_get_name(const COMPS_ElemType type);
void comps_elem_attr_destroy(void *attr);
COMPS_ElemAttr * comps_elem_attr_create(const char *name, const char *val);
COMPS_Elem* comps_elem_create(const char * s, const char ** attrs,
                              COMPS_ElemType type);
COMPS_ElemType comps_elem_get_type(const char * name);

void comps_elem_destroy(void * elem);

COMPS_PackageType comps_package_get_type(char *s);

void comps_elem_doc_preproc(COMPS_Parsed* parsed, COMPS_Elem *elem);
void comps_elem_group_preproc(COMPS_Parsed* parsed, COMPS_Elem *elem);
void comps_elem_group_postproc(COMPS_Parsed* parsed, COMPS_Elem *elem);
void comps_elem_category_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_category_postproc(COMPS_Parsed* parsed, COMPS_Elem *elem);
void comps_elem_env_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_env_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_packagereq_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_packagereq_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_groupid_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_groupid_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_match_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_package_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_ignoredep_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_idnamedesc_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_default_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_langonly_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_uservisible_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_biarchonly_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);

void comps_elem_grouplist_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_optionlist_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_optionlist_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_packagelist_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);
void comps_elem_display_order_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem);

extern const COMPS_ElemAttrInfo COMPS_REQUIRES_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_TYPE_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_BAO_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_DEFAULT_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_NAME_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_INSTALL_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_ARCH_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_C_ARCH_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_PACKAGE_ElemAttrInfo;
extern const COMPS_ElemAttrInfo COMPS_XMLLANG_ElemAttrInfo;

#endif
