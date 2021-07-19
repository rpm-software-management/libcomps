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

#include "comps_docgroup.h"
#include "comps_set.h"

void comps_docgroup_create(COMPS_DocGroup* group, COMPS_Object **args) {
    (void)args;
    group->properties = COMPS_OBJECT_CREATE(COMPS_ObjDict, NULL);
    group->name_by_lang = COMPS_OBJECT_CREATE(COMPS_ObjDict, NULL);
    group->desc_by_lang = COMPS_OBJECT_CREATE(COMPS_ObjDict, NULL);
    group->packages = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
}
COMPS_CREATE_u(docgroup, COMPS_DocGroup)

void comps_docgroup_copy(COMPS_DocGroup *group_dst,
                         COMPS_DocGroup *group_src) {
    group_dst->properties = (COMPS_ObjDict*)comps_object_copy(
                                       (COMPS_Object*)group_src->properties);
    group_dst->name_by_lang = (COMPS_ObjDict*)comps_object_copy(
                                    (COMPS_Object*)group_src->name_by_lang);
    group_dst->desc_by_lang = (COMPS_ObjDict*)comps_object_copy(
                                    (COMPS_Object*)group_src->desc_by_lang);
    group_dst->packages = (COMPS_ObjList*)comps_object_copy(
                                    (COMPS_Object*)group_src->packages);
}
COMPS_COPY_u(docgroup, COMPS_DocGroup)    /*comps_utils.h macro*/

static void comps_docgroup_destroy(COMPS_DocGroup *group) {
    
    COMPS_OBJECT_DESTROY(group->properties);
    COMPS_OBJECT_DESTROY(group->name_by_lang);
    COMPS_OBJECT_DESTROY(group->desc_by_lang);
    COMPS_OBJECT_DESTROY(group->packages);
}
COMPS_DESTROY_u(docgroup, COMPS_DocGroup) /*comps_utils.h macro*/

COMPS_STRPROP_SETTER(group, COMPS_DocGroup, id) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(group, COMPS_DocGroup, name) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(group, COMPS_DocGroup, desc) /*comps_utils.h macro*/
COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, def) /*comps_utils.h macro*/
COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, uservisible) /*comps_utils.h macro*/
COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, biarchonly) /*comps_utils.h macro*/
COMPS_NUMPROP_SETTER(group, COMPS_DocGroup, display_order) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(group, COMPS_DocGroup, langonly) /*comps_utils.h macro*/

COMPS_PROP_GETTER(group, COMPS_DocGroup, id) /*comps_utils.h macro*/
COMPS_PROP_GETTER_OBJ(group, COMPS_DocGroup, id) /*comps_utils.h macro*/
COMPS_PROP_GETTER(group, COMPS_DocGroup, name) /*comps_utils.h macro*/
COMPS_PROP_GETTER(group, COMPS_DocGroup, desc) /*comps_utils.h macro*/
COMPS_PROP_GETTER(group, COMPS_DocGroup, def) /*comps_utils.h macro*/
COMPS_PROP_GETTER(group, COMPS_DocGroup, uservisible) /*comps_utils.h macro*/
COMPS_PROP_GETTER(group, COMPS_DocGroup, biarchonly) /*comps_utils.h macro*/
COMPS_PROP_GETTER(group, COMPS_DocGroup, display_order) /*comps_utils.h macro*/
COMPS_PROP_GETTER(group, COMPS_DocGroup, langonly) /*comps_utils.h macro*/

COMPS_DOCOBJ_GETOBJLIST(docgroup, COMPS_DocGroup, packages, packages)
COMPS_DOCOBJ_SETOBJLIST(docgroup, COMPS_DocGroup, packages, packages)

COMPS_DOCOBJ_GETARCHES(docgroup, COMPS_DocGroup)/*comps_utils.h macro*/
COMPS_DOCOBJ_SETARCHES(docgroup, COMPS_DocGroup)/*comps_utils.h macro*/

void comps_docgroup_add_package(COMPS_DocGroup *group,
                                COMPS_DocGroupPackage *package) {

    if (group == NULL || package == NULL) {
        assert(0);
        return;
    }
    if (group->packages == NULL) {
        group->packages = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    }
    comps_objlist_append_x(group->packages, (COMPS_Object*)package);
}


