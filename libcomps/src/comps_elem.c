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

#include "comps_elem.h"

const COMPS_ElemInfo COMPS_UNKNOWN_ElemInfo = {
    .name = "???",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_NONE, COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,
    .postproc = NULL
};
const COMPS_ElemInfo COMPS_DOC_ElemInfo = {
    .name = "comps",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_NONE, COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = &comps_elem_doc_preproc,
    .postproc = NULL
};
const COMPS_ElemInfo COMPS_GROUP_ElemInfo = {
    .name = "group",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_DOC, COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_C_ARCH_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_group_preproc,
    .postproc = &comps_elem_group_postproc
};
const COMPS_ElemInfo COMPS_ID_ElemInfo = {
    .name = "id",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_CATEGORY,
                                          COMPS_ELEM_ENV,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_id_preproc,
    .postproc = &comps_elem_idnamedesc_postproc
};
const COMPS_ElemInfo COMPS_NAME_ElemInfo = {
    .name = "name",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_CATEGORY,
                                          COMPS_ELEM_ENV,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_XMLLANG_ElemAttrInfo},
    .preproc = NULL,//&comps_elem_name_preproc,
    .postproc = &comps_elem_idnamedesc_postproc
};
const COMPS_ElemInfo COMPS_DESC_ElemInfo = {
    .name = "description",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_CATEGORY,
                                          COMPS_ELEM_ENV,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_XMLLANG_ElemAttrInfo},
    .preproc = NULL,//&comps_elem_desc_preproc,
    .postproc = &comps_elem_idnamedesc_postproc
};
const COMPS_ElemInfo COMPS_DEFAULT_ElemInfo = {
    .name = "default",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_default_preproc,
    .postproc = &comps_elem_default_postproc
};
const COMPS_ElemInfo COMPS_LANGONLY_ElemInfo = {
    .name = "langonly",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_langonly_preproc,
    .postproc = &comps_elem_langonly_postproc
};
const COMPS_ElemInfo COMPS_USERVISIBLE_ElemInfo = {
    .name = "uservisible",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_uservisible_preproc,
    .postproc = &comps_elem_uservisible_postproc
};
const COMPS_ElemInfo COMPS_BIARCHONLY_ElemInfo = {
    .name = "biarchonly",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_uservisible_preproc,
    .postproc = &comps_elem_biarchonly_postproc
};
const COMPS_ElemInfo COMPS_PACKAGELIST_ElemInfo = {
    .name = "packagelist",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_packagelist_preproc,
    .postproc = &comps_elem_packagelist_postproc
};
const COMPS_ElemInfo COMPS_PACKAGEREQ_ElemInfo = {
    .name = "packagereq",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_PACKAGELIST,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_REQUIRES_ElemAttrInfo,
                                                &COMPS_TYPE_ElemAttrInfo,
                                                &COMPS_BAO_ElemAttrInfo,
                                                &COMPS_C_ARCH_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_packagereq_preproc,
    .postproc = &comps_elem_packagereq_postproc
};
const COMPS_ElemInfo COMPS_CATEGORY_ElemInfo = {
    .name = "category",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_DOC,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_C_ARCH_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_category_preproc,
    .postproc = &comps_elem_category_postproc
};
const COMPS_ElemInfo COMPS_GROUPLIST_ElemInfo = {
    .name = "grouplist",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_CATEGORY,
                                          COMPS_ELEM_ENV,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_grouplist_preproc,
    .postproc = &comps_elem_grouplist_postproc
};
const COMPS_ElemInfo COMPS_GROUPID_ElemInfo = {
    .name = "groupid",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUPLIST,
                                          COMPS_ELEM_OPTLIST,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_DEFAULT_ElemAttrInfo,
                                                &COMPS_C_ARCH_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_groupid_preproc,
    .postproc = &comps_elem_groupid_postproc
};
const COMPS_ElemInfo COMPS_DISPLAYORDER_ElemInfo = {
    .name = "display_order",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_GROUP,
                                          COMPS_ELEM_CATEGORY,
                                          COMPS_ELEM_ENV,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_display_order_preproc,
    .postproc = &comps_elem_display_order_postproc
};
const COMPS_ElemInfo COMPS_ENV_ElemInfo = {
    .name = "environment",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_DOC,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_C_ARCH_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_env_preproc,
    .postproc = &comps_elem_env_postproc
};
const COMPS_ElemInfo COMPS_OPTLIST_ElemInfo = {
    .name = "option_list",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_ENV,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = &comps_elem_optionlist_preproc,
    .postproc = &comps_elem_optionlist_postproc
};
const COMPS_ElemInfo COMPS_LANGPACKS_ElemInfo = {
    .name = "langpacks",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_DOC,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_langpacks_preproc,
    .postproc = NULL//&comps_elem_langpacks_postproc
};
const COMPS_ElemInfo COMPS_MATCH_ElemInfo = {
    .name = "match",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_LANGPACKS,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_NAME_ElemAttrInfo,
                                                &COMPS_INSTALL_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_match_preproc,
    .postproc = NULL//&comps_elem_match_postproc
};
const COMPS_ElemInfo COMPS_BLACKLIST_ElemInfo = {
    .name = "blacklist",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_DOC,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_blacklist_preproc,
    .postproc = NULL//&comps_elem_blacklist_postproc
};
const COMPS_ElemInfo COMPS_PACKAGE_ElemInfo = {
    .name = "package",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_BLACKLIST,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_NAME_ElemAttrInfo,
                                                &COMPS_ARCH_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_package_preproc,
    .postproc = NULL//&comps_elem_package_postproc
};
const COMPS_ElemInfo COMPS_WHITEOUT_ElemInfo = {
    .name = "whiteout",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_DOC,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){NULL},
    .preproc = NULL,//&comps_elem_whiteout_preproc,
    .postproc = NULL//&comps_elem_whiteout_postproc
};
const COMPS_ElemInfo COMPS_IGNOREDEP_ElemInfo = {
    .name = "ignoredep",
    .ancestors = (const COMPS_ElemType[]){COMPS_ELEM_WHITEOUT,
                                          COMPS_ELEM_SENTINEL},
    .attributes = (const COMPS_ElemAttrInfo*[]){&COMPS_REQUIRES_ElemAttrInfo,
                                                &COMPS_PACKAGE_ElemAttrInfo,
                                                NULL},
    .preproc = &comps_elem_ignoredep_preproc,
    .postproc = NULL//&comps_elem_ignoredep_postproc
};

