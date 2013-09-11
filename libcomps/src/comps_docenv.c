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

void comps_docenv_destroy(COMPS_DocEnv *env) {
    comps_object_destroy((COMPS_Object*)env->properties);
    comps_object_destroy((COMPS_Object*)env->name_by_lang);
    comps_object_destroy((COMPS_Object*)env->desc_by_lang);
    comps_object_destroy((COMPS_Object*)env->group_list);
    comps_object_destroy((COMPS_Object*)env->option_list);
}
COMPS_DESTROY_u(docenv, COMPS_DocEnv) /*comps_utils.h macro*/

COMPS_STRPROP_SETTER(env, COMPS_DocEnv, id) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(env, COMPS_DocEnv, name) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(env, COMPS_DocEnv, desc) /*comps_utils.h macro*/
COMPS_NUMPROP_SETTER(env, COMPS_DocEnv, display_order) /*comps_utils.h macro*/

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
    comps_objlist_append(env->group_list, (COMPS_Object*)gid);
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
    comps_objlist_append(env->option_list, (COMPS_Object*)gid);
}

signed char comps_docenv_cmp_u(COMPS_Object *env1, COMPS_Object *env2) {
    #define _env1 ((COMPS_DocEnv*)env1)
    #define _env2 ((COMPS_DocEnv*)env2)

    if (!comps_object_cmp((COMPS_Object*)_env1->properties,
                          (COMPS_Object*)_env2->properties)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_env1->name_by_lang,
                          (COMPS_Object*)_env2->name_by_lang)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_env1->desc_by_lang,
                          (COMPS_Object*)_env2->desc_by_lang)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_env1->group_list,
                          (COMPS_Object*)_env2->group_list)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_env1->option_list,
                          (COMPS_Object*)_env2->option_list)) return 0;
    return 1;
    #undef _env1
    #undef _env2
}

COMPS_DocEnv* comps_docenv_union(COMPS_DocEnv *e1, COMPS_DocEnv *e2) {
    COMPS_DocEnv *res;
    COMPS_ObjListIt *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_HSList *pairs;

    res = (COMPS_DocEnv*)comps_object_create(&COMPS_DocEnv_ObjInfo, NULL);
    comps_objdict_destroy(res->properties);
    res->properties = comps_objdict_clone(e2->properties);
    pairs = comps_objdict_pairs(e1->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        comps_objdict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                comps_object_copy(((COMPS_RTreePair*)hsit->data)->data));
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
    res->group_list = (COMPS_ObjList*)comps_object_create(
                                                  &COMPS_ObjList_ObjInfo, NULL);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_groupid(res, hsit->data);
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
    res->group_list = (COMPS_ObjList*)comps_object_create(
                                                  &COMPS_ObjList_ObjInfo, NULL);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_optionid(res, hsit->data);
    }
    comps_set_destroy(&set);
    comps_object_destroy((COMPS_Object*)res->name_by_lang);
    comps_object_destroy((COMPS_Object*)res->desc_by_lang);
    res->name_by_lang = comps_objdict_union(e1->name_by_lang, e2->name_by_lang);
    res->desc_by_lang = comps_objdict_union(e1->desc_by_lang, e2->desc_by_lang);
    return res;
}

void comps_docenv_xml(COMPS_DocEnv *env, xmlTextWriterPtr writer,
                        COMPS_Logger *log) {
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

    xmlTextWriterStartElement(writer, BAD_CAST "env");
    for (int i=0; i<6; i++) {
        if (!type[i]) {
            obj = comps_objdict_get(env->properties, props[i]);
            if (obj) {
                __comps_xml_prop((aliases[i])?aliases[i]:props[i], obj);
            }
        } else {
            pairlist = comps_objdict_pairs(*(COMPS_ObjDict**)
                                           (((char*)env)+type[i]));
            for (hsit = pairlist->first; hsit != NULL; hsit = hsit->next) {
                xmlTextWriterStartElement(writer,
                                   BAD_CAST((aliases[i])?aliases[i]:props[i]));
                xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                        (xmlChar*) ((COMPS_ObjRTreePair*)hsit->data)->key);
                str = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
                xmlTextWriterWriteString(writer, (xmlChar*)str);
                free(str);
                xmlTextWriterEndElement(writer);
            }
            comps_hslist_destroy(&pairlist);
        }
    }
    xmlTextWriterStartElement(writer, (xmlChar*)"grouplist");
    for (it = env->group_list->first; it != NULL; it = it->next) {
        xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
        if (((COMPS_DocGroupId*)it->comps_obj)->def) {
            xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                                BAD_CAST "true");
        }
        xmlTextWriterWriteString(writer,
                            BAD_CAST ((COMPS_DocGroupId*)it->comps_obj)->name);
        xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
    xmlTextWriterStartElement(writer, (xmlChar*)"optionlist");
    for (it = env->option_list->first; it != NULL; it = it->next) {
        xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
        if (((COMPS_DocGroupId*)it->comps_obj)->def) {
            xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                                BAD_CAST "true");
        }
        xmlTextWriterWriteString(writer,
                            BAD_CAST ((COMPS_DocGroupId*)it->comps_obj)->name);
        xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);
}

COMPS_ObjectInfo COMPS_DocEnv_ObjInfo = {
    .obj_size = sizeof(COMPS_DocEnv),
    .constructor = &comps_docenv_create_u,
    .destructor = &comps_docenv_destroy_u,
    .copy = &comps_docenv_copy_u,
    .obj_cmp = &comps_docenv_cmp_u
};