COMPS_ObjList* comps_docgroup_get_packages(COMPS_DocGroup *group, char *name,
                                       COMPS_PackageType type) {
    COMPS_ObjListIt *it;
    COMPS_ObjList *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    #define package ((COMPS_DocGroupPackage*)it->comps_obj)
    COMPS_Str *objname = comps_str(name);
    if (!group) return NULL;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    if (name != NULL) matched_max++;
    if (type != COMPS_PACKAGE_UNKNOWN) matched_max++;

    for (it = (group->packages)?group->packages->first:NULL;
         it != NULL; it = it->next) {
        matched = 0;
        if ((name != NULL) && COMPS_OBJECT_CMP(package->name, objname))
            matched++;
        if ((type != COMPS_PACKAGE_UNKNOWN) && package->type  == type)
            matched++;
        if (matched == matched_max) {
            comps_objlist_append(ret, (COMPS_Object*)package);
        }
    }
    COMPS_OBJECT_DESTROY(objname);
    return ret;
}


signed char comps_docgroup_cmp_u(COMPS_Object *group1, COMPS_Object *group2) {
    #define _group1 ((COMPS_DocGroup*)group1)
    #define _group2 ((COMPS_DocGroup*)group2)

    if (!comps_object_cmp((COMPS_Object*)_group1->properties,
                          (COMPS_Object*)_group2->properties)) {
        printf("Group properties cmp fail\n");
        return 0;
    }
    //printf("group cmp properties pass\n");
    if (!comps_object_cmp((COMPS_Object*)_group1->name_by_lang,
                          (COMPS_Object*)_group2->name_by_lang)) {
        //printf("Group name_by_lang cmp fail\n");
        return 0;
    }
    //printf("group cmp name_by_lang pass\n");
    if (!comps_object_cmp((COMPS_Object*)_group1->desc_by_lang,
                          (COMPS_Object*)_group2->desc_by_lang)) {
        //printf("Group desc_by_lang cmp fail\n");
        return 0;
    }
    //printf("group cmp desc_by_lang pass\n");
    if (!comps_object_cmp((COMPS_Object*)_group1->packages,
                          (COMPS_Object*)_group2->packages)) {
        //printf("Group packages cmp fail\n");
        return 0;
    }
    //printf("group cmp packages pass\n");
    return 1;
    #undef _group1
    #undef _group2
}

char __comps_docgroup_idcmp(void *g1, void *g2) {
    COMPS_Object *obj1, *obj2;
    char ret;
    obj1 = comps_objdict_get(((COMPS_DocGroup*)g1)->properties, "id");
    obj2 = comps_objdict_get(((COMPS_DocGroup*)g2)->properties, "id");
    ret = comps_object_cmp(obj1, obj2);
    COMPS_OBJECT_DESTROY(obj1);
    COMPS_OBJECT_DESTROY(obj2);
    return ret;
}

COMPS_DocGroup* comps_docgroup_union(COMPS_DocGroup *g1, COMPS_DocGroup *g2) {
    COMPS_DocGroup *res;
    COMPS_ObjListIt *it;
    COMPS_Set *set;
    COMPS_DocGroupPackage *pkg;

    res = COMPS_OBJECT_CREATE(COMPS_DocGroup, NULL);
    COMPS_OBJECT_DESTROY(res->properties);

    res->properties = comps_objdict_union(g1->properties, g2->properties);
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, (void(*)(void*))&comps_object_destroy,
                                    &comps_docpackage_cmp_set);
    it = g1->packages?g1->packages->first:NULL;
    for (; it != NULL; it = it->next) {
        pkg = (COMPS_DocGroupPackage*) comps_object_copy(it->comps_obj);
        comps_set_add(set, (void*)comps_object_incref((COMPS_Object*)pkg));
        comps_docgroup_add_package(res, pkg);
    }
    void *data;
    int index;
    it = g2->packages?g2->packages->first:NULL;
    for (; it != NULL; it = it->next) {
        if ((data = comps_set_data_at(set, (void*)it->comps_obj)) != NULL) {
            index = comps_objlist_index(res->packages, (COMPS_Object*)data);
            comps_objlist_remove_at(res->packages, index);
            comps_objlist_insert_at_x(res->packages, index,
                                      comps_object_copy(it->comps_obj));
        } else {
            comps_docgroup_add_package(res, (COMPS_DocGroupPackage*)
                                            comps_object_copy(it->comps_obj));
        }
    }
    comps_set_destroy(&set);
    comps_object_destroy((COMPS_Object*)res->name_by_lang);
    comps_object_destroy((COMPS_Object*)res->desc_by_lang);
    res->name_by_lang = comps_objdict_union(g1->name_by_lang, g2->name_by_lang);
    res->desc_by_lang = comps_objdict_union(g1->desc_by_lang, g2->desc_by_lang);
    return res;
}