const COMPS_ElemAttrInfo COMPS_REQUIRES_ElemAttrInfo = {
    .name="requires"
};
const COMPS_ElemAttrInfo COMPS_TYPE_ElemAttrInfo = {
    .name="type"
};
const COMPS_ElemAttrInfo COMPS_BAO_ElemAttrInfo = {
    .name="basearchonly"
};
const COMPS_ElemAttrInfo COMPS_DEFAULT_ElemAttrInfo = {
    .name="default"
};
const COMPS_ElemAttrInfo COMPS_NAME_ElemAttrInfo = {
    .name="name"
};
const COMPS_ElemAttrInfo COMPS_INSTALL_ElemAttrInfo = {
    .name="install"
};
const COMPS_ElemAttrInfo COMPS_ARCH_ElemAttrInfo = {
    .name="arch"
};
const COMPS_ElemAttrInfo COMPS_C_ARCH_ElemAttrInfo = {
    .name="arch"
};
const COMPS_ElemAttrInfo COMPS_PACKAGE_ElemAttrInfo = {
    .name="package"
};
const COMPS_ElemAttrInfo COMPS_XMLLANG_ElemAttrInfo = {
    .name="xml:lang"
};

const COMPS_ElemInfo *COMPS_ElemInfos[] = {
    [COMPS_ELEM_UNKNOWN] = &COMPS_UNKNOWN_ElemInfo,
    [COMPS_ELEM_DOC] = &COMPS_DOC_ElemInfo,
    [COMPS_ELEM_GROUP] = &COMPS_GROUP_ElemInfo,
    [COMPS_ELEM_ID] = &COMPS_ID_ElemInfo,
    [COMPS_ELEM_NAME] = &COMPS_NAME_ElemInfo,
    [COMPS_ELEM_DESC] = &COMPS_DESC_ElemInfo,
    [COMPS_ELEM_DEFAULT] = &COMPS_DEFAULT_ElemInfo,
    [COMPS_ELEM_LANGONLY] = &COMPS_LANGONLY_ElemInfo,
    [COMPS_ELEM_USERVISIBLE] = &COMPS_USERVISIBLE_ElemInfo,
    [COMPS_ELEM_BIARCHONLY] = &COMPS_BIARCHONLY_ElemInfo,
    [COMPS_ELEM_PACKAGELIST] = &COMPS_PACKAGELIST_ElemInfo,
    [COMPS_ELEM_PACKAGEREQ] = &COMPS_PACKAGEREQ_ElemInfo,
    [COMPS_ELEM_CATEGORY] = &COMPS_CATEGORY_ElemInfo,
    [COMPS_ELEM_GROUPLIST] = &COMPS_GROUPLIST_ElemInfo,
    [COMPS_ELEM_GROUPID] = &COMPS_GROUPID_ElemInfo,
    [COMPS_ELEM_DISPLAYORDER] = &COMPS_DISPLAYORDER_ElemInfo,
    [COMPS_ELEM_ENV] = &COMPS_ENV_ElemInfo,
    [COMPS_ELEM_OPTLIST] = &COMPS_OPTLIST_ElemInfo,
    [COMPS_ELEM_IGNOREDEP] = &COMPS_IGNOREDEP_ElemInfo,
    [COMPS_ELEM_WHITEOUT] = &COMPS_WHITEOUT_ElemInfo,
    [COMPS_ELEM_BLACKLIST] = &COMPS_BLACKLIST_ElemInfo,
    [COMPS_ELEM_PACKAGE] = &COMPS_PACKAGE_ElemInfo,
    [COMPS_ELEM_LANGPACKS] = &COMPS_LANGPACKS_ElemInfo,
    [COMPS_ELEM_MATCH] = &COMPS_MATCH_ElemInfo
};

