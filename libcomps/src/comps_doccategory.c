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

#include "comps_doccategory.h"
#include "comps_set.h"

void comps_doccategory_create(COMPS_DocCategory *category, COMPS_Object **args) {
    (void)args;
    category->properties = (COMPS_ObjDict*)comps_object_create(
                                                  &COMPS_ObjDict_ObjInfo, NULL);
    category->name_by_lang = (COMPS_ObjDict*)comps_object_create(
                                                  &COMPS_ObjDict_ObjInfo, NULL);
    category->desc_by_lang = (COMPS_ObjDict*)comps_object_create(
                                                  &COMPS_ObjDict_ObjInfo, NULL);
    category->group_ids = (COMPS_ObjList*)comps_object_create(
                                                  &COMPS_ObjList_ObjInfo, NULL);
}
COMPS_CREATE_u(doccategory, COMPS_DocCategory)  /*comps_utils.h macro*/

void comps_doccategory_copy(COMPS_DocCategory *category_dst,
                            COMPS_DocCategory *category_src) {
    category_dst->properties = (COMPS_ObjDict*)
                                COMPS_OBJECT_COPY(category_src->properties);
    category_dst->name_by_lang = (COMPS_ObjDict*)
                                COMPS_OBJECT_COPY(category_src->name_by_lang);
    category_dst->desc_by_lang = (COMPS_ObjDict*)
                                COMPS_OBJECT_COPY(category_src->desc_by_lang);
    category_dst->group_ids = (COMPS_ObjList*)
                                COMPS_OBJECT_COPY(category_src->group_ids);
}
COMPS_COPY_u(doccategory, COMPS_DocCategory)    /*comps_utils.h macro*/

static void comps_doccategory_destroy(COMPS_DocCategory *category) {
    COMPS_OBJECT_DESTROY(category->properties);
    COMPS_OBJECT_DESTROY(category->name_by_lang);
    COMPS_OBJECT_DESTROY(category->desc_by_lang);
    COMPS_OBJECT_DESTROY(category->group_ids);
}
COMPS_DESTROY_u(doccategory, COMPS_DocCategory) /*comps_utils.h macro*/

COMPS_STRPROP_SETTER(category, COMPS_DocCategory, id) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(category, COMPS_DocCategory, name) /*comps_utils.h macro*/
COMPS_STRPROP_SETTER(category, COMPS_DocCategory, desc) /*comps_utils.h macro*/
COMPS_NUMPROP_SETTER(category, COMPS_DocCategory, display_order) /*comps_utils.h macro*/

COMPS_PROP_GETTER(category, COMPS_DocCategory, id) /*comps_utils.h macro*/
COMPS_PROP_GETTER_OBJ(category, COMPS_DocCategory, id) /*comps_utils.h macro*/
COMPS_PROP_GETTER(category, COMPS_DocCategory, name) /*comps_utils.h macro*/
COMPS_PROP_GETTER(category, COMPS_DocCategory, desc) /*comps_utils.h macro*/
COMPS_PROP_GETTER(category, COMPS_DocCategory, display_order) /*comps_utils.h macro*/

COMPS_DOCOBJ_GETOBJLIST(doccategory, COMPS_DocCategory, group_ids, group_ids)
COMPS_DOCOBJ_SETOBJLIST(doccategory, COMPS_DocCategory, group_ids, group_ids)

void comps_doccategory_add_groupid(COMPS_DocCategory *category,
                                COMPS_DocGroupId *gid) {

    if (category == NULL || gid == NULL) {
        assert(0);
        return;
    }
    if (category->group_ids == NULL) {
        category->group_ids = (COMPS_ObjList*) comps_object_create(
                                                 &COMPS_ObjList_ObjInfo, NULL);
    }
    comps_objlist_append_x(category->group_ids, (COMPS_Object*)gid);
}