COMPS_DocGroup* comps_docgroup_intersect(COMPS_DocGroup *g1,
                                         COMPS_DocGroup *g2) {
    COMPS_DocGroup *res;
    COMPS_ObjListIt *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_DocGroupPackage *newpkg;
    COMPS_HSList *pairs1, *pairs2;

    res = COMPS_OBJECT_CREATE(COMPS_DocGroup, NULL);
    set = comps_set_create();
    //comps_objrtree_paircmp(void *obj1, void *obj2) {
    comps_set_init(set, NULL, NULL, NULL, &comps_objrtree_paircmp);

    pairs1 = comps_objdict_pairs(g1->properties);
    for (hsit = pairs1->first; hsit != NULL; hsit = hsit->next) {
        comps_set_add(set, hsit->data);
    }
    pairs2 = comps_objdict_pairs(g2->properties);
    for (hsit = pairs2->first; hsit != NULL; hsit = hsit->next) {
        if (comps_set_in(set, hsit->data)) {
            comps_objdict_set(res->properties,
                              ((COMPS_RTreePair*)hsit->data)->key,
                              ((COMPS_RTreePair*)hsit->data)->data);
        }
    }
    comps_hslist_destroy(&pairs1);
    comps_hslist_destroy(&pairs2);
    comps_set_clear(set);
    //res->packages = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo,
    //                                                    NULL);

    //set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_docpackage_cmp_set);

    for (it = g1->packages->first; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    for (it = g2->packages->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            newpkg = (COMPS_DocGroupPackage*)
                     comps_object_copy(it->comps_obj);
            comps_docgroup_add_package(res, newpkg);
        }
    }
    comps_set_destroy(&set);
    return res;
}