char * comps_elem_get_name(COMPS_ElemType type) {
    switch (type) {
        case COMPS_ELEM_SENTINEL:
        case COMPS_ELEM_UNKNOWN:
        case COMPS_ELEM_NONE:
            return "";
        break;
        case COMPS_ELEM_DOC:
            return "comps";
        break;
        case COMPS_ELEM_GROUP:
            return "group";
        break;
        case COMPS_ELEM_ID:
            return "id";
        break;
        case COMPS_ELEM_NAME:
            return "name";
        break;
        case COMPS_ELEM_DESC:
            return "description";
        break;
        case COMPS_ELEM_DEFAULT:
            return "default";
        break;
        case COMPS_ELEM_USERVISIBLE:
            return "uservisible";
        break;
        case COMPS_ELEM_BIARCHONLY:
            return "biarchonly";
        break;
        case COMPS_ELEM_PACKAGELIST:
            return "packagelist";
        break;
        case COMPS_ELEM_PACKAGEREQ:
            return "packagereq";
        break;
        case COMPS_ELEM_CATEGORY:
            return "category";
        break;
        case COMPS_ELEM_GROUPLIST:
            return "grouplist";
        break;
        case COMPS_ELEM_GROUPID:
            return "groupid";
        break;
        case COMPS_ELEM_LANGONLY:
            return "langonly";
        break;
        case COMPS_ELEM_DISPLAYORDER:
            return "display_order";
        break;
        case COMPS_ELEM_ENV:
            return "environment";
        break;
        case COMPS_ELEM_OPTLIST:
            return "optionlist";
        break;
        case COMPS_ELEM_IGNOREDEP:
            return "ignoredep";
        break;
        case COMPS_ELEM_WHITEOUT:
            return "whiteout";
        break;
        case COMPS_ELEM_BLACKLIST:
            return "blacklist";
        break;
        case COMPS_ELEM_PACKAGE:
            return "package";
        break;
        case COMPS_ELEM_LANGPACKS:
            return "langpacks";
        break;
        case COMPS_ELEM_MATCH:
            return "match";
        break;
    }
    return NULL;
}

COMPS_ElemType comps_elem_get_type(const char * name) {
    COMPS_ElemType type;
    if (name == NULL) return COMPS_ELEM_NONE;
    if (strcmp(name, "comps") == 0) type = COMPS_ELEM_DOC;
    else if (strcmp(name, "group") == 0) type = COMPS_ELEM_GROUP;
    else if (strcmp(name, "id") == 0) type = COMPS_ELEM_ID;
    else if (strcmp(name, "name") == 0) type = COMPS_ELEM_NAME;
    else if (strcmp(name, "description") == 0) type = COMPS_ELEM_DESC;
    else if (strcmp(name, "default") == 0) type = COMPS_ELEM_DEFAULT;
    else if (strcmp(name, "uservisible") == 0) type = COMPS_ELEM_USERVISIBLE;
    else if (strcmp(name, "biarchonly") == 0) type = COMPS_ELEM_BIARCHONLY;
    else if (strcmp(name, "packagelist") == 0) type = COMPS_ELEM_PACKAGELIST;
    else if (strcmp(name, "packagereq") == 0) type = COMPS_ELEM_PACKAGEREQ;
    else if (strcmp(name, "category") == 0) type = COMPS_ELEM_CATEGORY;
    else if (strcmp(name, "grouplist") == 0) type = COMPS_ELEM_GROUPLIST;
    else if (strcmp(name, "groupid") == 0) type = COMPS_ELEM_GROUPID;
    else if (strcmp(name, "display_order") == 0) type = COMPS_ELEM_DISPLAYORDER;
    else if (strcmp(name, "langonly") == 0) type = COMPS_ELEM_LANGONLY;
    else if (strcmp(name, "environment") == 0) type = COMPS_ELEM_ENV;
    else if (strcmp(name, "optionlist") == 0) type = COMPS_ELEM_OPTLIST;
    else if (strcmp(name, "langpacks") == 0) type = COMPS_ELEM_LANGPACKS;
    else if (strcmp(name, "match") == 0) type = COMPS_ELEM_MATCH;
    else if (strcmp(name, "blacklist") == 0) type = COMPS_ELEM_BLACKLIST;
    else if (strcmp(name, "package") == 0) type = COMPS_ELEM_PACKAGE;
    else if (strcmp(name, "whiteout") == 0) type = COMPS_ELEM_WHITEOUT;
    else if (strcmp(name, "ignoredep") == 0) type = COMPS_ELEM_IGNOREDEP;
    else type = COMPS_ELEM_UNKNOWN;
    return type;

}

