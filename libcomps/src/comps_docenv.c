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

#include "comps_docenv.h"

void comps_docenv_create(COMPS_DocEnv* env, COMPS_Object **args) {
    (void)args;
    env->properties = (COMPS_ObjDict*)comps_object_create(
                                                &COMPS_ObjDict_ObjInfo, NULL);
    env->name_by_lang = (COMPS_ObjDict*)comps_object_create(
                                                &COMPS_ObjDict_ObjInfo, NULL);
    env->desc_by_lang = (COMPS_ObjDict*)comps_object_create(
                                                &COMPS_ObjDict_ObjInfo, NULL);
    env->group_list = (COMPS_ObjList*)comps_object_create(
                                                &COMPS_ObjList_ObjInfo, NULL);
    env->option_list = (COMPS_ObjList*)comps_object_create(
                                                &COMPS_ObjList_ObjInfo, NULL);
}
COMPS_CREATE_u(docenv, COMPS_DocEnv)

void comps_docenv_copy(COMPS_DocEnv *env_dst, COMPS_DocEnv *env_src) {
    env_dst->properties = (COMPS_ObjDict*)comps_object_copy(
                                       (COMPS_Object*)env_src->properties);
    env_dst->name_by_lang = (COMPS_ObjDict*)comps_object_copy(
                                    (COMPS_Object*)env_src->name_by_lang);
    env_dst->desc_by_lang = (COMPS_ObjDict*)comps_object_copy(
                                    (COMPS_Object*)env_src->desc_by_lang);
    env_dst->group_list = (COMPS_ObjList*)comps_object_copy(
                                    (COMPS_Object*)env_src->group_list);
    env_dst->option_list = (COMPS_ObjList*)comps_object_copy(
                                    (COMPS_Object*)env_src->option_list);
}
COMPS_COPY_u(docenv, COMPS_DocEnv)    /*comps_utils.h macro*/

static void comps_docenv_destroy(COMPS_DocEnv *env) {
    comps_object_destroy((COMPS_Object*)env->properties);
    comps_object_destroy((COMPS_Object*)env->name_by_lang);
    comps_object_destroy((COMPS_Object*)env->desc_by_lang);
    comps_object_destroy((COMPS_Object*)env->group_list);
    comps_object_destroy((COMPS_Object*)env->option_list);
}
COMPS_DESTROY_u(docenv, COMPS_DocEnv) /*comps_utils.h macro*/

COMPS_PROP_GETTER(env, COMPS_DocEnv, id) /*comps_utils.h macro*/
COMPS_PROP_GETTER_OBJ(env, COMPS_DocEnv, id) /*comps_utils.h macro*/
COMPS_PROP_GETTER(env, COMPS_DocEnv, name) /*comps_utils.h macro*/
COMPS_PROP_GETTER(env, COMPS_DocEnv, desc) /*comps_utils.h macro*/
COMPS_PROP_GETTER(env, COMPS_DocEnv, display_order) /*comps_utils.h macro*/

COMPS_STRPROP_SETTER(env, COMPS_DocEnv, id) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(env, COMPS_DocEnv, name) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(env, COMPS_DocEnv, desc) /*comps_utils.h macro*/
COMPS_NUMPROP_SETTER(env, COMPS_DocEnv, display_order) /*comps_utils.h macro*/

COMPS_DOCOBJ_GETOBJLIST(docenv, COMPS_DocEnv, group_list, group_list)
COMPS_DOCOBJ_GETOBJLIST(docenv, COMPS_DocEnv, option_list, option_list)

COMPS_DOCOBJ_SETOBJLIST(docenv, COMPS_DocEnv, group_list, group_list)
COMPS_DOCOBJ_SETOBJLIST(docenv, COMPS_DocEnv, option_list, option_list)

void comps_docenv_add_groupid(COMPS_DocEnv *env,
                            COMPS_DocGroupId *gid) {

    if (env == NULL || gid == NULL) {
        assert(0);
        return;
    }
    if (env->group_list == NULL) {
        env->group_list = (COMPS_ObjList*) comps_object_create(
                                            &COMPS_ObjList_ObjInfo, NULL);
    }
    comps_objlist_append_x(env->group_list, (COMPS_Object*)gid);
}