signed char comps_docgroup_xml(COMPS_DocGroup *group, xmlTextWriterPtr writer,
                               COMPS_Log *log, COMPS_XMLOptions *xml_options,
                               COMPS_DefaultsOptions *def_options) {
    COMPS_ObjListIt *it;
    COMPS_Object *obj;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;

    static char* props[] = {"id", "name", "name", "desc",
                            "desc", "def", "uservisible", "biarchonly",
                            "display_order", "langonly"};
    static size_t type[] =   {0, 0, offsetof(COMPS_DocGroup, name_by_lang),
                              0, offsetof(COMPS_DocGroup, desc_by_lang), 0,
                              0, 0, 0, 0};
    static char* aliases[] = {NULL, NULL, NULL, "description", "description",
                              "default", NULL, NULL, NULL, NULL};
    static bool explicit[] = {true, true, true, true, true, false, false,
                              false, true, true};
    const char *str_true = "true";
    const char *str_false = "false";
    const char *default_val[] = {NULL, NULL, NULL, NULL, NULL,
                           def_options->default_default?str_true:str_false,
                           def_options->default_uservisible?str_true:str_false,
                           def_options->default_biarchonly?str_true:str_false,
                           NULL, NULL};

    static char*(*tostrf[])(COMPS_Object*) = {&comps_object_tostr,
                                               &comps_object_tostr,
                                               &comps_object_tostr,
                                               &comps_object_tostr,
                                               &comps_object_tostr,
                                               &__comps_num2boolstr,
                                               &__comps_num2boolstr,
                                               &__comps_num2boolstr,
                                               &comps_object_tostr,
                                               &comps_object_tostr};
    char *str;
    int ret;
    explicit[5] = xml_options->default_explicit;
    explicit[6] = xml_options->uservisible_explicit;
    explicit[7] = xml_options->biarchonly_explicit;
    if (group->packages->len == 0 && !xml_options->empty_groups) {
        obj = comps_docgroup_get_id(group);
        comps_log_error(log, COMPS_ERR_PKGLIST_EMPTY, 1, obj);
        COMPS_OBJECT_DESTROY(obj);
        return 1;
    }
    ret = xmlTextWriterStartElement(writer, BAD_CAST "group");
    COMPS_XMLRET_CHECK()
    if (xml_options->arch_output) {
        obj = (COMPS_Object*)comps_docgroup_arches(group);
        ret = __comps_xml_arch(obj, writer);
        COMPS_XMLRET_CHECK()
        COMPS_OBJECT_DESTROY(obj);
    }
    for (int i=0; i<10; i++) {
        //printf("%s\n", props[i]);
        if (!type[i]) {
            obj = comps_objdict_get_x(group->properties, props[i]);
            if (explicit[i]) {
                if (obj) {
                    str = tostrf[i](obj);
                    __comps_xml_prop((aliases[i])?aliases[i]:props[i], str, writer);
                    free(str);
                } else if (default_val[i]){
                    __comps_xml_prop((aliases[i])?aliases[i]:props[i],
                                     (char*)default_val[i], writer);
                }
            } else {
                if (obj) {
                    str = tostrf[i](obj);
                    if (!default_val[i] || (strcmp(str, default_val[i]) != 0)) {
                        __comps_xml_prop((aliases[i])?aliases[i]:props[i], str,
                                         writer);
                    }
                    free(str);
                }
            }
        } else {
            if (*(void**)(((char*)group)+type[i]) == (void*)group->name_by_lang){
                pairlist = comps_objdict_pairs(group->name_by_lang);
            } else {
                pairlist = comps_objdict_pairs(group->desc_by_lang);
            }
            for (hsit = pairlist->first; hsit != NULL; hsit = hsit->next) {
                ret = xmlTextWriterStartElement(writer,
                            (const xmlChar*)((aliases[i])?aliases[i]:props[i]));
                COMPS_XMLRET_CHECK(comps_hslist_destroy(&pairlist))

                ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                            (xmlChar*) ((COMPS_ObjRTreePair*)hsit->data)->key);
                COMPS_XMLRET_CHECK(comps_hslist_destroy(&pairlist))
                str = tostrf[i](((COMPS_ObjRTreePair*)hsit->data)->data);
                ret = xmlTextWriterWriteString(writer, (xmlChar*)str);
                COMPS_XMLRET_CHECK(comps_hslist_destroy(&pairlist))
                free(str);
                ret = xmlTextWriterEndElement(writer);
                COMPS_XMLRET_CHECK(comps_hslist_destroy(&pairlist))
            }
            comps_hslist_destroy(&pairlist);
        }
    }
    if (group->packages->len || xml_options->empty_packages) {
        ret = xmlTextWriterStartElement(writer, (xmlChar*)"packagelist");
        COMPS_XMLRET_CHECK()
        for (it = group->packages->first; it != NULL; it = it->next) {
            comps_docpackage_xml((COMPS_DocGroupPackage*)it->comps_obj,
                                 writer, log, xml_options, def_options);
        }
        ret = xmlTextWriterEndElement(writer);
        COMPS_XMLRET_CHECK()
    }
    ret = xmlTextWriterEndElement(writer);
    COMPS_XMLRET_CHECK()
    return 0;
}