COMPS_PackageType comps_package_get_type(char * s) {
    if (__comps_strcmp(s, "default")) return COMPS_PACKAGE_DEFAULT;
    else if (__comps_strcmp(s, "optional")) return COMPS_PACKAGE_OPTIONAL;
    else if (__comps_strcmp(s, "conditional")) return COMPS_PACKAGE_CONDITIONAL;
    else if (__comps_strcmp(s, "mandatory") || !s) return COMPS_PACKAGE_MANDATORY;
    else return COMPS_PACKAGE_UNKNOWN;
}

#define parser_line XML_GetCurrentLineNumber(parsed->parser)
#define parser_col XML_GetCurrentColumnNumber(parsed->parser)

void __comps_check_required_param(COMPS_Object *param, char *param_name,
                                  COMPS_Parsed *parsed) {
    if (!param) {
        comps_log_error_x(parsed->log, COMPS_ERR_ELEM_REQUIRED, 3,
                          comps_str(param_name),
                          comps_num(parser_line), comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(param);
}

void __comps_check_allready_set(COMPS_Object *param, char *param_name,
                                COMPS_Parsed *parsed) {
    if (param) {
        comps_log_error_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET, 3,
                          comps_str(param_name),
                          comps_num(parser_line), comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(param);
}

COMPS_ObjList * __comps_split_arches(char *arches) {
    COMPS_ObjList *list;
    char *pch;
    list = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    pch = strtok(arches, " ,");
    while (pch != NULL) {
        comps_objlist_append_x(list, (COMPS_Object*)comps_str(pch));
        pch = strtok(NULL, " ,");
    }
    return list;
}

void comps_elem_doc_preproc(COMPS_Parsed* parsed, COMPS_Elem *elem) {
    (void)elem;
    COMPS_Object *prop = (COMPS_Object*)comps_str(parsed->enc);
    parsed->comps_doc = COMPS_OBJECT_CREATE(COMPS_Doc, (COMPS_Object*[]){prop});
    comps_object_destroy(prop);
}
void comps_elem_group_preproc(COMPS_Parsed* parsed, COMPS_Elem *elem) {
    char *arches;

    COMPS_DocGroup *group = COMPS_OBJECT_CREATE(COMPS_DocGroup, NULL);
    comps_doc_add_group(parsed->comps_doc, group);
    arches = comps_dict_get(elem->attrs, "arch");
    if (arches) {
        COMPS_ObjList *larches = __comps_split_arches(arches);
        comps_docgroup_set_arches(group, larches);
    }
}
void comps_elem_group_postproc(COMPS_Parsed* parsed, COMPS_Elem *elem) {
    COMPS_DocGroup *group;
    COMPS_ObjList *list;
    COMPS_Object *obj;
    (void)elem;

    list = comps_doc_groups(parsed->comps_doc);
    group = (COMPS_DocGroup*)list->last->comps_obj;

    __comps_check_required_param(comps_docgroup_get_id(group), "id", parsed);
    __comps_check_required_param(comps_docgroup_get_name(group), "name", parsed);
    __comps_check_required_param(comps_docgroup_get_desc(group), "description", parsed);
    obj = comps_docgroup_get_def(group);
    //printf("default_default: %d\n", parsed->def_options->default_default);
    if (!obj) {
        comps_docgroup_set_def(group, parsed->def_options->default_default, 0);
    }
    COMPS_OBJECT_DESTROY(obj);
    obj = comps_docgroup_get_uservisible(group);
    if (!obj) {
        comps_docgroup_set_uservisible(group,
                                      parsed->def_options->default_uservisible,
                                      0);
    }
    COMPS_OBJECT_DESTROY(obj);
    obj = comps_docgroup_get_biarchonly(group);
    if (!obj) {
        comps_docgroup_set_biarchonly(group,
                                      parsed->def_options->default_biarchonly,
                                      0);
        //printf("biarchonly not found\n");
    }
    COMPS_OBJECT_DESTROY(obj);
    COMPS_OBJECT_DESTROY(list);
}

void comps_elem_category_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    char *arches;
    COMPS_DocCategory *category = COMPS_OBJECT_CREATE(COMPS_DocCategory, NULL);
    comps_doc_add_category(parsed->comps_doc, category);
    arches = comps_dict_get(elem->attrs, "arch");
    if (arches) {
        COMPS_ObjList *larches = __comps_split_arches(arches);
        comps_doccategory_set_arches(category, larches);
    }
}
void comps_elem_category_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_DocCategory *category;
    COMPS_ObjList *list;
    (void)elem;

    list = comps_doc_categories(parsed->comps_doc);
    category = (COMPS_DocCategory*)list->last->comps_obj;
    __comps_check_required_param(comps_doccategory_get_id(category),
                                 "id", parsed);
    __comps_check_required_param(comps_doccategory_get_name(category),
                                 "name", parsed);
    __comps_check_required_param(comps_doccategory_get_desc(category),
                                 "description", parsed);
    COMPS_OBJECT_DESTROY(list);
}
void comps_elem_env_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    char *arches;
    COMPS_DocEnv *env = COMPS_OBJECT_CREATE(COMPS_DocEnv, NULL);
    comps_doc_add_environment(parsed->comps_doc, env);
    arches = comps_dict_get(elem->attrs, "arch");
    if (arches) {
        COMPS_ObjList *larches = __comps_split_arches(arches);
        comps_docenv_set_arches(env, larches);
    }
}
void comps_elem_env_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_DocEnv *env;
    COMPS_ObjList *list;
    (void)elem;

    list = comps_doc_environments(parsed->comps_doc);
    env = (COMPS_DocEnv*)list->last->comps_obj;
    __comps_check_required_param(comps_docenv_get_id(env),
                                 "id", parsed);
    __comps_check_required_param(comps_docenv_get_name(env),
                                 "name", parsed);
    __comps_check_required_param(comps_docenv_get_desc(env),
                                 "description", parsed);
    COMPS_OBJECT_DESTROY(list);
}
void comps_elem_grouplist_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_ObjList *list, *list2;
    if (elem->ancestor->type == COMPS_ELEM_ENV) {
        list = comps_doc_environments(parsed->comps_doc);
    } else {
        list = comps_doc_categories(parsed->comps_doc);
    }
    list2 = ((COMPS_DocEnv*)list->last->comps_obj)->group_list;
    if (!list2->len) {
        comps_log_error_x(parsed->log, COMPS_ERR_LIST_EMPTY, 3,
                          comps_str(COMPS_ElemInfos[elem->type]->name),
                          comps_num(parser_line), comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(list);
}
void comps_elem_optionlist_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    (void)elem;
    (void)parsed;
}
void comps_elem_optionlist_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    (void)elem;
    (void)parsed;
}
void comps_elem_packagelist_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_ObjList *list, *list2;
    list = comps_doc_groups(parsed->comps_doc);
    list2 = ((COMPS_DocGroup*)list->last->comps_obj)->packages;
    if (!list2->len) {
        comps_log_error_x(parsed->log, COMPS_ERR_LIST_EMPTY, 3,
                          comps_str(COMPS_ElemInfos[elem->type]->name),
                          comps_num(parser_line), comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(list);
}

void comps_elem_packagereq_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_DocGroup *group;
    COMPS_ObjList *list;
    COMPS_DocGroupPackage *package;
    char *tmp;

    list = comps_doc_groups(parsed->comps_doc);
    group = (COMPS_DocGroup*)list->last->comps_obj;
    COMPS_OBJECT_DESTROY(list);

    package = COMPS_OBJECT_CREATE(COMPS_DocGroupPackage, NULL);

    comps_docgroup_add_package(group, package);
    tmp = comps_dict_get(elem->attrs, "type");
    if (!tmp)
        package->type = parsed->def_options->default_pkgtype;
    else
        package->type = comps_package_get_type(tmp);
    tmp = comps_dict_get(elem->attrs, "requires");
    if (tmp)
        package->requires = comps_str(tmp);
    tmp = comps_dict_get(elem->attrs, "basearchonly");
    if (tmp && (strcmp(tmp, "true") == 0))
        package->basearchonly = comps_num(1);
    char *arches = comps_dict_get(elem->attrs, "arch");
    if (arches) {
        COMPS_ObjList *larches = __comps_split_arches(arches);
        comps_docpackage_set_arches(package, larches);
    }
}
void comps_elem_packagereq_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    #define last_pkg ((COMPS_DocGroupPackage*)((COMPS_DocGroup*)list->last->comps_obj)->packages->last->comps_obj)
    COMPS_ObjList *list = comps_doc_groups(parsed->comps_doc);
    if (parsed->tmp_buffer) {
        //printf("%s\n", parsed->tmp_buffer);
        comps_docpackage_set_name(last_pkg, parsed->tmp_buffer, 1);
    } else {
        comps_log_error_x(parsed->log, COMPS_ERR_NOCONTENT, 3,
                          comps_str(COMPS_ElemInfos[elem->type]->name),
                          comps_num(parser_line), comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(list);
    parsed->tmp_buffer = NULL;
    #undef last_pkg
}

void comps_elem_groupid_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_DocGroupId *groupid;
    COMPS_ObjList *list;
    char *tmp;
    groupid = COMPS_OBJECT_CREATE(COMPS_DocGroupId, NULL);
    tmp = comps_dict_get(elem->attrs, "default");
    if (tmp)
        comps_docgroupid_set_default(groupid,
                                     __comps_strcmp(tmp, "true")?1:0);
    else
        comps_docgroupid_set_default(groupid, 0);
    if (elem->ancestor->type == COMPS_ELEM_OPTLIST) {
        list = comps_doc_environments(parsed->comps_doc);
        COMPS_DocEnv *env = (COMPS_DocEnv*)list->last->comps_obj;
        COMPS_OBJECT_DESTROY(list);
        comps_docenv_add_optionid(env, groupid);
    } else {
        if (elem->ancestor->ancestor->type == COMPS_ELEM_ENV) {
            list = comps_doc_environments(parsed->comps_doc);
            COMPS_DocEnv *env = (COMPS_DocEnv*)list->last->comps_obj;
            COMPS_OBJECT_DESTROY(list);
            comps_docenv_add_groupid(env, groupid);
        } else {
            list = comps_doc_categories(parsed->comps_doc);
            COMPS_DocCategory *cat = (COMPS_DocCategory*)list->last->comps_obj;
            COMPS_OBJECT_DESTROY(list);
            comps_doccategory_add_groupid(cat, groupid);
        }
    }
    char *arches = comps_dict_get(elem->attrs, "arch");
    if (arches) {
        //printf("arches :%s\n", arches);
        COMPS_ObjList *larches = __comps_split_arches(arches);
        //printf("larches :%d\n", larches);
        comps_docgroupid_set_arches(groupid, larches);
    }
}
void comps_elem_groupid_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    #define last_groupid ((COMPS_DocGroupId*)list->last->comps_obj)
    COMPS_ObjList *list;
    if (elem->ancestor->type == COMPS_ELEM_OPTLIST) {
        list = comps_doc_environments(parsed->comps_doc);
        COMPS_DocEnv *env = (COMPS_DocEnv*)list->last->comps_obj;
        COMPS_OBJECT_DESTROY(list);
        list = env->option_list;
        comps_docgroupid_set_name(last_groupid, parsed->tmp_buffer, 1);
    } else {
        if (elem->ancestor->ancestor->type == COMPS_ELEM_ENV) {
            list = comps_doc_environments(parsed->comps_doc);
            COMPS_DocEnv *env = (COMPS_DocEnv*)list->last->comps_obj;
            COMPS_OBJECT_DESTROY(list);
            list = env->group_list;
            comps_docgroupid_set_name(last_groupid, parsed->tmp_buffer, 1);
        } else {
            list = comps_doc_categories(parsed->comps_doc);
            COMPS_DocCategory *cat = (COMPS_DocCategory*)list->last->comps_obj;
            COMPS_OBJECT_DESTROY(list);
            list = cat->group_ids;
            comps_docgroupid_set_name(last_groupid, parsed->tmp_buffer, 1);
        }
    }
    parsed->tmp_buffer = NULL;
}
void comps_elem_match_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    comps_doc_add_langpack(parsed->comps_doc,
                           comps_dict_get(elem->attrs, "name"),
                           comps_str(comps_dict_get(elem->attrs, "install")));
}
void comps_elem_package_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    comps_doc_add_blacklist(parsed->comps_doc,
                            comps_dict_get(elem->attrs, "name"),
                            comps_str((comps_dict_get(elem->attrs, "arch"))));
}
void comps_elem_ignoredep_preproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    comps_doc_add_whiteout(parsed->comps_doc,
                           comps_dict_get(elem->attrs, "requires"),
                           comps_str((comps_dict_get(elem->attrs, "package"))));
}
void comps_elem_idnamedesc_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_ObjDict *props, *name_by_lang, *desc_by_lang;
    COMPS_ObjList *list;
    char *lang;

    if (elem->ancestor->type == COMPS_ELEM_GROUP) {
        list = comps_doc_groups(parsed->comps_doc);
        COMPS_DocGroup *group = (COMPS_DocGroup*)list->last->comps_obj;
        props = group->properties;
        name_by_lang = group->name_by_lang;
        desc_by_lang = group->desc_by_lang;
    } else if (elem->ancestor->type == COMPS_ELEM_CATEGORY) {
        list = comps_doc_categories(parsed->comps_doc);
        COMPS_DocCategory *cat = (COMPS_DocCategory*)list->last->comps_obj;
        props = cat->properties;
        name_by_lang = cat->name_by_lang;
        desc_by_lang = cat->desc_by_lang;
    } else {
        list = comps_doc_environments(parsed->comps_doc);
        COMPS_DocEnv *env = (COMPS_DocEnv*)list->last->comps_obj;
        props = env->properties;
        name_by_lang = env->name_by_lang;
        desc_by_lang = env->desc_by_lang;
    }
    COMPS_OBJECT_DESTROY(list);
    if (!parsed->tmp_buffer) {
        comps_log_error_x(parsed->log, COMPS_ERR_NOCONTENT, 3,
                        comps_str(COMPS_ElemInfos[elem->type]->name),
                        comps_num(parser_line),
                        comps_num(parser_col));
        return;
    }
    if (elem->type == COMPS_ELEM_ID) {
        __comps_check_allready_set(comps_objdict_get(props, "id"), "id",parsed);
        comps_objdict_set_x(props, "id",
                            (COMPS_Object*)comps_str(parsed->tmp_buffer));
        //printf("id set %s\n", parsed->tmp_buffer);
    } else if (elem->type == COMPS_ELEM_NAME) {
        if ((lang = comps_dict_get(elem->attrs, "xml:lang"))) {
            comps_objdict_set_x(name_by_lang, lang,
                                (COMPS_Object*)comps_str(parsed->tmp_buffer));
        } else {
            __comps_check_allready_set(comps_objdict_get(props, "name"),
                                       "name", parsed);
            //printf("name set %s\n", parsed->tmp_buffer);
            comps_objdict_set_x(props, "name",
                                (COMPS_Object*)comps_str(parsed->tmp_buffer));
        }
    } else {
        if ((lang = comps_dict_get(elem->attrs, "xml:lang"))) {
            comps_objdict_set_x(desc_by_lang, lang,
                                (COMPS_Object*)comps_str(parsed->tmp_buffer));
        } else {
            __comps_check_allready_set(comps_objdict_get(props, "desc"),
                                       "desc", parsed);
            comps_objdict_set_x(props, "desc",
                                (COMPS_Object*)comps_str(parsed->tmp_buffer));
        }
    }
    parsed->tmp_buffer = NULL;
}
void comps_elem_default_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    (void)elem;
    #define last_group ((COMPS_DocGroup*)list->last->comps_obj)
    COMPS_ObjList *list = comps_doc_groups(parsed->comps_doc);
    if (__comps_strcmp(parsed->tmp_buffer, "false"))
        comps_docgroup_set_def(last_group, 0, false);
    else if (__comps_strcmp(parsed->tmp_buffer, "true"))
        comps_docgroup_set_def(last_group, 1, false);
    else {
        comps_log_warning_x(parsed->log, COMPS_ERR_DEFAULT_PARAM, 3,
                            comps_str(parsed->tmp_buffer),
                            comps_num(parser_line),
                            comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(list);
    parsed->tmp_buffer = NULL;
}
void comps_elem_langonly_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    (void)elem;
    COMPS_ObjList *list = comps_doc_groups(parsed->comps_doc);
    __comps_check_allready_set(comps_objdict_get(last_group->properties,
                                                 "langonly"),
                               "langonly", parsed);
    comps_docgroup_set_langonly(last_group, parsed->tmp_buffer, 1);
    COMPS_OBJECT_DESTROY(list);
    parsed->tmp_buffer = NULL;
}
void comps_elem_uservisible_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    (void)elem;
    COMPS_ObjList *list = comps_doc_groups(parsed->comps_doc);
    if (strcmp(parsed->tmp_buffer, "false") == 0)
        comps_docgroup_set_uservisible(last_group, 0, false);
    else if (strcmp(parsed->tmp_buffer, "true") == 0)
        comps_docgroup_set_uservisible(last_group, 1, false);
    else {
        comps_log_warning_x(parsed->log, COMPS_ERR_DEFAULT_PARAM, 3,
                            comps_str(parsed->tmp_buffer),
                            comps_num(parser_line),
                            comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(list);
    parsed->tmp_buffer = NULL;
}
void comps_elem_biarchonly_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    (void)elem;
    COMPS_ObjList *list = comps_doc_groups(parsed->comps_doc);
    if (strcmp(parsed->tmp_buffer, "false") == 0)
        comps_docgroup_set_biarchonly(last_group, 0, false);
    else if (strcmp(parsed->tmp_buffer, "true") == 0)
        comps_docgroup_set_biarchonly(last_group, 1, false);
    else {
        comps_log_warning_x(parsed->log, COMPS_ERR_DEFAULT_PARAM, 3,
                            comps_str(parsed->tmp_buffer),
                            comps_num(parser_line),
                            comps_num(parser_col));
    }
    COMPS_OBJECT_DESTROY(list);
    #undef last_group
    parsed->tmp_buffer = NULL;
}
void comps_elem_display_order_postproc(COMPS_Parsed *parsed, COMPS_Elem *elem) {
    COMPS_Object* prop;
    COMPS_ObjDict *dict;
    COMPS_ObjList *list;

    if (elem->ancestor->type == COMPS_ELEM_CATEGORY) {
        list = comps_doc_categories(parsed->comps_doc);
        COMPS_DocCategory *category = (COMPS_DocCategory*)list->last->comps_obj;
        dict = category->properties;
        prop = comps_doccategory_get_display_order(category);
    } else if (elem->ancestor->type == COMPS_ELEM_ENV) {
        list = comps_doc_environments(parsed->comps_doc);
        COMPS_DocEnv *env = (COMPS_DocEnv*)list->last->comps_obj;
        dict = env->properties;
        prop = comps_docenv_get_display_order(env);
    } else {
        list = comps_doc_groups(parsed->comps_doc);
        COMPS_DocGroup *group = (COMPS_DocGroup*)list->last->comps_obj;
        dict = group->properties;
        prop = comps_docgroup_get_display_order(group);
    }
    COMPS_OBJECT_DESTROY(list);
    if (prop) {
        comps_log_warning_x(parsed->log, COMPS_ERR_ELEM_ALREADYSET, 3,
                            comps_str(elem->name), comps_num(parser_line),
                            comps_num(parser_col));
    } else if (dict) {
        prop = (COMPS_Object*)comps_num(0);
        comps_objdict_set_x(dict, "display_order", prop);
    }
    if (prop)
        sscanf(parsed->tmp_buffer, "%d", &((COMPS_Num*)prop)->val);
    parsed->tmp_buffer = NULL;
}
    #undef parser_line
    #undef parser_col