signed char comps_doccategory_cmp_u(COMPS_Object *cat1, COMPS_Object *cat2) {
    #define _cat1 ((COMPS_DocCategory*)cat1)
    #define _cat2 ((COMPS_DocCategory*)cat2)

    if (!comps_object_cmp((COMPS_Object*)_cat1->properties,
                          (COMPS_Object*)_cat2->properties)) {
        //printf("Category properties cmp fail\n");
        return 0;
    }
    if (!comps_object_cmp((COMPS_Object*)_cat1->name_by_lang,
                          (COMPS_Object*)_cat2->name_by_lang)) {
        //printf("Category name_by_lang cmp fail\n");
        return 0;
    }
    if (!comps_object_cmp((COMPS_Object*)_cat1->desc_by_lang,
                          (COMPS_Object*)_cat2->desc_by_lang)) {
        //printf("Category desc_by_lang cmp fail\n");
        return 0;
    }
    if (!comps_object_cmp((COMPS_Object*)_cat1->group_ids,
                          (COMPS_Object*)_cat2->group_ids)) {
        //printf("Category group_ids cmp fail\n");
        return 0;
    }
    //printf("group category pass\n");
    return 1;
    #undef _cat1
    #undef _cat2
}

char __comps_doccategory_idcmp(void *c1, void *c2) {
    COMPS_Object *obj1, *obj2;
    char ret;
    obj1 = comps_objdict_get(((COMPS_DocCategory*)c1)->properties, "id");
    obj2 = comps_objdict_get(((COMPS_DocCategory*)c2)->properties, "id");
    COMPS_OBJECT_DESTROY(obj1);
    COMPS_OBJECT_DESTROY(obj2);
    ret = comps_object_cmp(obj1, obj2);
    return ret;
}

COMPS_DocCategory* comps_doccategory_union(COMPS_DocCategory *c1,
                                           COMPS_DocCategory *c2) {
    COMPS_DocCategory *res;
    COMPS_ObjListIt *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_HSList *pairs;
    res = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo,
                                                  NULL);
    COMPS_OBJECT_DESTROY(res->properties);


    res->properties = comps_objdict_clone(c2->properties);
    pairs = comps_objdict_pairs(c1->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        comps_objdict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                          ((COMPS_RTreePair*)hsit->data)->data);
    }
    comps_hslist_destroy(&pairs);
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);
    it = c1->group_ids?c1->group_ids->first:NULL;
    for (; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    it = c2->group_ids?c2->group_ids->first:NULL;
    for (; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_doccategory_add_groupid(res,
                            (COMPS_DocGroupId*)comps_object_copy(hsit->data));
    }
    comps_set_destroy(&set);
    COMPS_OBJECT_DESTROY(res->name_by_lang);
    COMPS_OBJECT_DESTROY(res->desc_by_lang);
    res->name_by_lang = comps_objdict_union(c1->name_by_lang, c2->name_by_lang);

    res->desc_by_lang = comps_objdict_union(c1->desc_by_lang, c2->desc_by_lang);
    return res;
}

COMPS_DocCategory* comps_doccategory_intersect(COMPS_DocCategory *c1,
                                         COMPS_DocCategory *c2) {
    COMPS_DocCategory *res;
    COMPS_ObjListIt *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;

    COMPS_HSList *pairs1, *pairs2;

    res = (COMPS_DocCategory*)comps_object_create(&COMPS_DocCategory_ObjInfo, NULL);
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_objrtree_paircmp);

    pairs1 = comps_objdict_pairs(c1->properties);
    for (hsit = pairs1->first; hsit != NULL; hsit = hsit->next) {
        comps_set_add(set, hsit->data);
        
    }
    pairs2 = comps_objdict_pairs(c2->properties);
    for (hsit = pairs2->first; hsit != NULL; hsit = hsit->next) {
        if (comps_set_in(set, hsit->data)) {
            comps_objdict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                  ((COMPS_RTreePair*)hsit->data)->data);
        }
    }
    comps_hslist_destroy(&pairs1);
    comps_hslist_destroy(&pairs2);
    comps_set_clear(set);

    //set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroupid_cmp_set);

    //res->group_ids = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo,
    //                                                     NULL);
    //comps_list_init(res->group_ids);

    for (it = c1->group_ids->first; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    for (it = c2->group_ids->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            comps_doccategory_add_groupid(res,
                        (COMPS_DocGroupId*)comps_object_copy(it->comps_obj));
        }
    }
    comps_set_destroy(&set);
    return res;
}