char* comps_docgroup_tostr_u(COMPS_Object* group) {
    #define _group_ ((COMPS_DocGroup*)group)
    char *ret, *name_by_lang_str, *desc_by_lang_str, *group_packages_str;
    int total_len = 0;
    const char *head = "<COMPS_Group ";
    COMPS_Object *tmpprop;
    COMPS_Object*(*getters[])(COMPS_DocGroup*)= {comps_docgroup_get_id,
                                     comps_docgroup_get_name,
                                     comps_docgroup_get_desc,
                                     comps_docgroup_get_display_order,
                                     comps_docgroup_get_def,
                                     comps_docgroup_get_langonly};
    char *props[6];
    for (int i=0; i<6; i++) {
        tmpprop = getters[i](_group_);
        props[i] = comps_object_tostr(tmpprop);
        total_len += strlen(props[i]);
        COMPS_OBJECT_DESTROY(tmpprop);
    }
    name_by_lang_str = comps_object_tostr((COMPS_Object*)_group_->name_by_lang);
    total_len += strlen(name_by_lang_str);
    desc_by_lang_str = comps_object_tostr((COMPS_Object*)_group_->desc_by_lang);
    total_len += strlen(desc_by_lang_str);
    group_packages_str = comps_object_tostr((COMPS_Object*)_group_->packages);
    total_len += strlen(group_packages_str);

    if ((ret = malloc(sizeof(char) * (total_len+2+(8*2)+strlen(head)))) == NULL) {
        free(name_by_lang_str);
        free(desc_by_lang_str);
        free(group_packages_str);
        return NULL;
    }
    ret[0] = 0;
    strcat(ret, head);
    for (int i=0; i<6; i++) {
        strcat(ret, props[i]);
        free(props[i]);
        strcat(ret, ", ");
    }
    strcat(ret, name_by_lang_str);
    free(name_by_lang_str);
    strcat(ret, ", ");
    strcat(ret, desc_by_lang_str);
    free(desc_by_lang_str);
    strcat(ret, ", ");
    strcat(ret, group_packages_str);
    free(group_packages_str);
    strcat(ret, ">");
    return ret;
    #undef _group_
}

COMPS_DocGroup* comps_docgroup_arch_filter(COMPS_DocGroup *source,
                                           COMPS_ObjList *arches) {
    COMPS_ObjList *arches2;
    COMPS_DocGroup *ret = COMPS_OBJECT_CREATE(COMPS_DocGroup, NULL);
    COMPS_OBJECT_DESTROY(ret->properties);
    ret->properties = (COMPS_ObjDict*)COMPS_OBJECT_COPY(source->properties);
    COMPS_OBJECT_DESTROY(ret->name_by_lang);
    ret->name_by_lang = (COMPS_ObjDict*)COMPS_OBJECT_COPY(source->name_by_lang);
    COMPS_OBJECT_DESTROY(ret->desc_by_lang);
    ret->desc_by_lang = (COMPS_ObjDict*)COMPS_OBJECT_COPY(source->desc_by_lang);
    for (COMPS_ObjListIt *it = source->packages->first;
         it != NULL; it = it->next) {
        arches2 = comps_docpackage_arches((COMPS_DocGroupPackage*)it->comps_obj);
        if ((!arches2) || (__comps_objlist_intersected(arches, arches2))) {
            comps_docgroup_add_package(ret, (COMPS_DocGroupPackage*)
                                            comps_object_copy(it->comps_obj));
        }
        COMPS_OBJECT_DESTROY(arches2);
    }
    return ret;
}

COMPS_ObjectInfo COMPS_DocGroup_ObjInfo = {
    .obj_size = sizeof(COMPS_DocGroup),
    .constructor = &comps_docgroup_create_u,
    .destructor = &comps_docgroup_destroy_u,
    .copy = &comps_docgroup_copy_u,
    .obj_cmp = &comps_docgroup_cmp_u,
    .to_str = &comps_docgroup_tostr_u
};

COMPS_ValRuleGeneric* COMPS_DocGroup_ValidateRules[] = {
    (COMPS_ValRuleGeneric*)&(COMPS_ValRuleProp){COMPS_VAL_RULE_PROP,
                         .verbose_msg = "Group id check: ",
                         .get_f = (COMPS_VAL_GETF) &comps_docgroup_get_id,
                         .check_f = &comps_empty_check},
    (COMPS_ValRuleGeneric*)&(COMPS_ValRuleList){COMPS_VAL_RULE_LIST,
                         .verbose_msg = "Group unique package list check: ",
                         .offset = offsetof(COMPS_DocGroup, packages),
                         .check_f = &comps_objlist_unique_check},
    NULL
};