void comps_docenv_add_optionid(COMPS_DocEnv *env,
                            COMPS_DocGroupId *gid) {

    if (env == NULL || gid == NULL) {
        assert(0);
        return;
    }
    if (env->option_list == NULL) {
        env->option_list = (COMPS_ObjList*) comps_object_create(
                                            &COMPS_ObjList_ObjInfo, NULL);
    }
    comps_objlist_append_x(env->option_list, (COMPS_Object*)gid);
}

signed char comps_docenv_cmp_u(COMPS_Object *env1, COMPS_Object *env2) {
    #define _env1 ((COMPS_DocEnv*)env1)
    #define _env2 ((COMPS_DocEnv*)env2)

    if (!comps_object_cmp((COMPS_Object*)_env1->properties,
                          (COMPS_Object*)_env2->properties)) {
        //printf("Env properties cmp fail\n");
        return 0;
    }
    if (!comps_object_cmp((COMPS_Object*)_env1->name_by_lang,
                          (COMPS_Object*)_env2->name_by_lang)) {
        //printf("Env name_by_lang cmp fail\n");
        return 0;
    }
    if (!comps_object_cmp((COMPS_Object*)_env1->desc_by_lang,
                          (COMPS_Object*)_env2->desc_by_lang)) {
        //printf("Env desc_by_lang cmp fail\n");
        return 0;
    }
    if (!comps_object_cmp((COMPS_Object*)_env1->group_list,
                          (COMPS_Object*)_env2->group_list)) {
        //printf("Env group_list cmp fail\n");
        return 0;
    }
    if (!comps_object_cmp((COMPS_Object*)_env1->option_list,
                          (COMPS_Object*)_env2->option_list)) {
        //printf("Env option_list cmp fail\n");
        return 0;
    }
    return 1;
    #undef _env1
    #undef _env2
}

char __comps_docenv_idcmp(void *e1, void *e2) {
    COMPS_Object *obj1, *obj2;
    char ret;
    obj1 = comps_objdict_get(((COMPS_DocEnv*)e1)->properties, "id");
    obj2 = comps_objdict_get(((COMPS_DocEnv*)e2)->properties, "id");
    COMPS_OBJECT_DESTROY(obj1);
    COMPS_OBJECT_DESTROY(obj2);
    ret = comps_object_cmp(obj1, obj2);
    return ret;
}

COMPS_DocEnv* comps_docenv_union(COMPS_DocEnv *e1, COMPS_DocEnv *e2) {
    COMPS_DocEnv *res;
    COMPS_ObjListIt *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_HSList *pairs;

    res = (COMPS_DocEnv*)comps_object_create(&COMPS_DocEnv_ObjInfo, NULL);
    COMPS_OBJECT_DESTROY(res->properties);
    res->properties = comps_objdict_clone(e2->properties);
    pairs = comps_objdict_pairs(e1->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        comps_objdict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                         ((COMPS_RTreePair*)hsit->data)->data);
    }
    comps_hslist_destroy(&pairs);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);
    it = e1->group_list?e1->group_list->first:NULL;
    for (; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    it = e2->group_list?e2->group_list->first:NULL;
    for (; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    //res->group_list = (COMPS_ObjList*)comps_object_create(
    //                                              &COMPS_ObjList_ObjInfo, NULL);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_groupid(res,
                             (COMPS_DocGroupId*)comps_object_copy(hsit->data));
    }
    comps_set_destroy(&set);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);
    if  (e1->option_list)
        for (it = e1->option_list->first; it != NULL; it = it->next)
            comps_set_add(set, it->comps_obj);
    if (e2->option_list)
        for (it = e2->option_list->first; it != NULL; it = it->next)
            comps_set_add(set, it->comps_obj);
    //res->group_list = (COMPS_ObjList*)comps_object_create(
    //                                              &COMPS_ObjList_ObjInfo, NULL);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_optionid(res,
                             (COMPS_DocGroupId*)comps_object_copy(hsit->data));
    }
    comps_set_destroy(&set);
    comps_object_destroy((COMPS_Object*)res->name_by_lang);
    comps_object_destroy((COMPS_Object*)res->desc_by_lang);
    res->name_by_lang = comps_objdict_union(e1->name_by_lang, e2->name_by_lang);
    res->desc_by_lang = comps_objdict_union(e1->desc_by_lang, e2->desc_by_lang);
    return res;
}

