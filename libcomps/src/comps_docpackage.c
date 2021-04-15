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

#include "comps_docpackage.h"

static void comps_docpackage_create(COMPS_DocGroupPackage* package, COMPS_Object **args) {
    (void)args;
    package->name = NULL;
    package->requires = NULL;
    package->basearchonly = NULL;
    package->arches = NULL;
    package->type = COMPS_PACKAGE_UNKNOWN;
}
COMPS_CREATE_u(docpackage, COMPS_DocGroupPackage)

static void comps_docpackage_copy(COMPS_DocGroupPackage *pkg_dst,
                           COMPS_DocGroupPackage *pkg_src) {
    pkg_dst->name = (COMPS_Str*)comps_object_copy((COMPS_Object*)pkg_src->name);
    pkg_dst->requires = (COMPS_Str*)comps_object_copy(
                                            (COMPS_Object*)pkg_src->requires);
    pkg_dst->basearchonly = (COMPS_Num*)comps_object_copy(
                                          (COMPS_Object*)pkg_src->basearchonly);
    pkg_dst->arches = (COMPS_ObjList*)comps_object_copy(
                                          (COMPS_Object*)pkg_src->arches);
    pkg_dst->type = pkg_src->type;
}
COMPS_COPY_u(docpackage, COMPS_DocGroupPackage)    /*comps_utils.h macro*/

static void comps_docpackage_destroy(COMPS_DocGroupPackage *pkg) {
    comps_object_destroy((COMPS_Object*)pkg->name);
    comps_object_destroy((COMPS_Object*)pkg->requires);
    comps_object_destroy((COMPS_Object*)pkg->basearchonly);
    comps_object_destroy((COMPS_Object*)pkg->arches);
}
COMPS_DESTROY_u(docpackage, COMPS_DocGroupPackage) /*comps_utils.h macro*/

void comps_docpackage_set_name(COMPS_DocGroupPackage *pkg, char *name, char copy) {
    (void)copy;
    if (pkg->name)
        comps_object_destroy((COMPS_Object*)pkg->name);
    pkg->name = comps_str(name);
}

COMPS_Object* comps_docpackage_get_name(COMPS_DocGroupPackage *pkg) {
    return comps_object_incref((COMPS_Object*)pkg->name);
}

void comps_docpackage_set_requires(COMPS_DocGroupPackage *pkg, char *requires, char copy) {
    (void)copy;
    if (pkg->requires)
        comps_object_destroy((COMPS_Object*)pkg->requires);
    pkg->requires = comps_str(requires);
}

COMPS_Object* comps_docpackage_get_requires(COMPS_DocGroupPackage *pkg) {
    return comps_object_incref((COMPS_Object*)pkg->requires);
}

void comps_docpackage_set_basearchonly(COMPS_DocGroupPackage *pkg,
                                       int basearchonly, bool unset) {
    (void)unset;
    if (pkg->basearchonly) {
        COMPS_OBJECT_DESTROY(pkg->basearchonly);
    }
    pkg->basearchonly = comps_num(basearchonly);
}

COMPS_Object* comps_docpackage_get_basearchonly(COMPS_DocGroupPackage *pkg) {
    return comps_object_incref((COMPS_Object*)pkg->basearchonly);
}

void comps_docpackage_set_type_i(COMPS_DocGroupPackage *pkg, int type, bool unset) {
    (void)unset;
    pkg->type = type;
}

void comps_docpackage_set_type(COMPS_DocGroupPackage *pkg,
                                   COMPS_PackageType type, bool unset) {
    (void)unset;
    pkg->type = type;
}

COMPS_Object* comps_docpackage_get_type(COMPS_DocGroupPackage *pkg) {
    return (COMPS_Object*)comps_num(pkg->type);
}


signed char comps_docpackage_cmp_u(COMPS_Object *pkg1, COMPS_Object *pkg2) {
    #define _pkg1 ((COMPS_DocGroupPackage*)pkg1)
    #define _pkg2 ((COMPS_DocGroupPackage*)pkg2)

    if (!comps_object_cmp((COMPS_Object*)_pkg1->name,
                          (COMPS_Object*)_pkg2->name)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_pkg1->requires,
                          (COMPS_Object*)_pkg2->requires)) return 0;
    if (_pkg1->type != _pkg2->type) return 0;
    
    return 1;

    #undef _pkg1
    #undef _pkg2
}

char __comps_docpackage_idcmp(void *pkg1, void *pkg2) {
    return comps_object_cmp((COMPS_Object*)((COMPS_DocGroupPackage*)pkg1)->name,
                           (COMPS_Object*)((COMPS_DocGroupPackage*)pkg2)->name);
}

const char* comps_docpackage_type_str(COMPS_PackageType type) {
    switch(type){
        case COMPS_PACKAGE_OPTIONAL:
            return "optional";
        case COMPS_PACKAGE_MANDATORY:
            return "mandatory";
        case COMPS_PACKAGE_CONDITIONAL:
            return "conditional";
        default:
            return "default";
    }
}