signed char comps_doccategory_xml(COMPS_DocCategory *category, xmlTextWriterPtr writer,
                        COMPS_Log *log) {
    COMPS_ObjListIt *it;
    COMPS_Object *obj;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;

    static char* props[] = {"id", "name", "name", "desc", "desc", "display_order"};
    static size_t type[] =   {0, 0, offsetof(COMPS_DocCategory, name_by_lang),
                           0, offsetof(COMPS_DocCategory, desc_by_lang), 0};
    static char* aliases[] = {NULL, NULL, NULL, "description", "description", NULL};
    char *str;
    int ret;

    if (category->group_ids->len == 0) {
        obj = comps_doccategory_get_id(category);
        comps_log_error(log, COMPS_ERR_PKGLIST_EMPTY, 1, obj);
        COMPS_OBJECT_DESTROY(obj);
        return 1;
    }
    ret = xmlTextWriterStartElement(writer, BAD_CAST "category");
    COMPS_XMLRET_CHECK
    for (int i=0; i<6; i++) {
        if (!type[i]) {
            obj = comps_objdict_get_x(category->properties, props[i]);
            if (obj) {
                str = comps_object_tostr(obj);
                //printf("%s = %s\n", props[i], str);
                __comps_xml_prop((aliases[i])?aliases[i]:props[i], str, writer);
                free(str);
            } else {
                //printf("missing %s\n", props[i]);
            }
        } else {
            pairlist = comps_objdict_pairs(*(COMPS_ObjDict**)
                                           (((char*)category)+type[i]));
            for (hsit = pairlist->first; hsit != NULL; hsit = hsit->next) {
                ret = xmlTextWriterStartElement(writer,
                            (const xmlChar*)(aliases[i]?aliases[i]:props[i]));
                COMPS_XMLRET_CHECK
                ret = xmlTextWriterWriteAttribute(writer, (xmlChar*) "xml:lang",
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
    //if (category->group_ids->len) {
    ret = xmlTextWriterStartElement(writer, (xmlChar*)"grouplist");
    COMPS_XMLRET_CHECK
    for (it = category->group_ids->first; it != NULL; it = it->next) {
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
    ret = xmlTextWriterEndElement(writer);
    COMPS_XMLRET_CHECK
    return 0;
}

char* comps_doccategory_tostr_u(COMPS_Object* cat) {
    #define _cat_ ((COMPS_DocCategory*)cat)
    char *ret, *name_by_lang_str, *desc_by_lang_str, *group_ids_str;
    int total_len = 0;
    const char *head = "<COMPS_Category ";
    COMPS_Object *tmpprop;
    COMPS_Object*(*getters[])(COMPS_DocCategory*)= {comps_doccategory_get_id,
                                     comps_doccategory_get_name,
                                     comps_doccategory_get_desc,
                                     comps_doccategory_get_display_order};
    char *props[4];
    for (int i=0; i<4; i++) {
        tmpprop = getters[i](_cat_);
        props[i] = comps_object_tostr(tmpprop);
        total_len += strlen(props[i]);
    }
    name_by_lang_str = comps_object_tostr((COMPS_Object*)_cat_->name_by_lang);
    total_len += strlen(name_by_lang_str);
    desc_by_lang_str = comps_object_tostr((COMPS_Object*)_cat_->desc_by_lang);
    total_len += strlen(desc_by_lang_str);
    group_ids_str = comps_object_tostr((COMPS_Object*)_cat_->group_ids);
    total_len += strlen(group_ids_str);
    
    ret = malloc(sizeof(char) * (total_len+2+(6*2)+strlen(head)));
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
    strcat(ret, group_ids_str);
    free(group_ids_str);
    strcat(ret, ">");
    return ret;
    #undef _cat_
}


COMPS_ObjectInfo COMPS_DocCategory_ObjInfo = {
    .obj_size = sizeof(COMPS_DocCategory),
    .constructor = &comps_doccategory_create_u,
    .destructor = &comps_doccategory_destroy_u,
    .copy = &comps_doccategory_copy_u,
    .obj_cmp = &comps_doccategory_cmp_u,
    .to_str = &comps_doccategory_tostr_u
};

COMPS_ValRuleGeneric* COMPS_DocCategory_ValidateRules[] = {
    &(COMPS_ValRuleProp){COMPS_VAL_RULE_PROP,
                         .get_f = &comps_doccategory_get_id_obj,
                         .check_f = &comps_id_check},
    NULL
};