void comps_elem_attr_destroy(void *attr) {
    if (attr == NULL) return;
    free(((COMPS_ElemAttr*)attr)->name);
    free(((COMPS_ElemAttr*)attr)->val);
    free(attr);
}

COMPS_ElemAttr * comps_elem_attr_create(const char *name, const char *val) {

    COMPS_ElemAttr *attr;
    if ((attr = malloc(sizeof(*attr))) == NULL)
        return NULL;
    if ((attr->name = malloc(sizeof(char)*(strlen(name)+1))) == NULL) {
        free(attr);
        return NULL;
    }
    if ((attr->val = malloc(sizeof(char)*(strlen(val)+1))) == NULL) {
        free(attr->name);
        free(attr);
        return NULL;
    }
    memcpy(attr->name, name, sizeof(char)*(strlen(name)+1));
    memcpy(attr->val, val, sizeof(char)*(strlen(val)+1));
    return attr;
}

COMPS_Elem* comps_elem_create(const char * s, const char ** attrs,
                              COMPS_ElemType type) {
    COMPS_Elem *elem;
    char *val;
    if ((elem = malloc(sizeof(COMPS_Elem))) == NULL)
        return NULL;

    elem->type = type;
    if (type == COMPS_ELEM_UNKNOWN) {
        if ((elem->name = malloc(sizeof(char) * (strlen(s)+1))) == NULL) {
            free(elem);
            return NULL;
        }
        memcpy(elem->name, s, (strlen(s)+1) * sizeof(char));
    } else {
        elem->name = NULL;
    }

    /* Add attributes to elem */
    elem->attrs = comps_dict_create(NULL, NULL, &free);
    if (attrs != NULL) {
        for (; *attrs != NULL; attrs += 2) {
            val = malloc((strlen(*(attrs+1))+1)*sizeof(char));
            if (val == NULL) {
                comps_dict_destroy(elem->attrs);
                free(elem->name);
                free(elem);
                return NULL;
            }
            memcpy(val, *(attrs+1), sizeof(char) * (strlen(*(attrs+1))+1));
            comps_dict_set(elem->attrs, (char*)*attrs, val);
        }
    }
    return elem;
}

void comps_elem_destroy(void * elem)
{
    if (elem == NULL) return;
    if (((COMPS_Elem*)elem)->name) free(((COMPS_Elem*)elem)->name);
    comps_dict_destroy(((COMPS_Elem*)elem)->attrs);
    free(elem);
}