inline char comps_docpackage_cmp_set(void *pkg1, void *pkg2) {
    return COMPS_OBJECT_CMP(((COMPS_DocGroupPackage*)pkg1)->name,
                            ((COMPS_DocGroupPackage*)pkg2)->name);
}

signed char comps_docpackage_xml(COMPS_DocGroupPackage *pkg,
                                 xmlTextWriterPtr writer,
                                 COMPS_Log *log, COMPS_XMLOptions *xml_options,
                                 COMPS_DefaultsOptions *def_options) {
    char *str;
    int ret;
    bool bao_def = false;
    (void)def_options;

    ret = xmlTextWriterStartElement(writer, BAD_CAST "packagereq");
    COMPS_XMLRET_CHECK()
    if (xml_options->arch_output) {
        COMPS_Object *obj = (COMPS_Object*)pkg->arches;
        ret = __comps_xml_arch(obj, writer);
        COMPS_XMLRET_CHECK()
    }
    if (pkg->type == COMPS_PACKAGE_OPTIONAL)
        str = "optional";
    else if (pkg->type == COMPS_PACKAGE_MANDATORY)
        str = "mandatory";
    else if (pkg->type == COMPS_PACKAGE_CONDITIONAL)
        str = "conditional";
    else
        str = "default";

    ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST str);

    if (pkg->requires) {
        str = comps_object_tostr((COMPS_Object*)pkg->requires);
        if (str && *str) {
            ret = xmlTextWriterWriteAttribute(writer, (xmlChar*) "requires", BAD_CAST str);
        }
        free(str);
    }
    COMPS_XMLRET_CHECK()
    if (xml_options->bao_explicit) {
        if (pkg->basearchonly) {
            ret = xmlTextWriterWriteAttribute(writer, (xmlChar*) "basearchonly",
                                                BAD_CAST "true");
        } else {
            ret = xmlTextWriterWriteAttribute(writer, (xmlChar*) "basearchonly",
                                                BAD_CAST "false");
        }
    } else {
        if (pkg->basearchonly && pkg->basearchonly->val != bao_def) {
            if (pkg->basearchonly) {
                ret = xmlTextWriterWriteAttribute(writer, (xmlChar*) "basearchonly",
                                                    BAD_CAST "true");
            } else {
                ret = xmlTextWriterWriteAttribute(writer, (xmlChar*) "basearchonly",
                                                    BAD_CAST "false");
            }
        }
    }
    COMPS_XMLRET_CHECK()
    str = comps_object_tostr((COMPS_Object*)pkg->name);
    ret = xmlTextWriterWriteString(writer, (xmlChar*)str);
    free(str);
    COMPS_XMLRET_CHECK()
    ret = xmlTextWriterEndElement(writer);
    COMPS_XMLRET_CHECK()
    return 0;
}

static char* comps_docpackage_str_u(COMPS_Object* docpackage) {
    #define _package_ ((COMPS_DocGroupPackage*)docpackage)
    const size_t len = strlen("<COMPS_DocGroupPackage name='' type='' "
                              "requires='' basearchonly=''>");
    char *name = comps_object_tostr((COMPS_Object*)_package_->name);
    const char *type = comps_docpackage_type_str(_package_->type);
    char *requires = comps_object_tostr((COMPS_Object*)_package_->requires);
    char *basearchonly;
    if ((_package_->basearchonly) && (_package_->basearchonly->val))
        basearchonly = "True";
    else
        basearchonly = "False";
    size_t total_len = len + strlen(name)
                           + strlen(type)
                           + strlen(requires) +
                           + strlen(basearchonly) + 1;
    char *ret = malloc(sizeof(char)*(total_len));
    
    snprintf(ret, total_len, "<COMPS_DocGroupPackage name='%s' type='%s'"
                             " requires='%s' basearchonly='%s'>",
                 name, type, requires, basearchonly);
    free(name);
    free(requires);
    return ret;
}

COMPS_ObjList* comps_docpackage_arches(COMPS_DocGroupPackage *pkg) {
    return (COMPS_ObjList*)comps_object_incref((COMPS_Object*)pkg->arches);
}
void comps_docpackage_set_arches(COMPS_DocGroupPackage *pkg,
                                 COMPS_ObjList *arches) {
    COMPS_OBJECT_DESTROY(pkg->arches);
    pkg->arches = arches;
}

COMPS_ValRuleGeneric* COMPS_DocGroupPackage_ValidateRules[] = {
    (COMPS_ValRuleGeneric*)&(COMPS_ValRuleProp){COMPS_VAL_RULE_PROP,
                         .verbose_msg = "Package name check: ",
                         .get_f = (COMPS_VAL_GETF) &comps_docpackage_get_name,
                         .check_f = &comps_empty_check},
    NULL
};

COMPS_ObjectInfo COMPS_DocGroupPackage_ObjInfo = {
    .obj_size = sizeof(COMPS_DocGroupPackage),
    .constructor = &comps_docpackage_create_u,
    .destructor = &comps_docpackage_destroy_u,
    .copy = &comps_docpackage_copy_u,
    .obj_cmp = &comps_docpackage_cmp_u,
    .to_str = &comps_docpackage_str_u
};
