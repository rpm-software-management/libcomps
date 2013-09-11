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

void comps_doccategory_destroy(COMPS_DocCategory *category) {
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
    comps_objlist_append(category->group_ids, (COMPS_Object*)gid);
}

signed char comps_doccategory_cmp_u(COMPS_Object *cat1, COMPS_Object *cat2) {
    #define _cat1 ((COMPS_DocCategory*)cat1)
    #define _cat2 ((COMPS_DocCategory*)cat2)

    if (!comps_object_cmp((COMPS_Object*)_cat1->properties,
                          (COMPS_Object*)_cat2->properties)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_cat1->name_by_lang,
                          (COMPS_Object*)_cat2->name_by_lang)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_cat1->desc_by_lang,
                          (COMPS_Object*)_cat2->desc_by_lang)) return 0;
    if (!comps_object_cmp((COMPS_Object*)_cat1->group_ids,
                          (COMPS_Object*)_cat2->group_ids)) return 0;
    return 1;
    #undef _cat1
    #undef _cat2
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
    comps_objdict_destroy(res->properties);


    res->properties = comps_objdict_clone(c2->properties);
    pairs = comps_objdict_pairs(c1->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        comps_objdict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                   comps_object_copy(((COMPS_RTreePair*)hsit->data)->data));
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
    res->group_ids = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo,
                                                        NULL);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_doccategory_add_groupid(res, hsit->data);
    }
    comps_set_destroy(&set);
    comps_objdict_destroy(res->name_by_lang);
    comps_objdict_destroy(res->desc_by_lang);
    res->name_by_lang = comps_objdict_union(c1->name_by_lang, c2->name_by_lang);
    res->desc_by_lang = comps_objdict_union(c1->desc_by_lang, c2->desc_by_lang);
    return res;
}

void comps_doccategory_xml(COMPS_DocCategory *category, xmlTextWriterPtr writer,
                        COMPS_Logger *log) {
    COMPS_ObjListIt *it;
    COMPS_Object *obj;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;

    static char* props[] = {"id", "name", "name", "desc", "desc", "display_order"};
    static size_t type[] =   {0, 0, offsetof(COMPS_DocCategory, name_by_lang),
                           0, offsetof(COMPS_DocCategory, desc_by_lang), 0};
    static char* aliases[] = {NULL, NULL, NULL, "description", "description", NULL};
    char *str;

    xmlTextWriterStartElement(writer, BAD_CAST "category");
    for (int i=0; i<6; i++) {
        if (!type[i]) {
            obj = comps_objdict_get(category->properties, props[i]);
            if (obj) {
                __comps_xml_prop((aliases[i])?aliases[i]:props[i], obj);
            }
        } else {
            pairlist = comps_objdict_pairs(*(COMPS_ObjDict**)
                                           (((char*)category)+type[i]));
            for (hsit = pairlist->first; hsit != NULL; hsit = hsit->next) {
                xmlTextWriterStartElement(writer,
                            (const xmlChar*)(aliases[i]?aliases[i]:props[i]));
                xmlTextWriterWriteAttribute(writer, (xmlChar*) "xml:lang",
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
    for (it = category->group_ids->first; it != NULL; it = it->next) {
        xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
        if (((COMPS_DocGroupId*)it->comps_obj)->def) {
            xmlTextWriterWriteAttribute(writer, BAD_CAST "default",
                                                BAD_CAST "true");
        }
        str = comps_object_tostr((COMPS_Object*)((COMPS_DocGroupId*)it->comps_obj)->name);
        xmlTextWriterWriteString(writer, BAD_CAST str);
        free(str);
        xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);
}
COMPS_ObjectInfo COMPS_DocCategory_ObjInfo = {
    .obj_size = sizeof(COMPS_DocCategory),
    .constructor = &comps_doccategory_create_u,
    .destructor = &comps_doccategory_destroy_u,
    .copy = &comps_doccategory_copy_u,
    .obj_cmp = &comps_doccategory_cmp_u
};