COMPS_DocEnv* comps_docenv_intersect(COMPS_DocEnv *e1, COMPS_DocEnv *e2) {
    COMPS_DocEnv *res;
    COMPS_ObjListIt *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set, *set2;
    COMPS_HSList *pairs1, *pairs2;

    res = (COMPS_DocEnv*)comps_object_create(&COMPS_DocEnv_ObjInfo, NULL);
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_objrtree_paircmp);

    pairs1 = comps_objdict_pairs(e1->properties);
    for (hsit = pairs1->first; hsit != NULL; hsit = hsit->next) {
        comps_set_add(set, hsit->data);
    }
    pairs2 = comps_objdict_pairs(e2->properties);
    for (hsit = pairs2->first; hsit != NULL; hsit = hsit->next) {
        if (comps_set_in(set, hsit->data)) {
            comps_objdict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                              ((COMPS_RTreePair*)hsit->data)->data);
        }
    }
    comps_hslist_destroy(&pairs1);
    comps_hslist_destroy(&pairs2);
    comps_set_destroy(&set);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);
    set2 = comps_set_create();
    comps_set_init(set2, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);

    for (it = e1->group_list->first; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    for (it = e2->group_list->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj))
            comps_set_add(set2, it->comps_obj);
    }
    //res->group_list = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo,
    //                                                      NULL);
    //comps_list_init(res->group_list);
    for (hsit = set2->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_groupid(res,
                              (COMPS_DocGroupId*)comps_object_copy(hsit->data));
    }
    comps_set_destroy(&set);
    comps_set_destroy(&set2);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);
    set2 = comps_set_create();
    comps_set_init(set2, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);

    for (it = e1->option_list->first; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    for (it = e2->option_list->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj))
            comps_set_add(set2, it->comps_obj);
    }
    //res->option_list = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo,
    //                                                      NULL);
    for (hsit = set2->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_optionid(res,
                              (COMPS_DocGroupId*)comps_object_copy(hsit->data));
    }
    comps_set_destroy(&set);
    comps_set_destroy(&set2);
    return res;
}

signed char comps_docenv_xml(COMPS_DocEnv *env, xmlTextWriterPtr writer,
                        COMPS_Log *log) {
    COMPS_ObjListIt *it;
    COMPS_Object *obj;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;

    static char* props[] = {"id", "name", "name", "desc", "desc",
                            "display_order"};
    static size_t type[] =   {0, 0, offsetof(COMPS_DocEnv, name_by_lang),
                              0, offsetof(COMPS_DocEnv, desc_by_lang), 0};
    static char* aliases[] = {NULL, NULL, NULL, "description", "description",
                              NULL};
    char *str;
    int ret;

    if (env->group_list->len == 0 && env->group_list->len == 0) {
        obj = comps_docenv_get_id(env);
        comps_log_error(log, COMPS_ERR_PKGLIST_EMPTY, 1, obj);
        COMPS_OBJECT_DESTROY(obj);
        return 1;
    }
    ret = xmlTextWriterStartElement(writer, BAD_CAST "environment");
    COMPS_XMLRET_CHECK
    for (int i=0; i<6; i++) {
        if (!type[i]) {
            obj = comps_objdict_get_x(env->properties, props[i]);
            if (obj) {
                str = comps_object_tostr(obj);
                __comps_xml_prop((aliases[i])?aliases[i]:props[i], str, writer);
                free(str);
            } else {
                //printf("missing %s\n", props[i]);
            }
        } else {
            pairlist = comps_objdict_pairs(*(COMPS_ObjDict**)
                                           (((char*)env)+type[i]));
            for (hsit = pairlist->first; hsit != NULL; hsit = hsit->next) {
                ret = xmlTextWriterStartElement(writer,
                                   BAD_CAST((aliases[i])?aliases[i]:props[i]));
                COMPS_XMLRET_CHECK
                ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                        (xmlChar*) ((COMPS_ObjRTreePair*)hsit->data)->key);
                COMPS_XMLRET_CHECK
                str = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
                ret = xmlTextWriterWriteString(writer, (xmlChar*)str);
                COMPS_XMLRET_CHECK
                free(str);
                ret = xmlTextWriterEndElement(writer);
                COMPS_XMLRET_CHECK
            }
            comps_hslist_destroy(&pairlist);
        }
    }
    if (env->group_list->len) {
        ret = xmlTextWriterStartElement(writer, (xmlChar*)"grouplist");
        COMPS_XMLRET_CHECK
        for (it = env->group_list->first; it != NULL; it = it->next) {
            ret = xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
            COMPS_XMLRET_CHECK
            if (((COMPS_DocGroupId*)it->comps_obj)->def) {
                ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                                          BAD_CAST "true");
                COMPS_XMLRET_CHECK
            }
            str = comps_object_tostr((COMPS_Object*)
                                     ((COMPS_DocGroupId*)it->comps_obj)->name);
            ret = xmlTextWriterWriteString(writer, BAD_CAST str);
            COMPS_XMLRET_CHECK
            free(str);
            ret = xmlTextWriterEndElement(writer);
            COMPS_XMLRET_CHECK
        }
        ret = xmlTextWriterEndElement(writer);
        COMPS_XMLRET_CHECK
    }

    if (env->option_list->len) {
        ret = xmlTextWriterStartElement(writer, (xmlChar*)"optionlist");
        COMPS_XMLRET_CHECK
        for (it = env->option_list->first; it != NULL; it = it->next) {
            ret = xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
            COMPS_XMLRET_CHECK
            if (((COMPS_DocGroupId*)it->comps_obj)->def) {
                ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                                          BAD_CAST "true");
                COMPS_XMLRET_CHECK
            }
            str = comps_object_tostr((COMPS_Object*)
                                     ((COMPS_DocGroupId*)it->comps_obj)->name);
            ret = xmlTextWriterWriteString(writer, BAD_CAST str);
            COMPS_XMLRET_CHECK
            free(str);
            ret = xmlTextWriterEndElement(writer);
            COMPS_XMLRET_CHECK
        }
        ret = xmlTextWriterEndElement(writer);
        COMPS_XMLRET_CHECK
    }
    ret = xmlTextWriterEndElement(writer);
    COMPS_XMLRET_CHECK
    return 0;
}

char* comps_docenv_tostr_u(COMPS_Object* env) {
    #define _env_ ((COMPS_DocEnv*)env)
    char *ret, *name_by_lang_str, *desc_by_lang_str, *group_list_str;
    char *option_list_str;
    int total_len = 0;
    const char *head = "<COMPS_Env ";
    COMPS_Object *tmpprop;
    COMPS_Object*(*getters[])(COMPS_DocEnv*)= {comps_docenv_get_id,
                                     comps_docenv_get_name,
                                     comps_docenv_get_desc,
                                     comps_docenv_get_display_order};
    char *props[4];
    for (int i=0; i<4; i++) {
        tmpprop = getters[i](_env_);
        props[i] = comps_object_tostr(tmpprop);
        total_len += strlen(props[i]);
    }
    name_by_lang_str = comps_object_tostr((COMPS_Object*)_env_->name_by_lang);
    total_len += strlen(name_by_lang_str);
    desc_by_lang_str = comps_object_tostr((COMPS_Object*)_env_->desc_by_lang);
    total_len += strlen(desc_by_lang_str);
    group_list_str = comps_object_tostr((COMPS_Object*)_env_->group_list);
    total_len += strlen(group_list_str);
    option_list_str = comps_object_tostr((COMPS_Object*)_env_->option_list);
    total_len += strlen(option_list_str);
    
    ret = malloc(sizeof(char) * (total_len+2+(8*2)+strlen(head)));
    ret[0] = 0;
    strcat(ret, head);
    for (int i=0; i<4; i++) {
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
    strcat(ret, group_list_str);
    free(group_list_str);
    strcat(ret, ", ");
    strcat(ret, option_list_str);
    free(option_list_str);
    strcat(ret, ">");
    return ret;
    #undef _env_
}

COMPS_ObjectInfo COMPS_DocEnv_ObjInfo = {
    .obj_size = sizeof(COMPS_DocEnv),
    .constructor = &comps_docenv_create_u,
    .destructor = &comps_docenv_destroy_u,
    .copy = &comps_docenv_copy_u,
    .obj_cmp = &comps_docenv_cmp_u,
    .to_str = &comps_docenv_tostr_u
};

COMPS_ValRuleGeneric* COMPS_DocEnv_ValidateRules[] = {
    &(COMPS_ValRuleProp){COMPS_VAL_RULE_PROP,
                         .get_f = &comps_docenv_get_id_obj,
                         .check_f = &comps_id_check},
    NULL
};
