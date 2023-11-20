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

#include "comps_doc.h"
#include "comps_set.h"
//#include "comps_types.h"
#include "comps_utils.h"
#include "comps_default.h"

#include <stdio.h>
#include <assert.h>
#include <libxml/parser.h>
static signed char comps_doc_xml(COMPS_Doc *doc, xmlTextWriterPtr writer,
                                 COMPS_XMLOptions *xml_options,
                                 COMPS_DefaultsOptions *def_options);

void comps_doc_create(COMPS_Doc* doc, COMPS_Object **args) {
    doc->objects = COMPS_OBJECT_CREATE(COMPS_ObjDict, NULL);
    doc->log = COMPS_OBJECT_CREATE(COMPS_Log, NULL);
    //doc->log = comps_log_create(0);
    if (args && args[0]->obj_info == &COMPS_Str_ObjInfo) {
        doc->encoding = (COMPS_Str*) comps_object_incref(args[0]);
    } else {
        doc->encoding = NULL;
    }
    doc->doctype_name = comps_str(comps_default_doctype_name);
    doc->doctype_sysid = comps_str(comps_default_doctype_sysid);
    doc->doctype_pubid = comps_str(comps_default_doctype_pubid);
    doc->lang = NULL;
}
COMPS_CREATE_u(doc, COMPS_Doc)

void comps_doc_copy(COMPS_Doc *doc_dst, COMPS_Doc *doc_src) {
    doc_dst->encoding = (COMPS_Str*) COMPS_OBJECT_COPY(doc_src->encoding);
    doc_dst->doctype_name = (COMPS_Str*) COMPS_OBJECT_COPY(doc_src->doctype_name);
    doc_dst->doctype_sysid = (COMPS_Str*) COMPS_OBJECT_COPY(doc_src->doctype_sysid);
    doc_dst->doctype_pubid = (COMPS_Str*) COMPS_OBJECT_COPY(doc_src->doctype_pubid);
    doc_dst->objects = (COMPS_ObjDict*) COMPS_OBJECT_COPY(doc_src->objects);
}
COMPS_COPY_u(doc, COMPS_Doc)

void comps_doc_destroy(COMPS_Doc *doc) {
    if (doc != NULL) {
        COMPS_OBJECT_DESTROY(doc->log);
        COMPS_OBJECT_DESTROY(doc->objects);
        COMPS_OBJECT_DESTROY(doc->encoding);
        COMPS_OBJECT_DESTROY(doc->doctype_name);
        COMPS_OBJECT_DESTROY(doc->doctype_sysid);
        COMPS_OBJECT_DESTROY(doc->doctype_pubid);
    }
}
COMPS_DESTROY_u(doc, COMPS_Doc)

signed char comps_doc_cmp_u(COMPS_Object *obj1, COMPS_Object *obj2) {
    COMPS_Object *tmp1, *tmp2;
    signed char ret;

    #define _doc1_ ((COMPS_Doc*)obj1)
    #define _doc2_ ((COMPS_Doc*)obj2)
 
    if (!COMPS_OBJECT_CMP(_doc1_->encoding, _doc2_->encoding)) {
        return 0;
    }
    if (!COMPS_OBJECT_CMP(_doc1_->doctype_name, _doc2_->doctype_name)) {
        return 0;
    }
    if (!COMPS_OBJECT_CMP(_doc1_->doctype_sysid, _doc2_->doctype_sysid)) {
        return 0;
    }
    if (!COMPS_OBJECT_CMP(_doc1_->doctype_pubid, _doc2_->doctype_pubid)) {
        return 0;
    }
    tmp1 = (COMPS_Object*) comps_doc_categories(_doc1_);
    tmp2 = (COMPS_Object*) comps_doc_categories(_doc2_);
    ret = comps_object_cmp(tmp1, tmp2);
    COMPS_OBJECT_DESTROY(tmp1);
    COMPS_OBJECT_DESTROY(tmp2);
    if (!ret) {
        //printf("categories cmp fail\n");
        return ret;
    }

    tmp1 = (COMPS_Object*) comps_doc_groups(_doc1_);
    tmp2 = (COMPS_Object*) comps_doc_groups(_doc2_);
    ret = comps_object_cmp(tmp1, tmp2);
    COMPS_OBJECT_DESTROY(tmp1);
    COMPS_OBJECT_DESTROY(tmp2);
    if (!ret) {
        //printf("groups cmp fail\n");
        return ret;
    }

    tmp1 = (COMPS_Object*) comps_doc_environments(_doc1_);
    tmp2 = (COMPS_Object*) comps_doc_environments(_doc2_);
    ret = comps_object_cmp(tmp1, tmp2);
    COMPS_OBJECT_DESTROY(tmp1);
    COMPS_OBJECT_DESTROY(tmp2);
    if (!ret) {
        return ret;
    }

    tmp1 = (COMPS_Object*) comps_doc_langpacks(_doc1_);
    tmp2 = (COMPS_Object*) comps_doc_langpacks(_doc2_);
    ret = comps_object_cmp(tmp1, tmp2);
    COMPS_OBJECT_DESTROY(tmp1);
    COMPS_OBJECT_DESTROY(tmp2);
    if (!ret) {
        return ret;
    }

    tmp1 = (COMPS_Object*) comps_doc_blacklist(_doc1_);
    tmp2 = (COMPS_Object*) comps_doc_blacklist(_doc2_);
    ret = comps_object_cmp(tmp1, tmp2);
    COMPS_OBJECT_DESTROY(tmp1);
    COMPS_OBJECT_DESTROY(tmp2);
    if (!ret) {
        //printf("blacklist cmp fail\n");
        return ret;
    }

    tmp1 = (COMPS_Object*) comps_doc_whiteout(_doc1_);
    tmp2 = (COMPS_Object*) comps_doc_whiteout(_doc2_);
    ret = comps_object_cmp(tmp1, tmp2);
    COMPS_OBJECT_DESTROY(tmp1);
    COMPS_OBJECT_DESTROY(tmp2);
    if (!ret) {
        //printf("whiteout cmp fail\n");
        return ret;
    }

    return ret;
}

inline void comps_doc_clear(COMPS_Doc *doc) {
    if (doc == NULL) return;
    //comps_objdict_clear(doc->objects);
}

COMPS_DOC_GETOBJLIST(groups)        /*comps_doc.h macro*/
COMPS_DOC_SETOBJLIST(groups) /*comps_doc.h macro*/
COMPS_DOC_ADDOBJLIST(groups, group, COMPS_DocGroup) /*comps_doc.h macro*/
COMPS_DOC_GETOBJLIST(categories)    /*comps_doc.h macro*/
COMPS_DOC_SETOBJLIST(categories) /*comps_doc.h macro*/
COMPS_DOC_ADDOBJLIST(categories, category, COMPS_DocCategory) /*comps_doc.h macro*/
COMPS_DOC_GETOBJLIST(environments)  /*comps_doc.h macro*/
COMPS_DOC_SETOBJLIST(environments) /*comps_doc.h macro*/
COMPS_DOC_ADDOBJLIST(environments, environment, COMPS_DocEnv) /*comps_doc.h macro*/
COMPS_DOC_GETOBJDICT(langpacks)     /*comps_doc.h macro*/
COMPS_DOC_SETOBJDICT(langpacks) /*comps_doc.h macro*/
COMPS_DOC_ADDOBJDICT(langpacks, langpack) /*comps_doc.h macro*/
COMPS_DOC_GETOBJMDICT(blacklist)     /*comps_doc.h macro*/
COMPS_DOC_SETOBJMDICT(blacklist) /*comps_doc.h macro*/
COMPS_DOC_ADDOBJMDICT(blacklist,  blacklist) /*comps_doc.h macro*/
COMPS_DOC_GETOBJMDICT(whiteout)      /*comps_doc.h macro*/
COMPS_DOC_SETOBJMDICT(whiteout) /*comps_doc.h macro*/
COMPS_DOC_ADDOBJMDICT(whiteout,  whiteout) /*comps_doc.h macro*/

COMPS_DOC_SETPROP(lang,  COMPS_Str) /*comps_doc.h macro*/


signed char comps2xml_f(COMPS_Doc * doc, char *filename, char stdoutredirect,
                        COMPS_XMLOptions *xml_options,
                        COMPS_DefaultsOptions *def_options) {
    xmlDocPtr xmldoc;
    int retc;
    char *str;
    signed char genret;

    doc->log->std_out = stdoutredirect;
    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&xmldoc, 0);

    if ((COMPS_Object*)doc->encoding) {
        str = comps_object_tostr((COMPS_Object*)doc->encoding);
        retc = xmlTextWriterStartDocument(writer, NULL, str, NULL);
        free(str);
    } else {
        retc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    }
    if (retc<0)
        comps_log_error(doc->log, COMPS_ERR_XMLGEN, 0);

    if (!xml_options)
        xml_options = &COMPS_XMLDefaultOptions;
    if (!def_options)
        def_options = &COMPS_DDefaultsOptions;
    genret = comps_doc_xml(doc, writer, xml_options, def_options);
    retc = xmlTextWriterEndDocument(writer);
    if (retc<0)
        comps_log_error(doc->log, COMPS_ERR_XMLGEN, 0);

    retc = xmlSaveFormatFileEnc(filename, xmldoc, NULL, 1);
    if (retc<0)
        comps_log_error_x(doc->log, COMPS_ERR_WRITEF,
                          1, comps_str(filename));

    xmlFreeTextWriter(writer);
    xmlFreeDoc(xmldoc);
    xmlCleanupParser();
    xmlMemoryDump();
    return genret;
}

char* comps2xml_str(COMPS_Doc *doc, COMPS_XMLOptions *xml_options,
                    COMPS_DefaultsOptions *def_options) {
    xmlDocPtr xmldoc;
    const char *xmlstr;
    signed char genret;
    char *str, *ret;
    int retc;

    xmlBuffer *xmlbuff = xmlBufferCreate();
    xmlOutputBuffer *xmlobuff = xmlOutputBufferCreateBuffer(xmlbuff, 0);
    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&xmldoc, 0);

    if ((COMPS_Object*)doc->encoding) {
        str = comps_object_tostr((COMPS_Object*)doc->encoding);
        retc = xmlTextWriterStartDocument(writer, NULL, str, NULL);
        free(str);
    } else {
        retc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    }
    if (retc<0) comps_log_error(doc->log, COMPS_ERR_XMLGEN, 0);

    if (!xml_options)
        xml_options = &COMPS_XMLDefaultOptions;
    if (!def_options)
        def_options = &COMPS_DDefaultsOptions;
    genret = comps_doc_xml(doc, writer, xml_options, def_options);
    if (genret)
        comps_log_error(doc->log, COMPS_ERR_XMLGEN, 0);
    if (retc<0)
        comps_log_error(doc->log, COMPS_ERR_XMLGEN, 0);
    retc = xmlTextWriterEndDocument(writer);
    if (retc<0) comps_log_error(doc->log, COMPS_ERR_XMLGEN, 0);
    xmlSaveFormatFileTo(xmlobuff, xmldoc, NULL, 1);

    xmlFreeTextWriter(writer);
    xmlFreeDoc(xmldoc);

    xmlstr =(char*) xmlBufferContent(xmlbuff);
    ret =  malloc(sizeof(char) * (strlen(xmlstr)+1));
    if (ret == NULL) {
        xmlBufferFree(xmlbuff);
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    } else {
        memcpy(ret, xmlstr, sizeof(char)*(strlen(xmlstr)+1));
    }
    xmlBufferFree(xmlbuff);

    xmlCleanupParser();
    xmlMemoryDump();
    return ret;
}

COMPS_Doc* comps_doc_union(COMPS_Doc *c1, COMPS_Doc *c2) {
    COMPS_ObjListIt *it;
    COMPS_Set *set;
    COMPS_Doc *res;
    COMPS_DocGroup *tmpgroup;
    COMPS_DocCategory *tmpcat;
    COMPS_DocEnv *tmpenv;
    COMPS_HSListItem *hsit;
    COMPS_ObjList *groups = comps_doc_groups(c1);
    COMPS_ObjList *categories = comps_doc_categories(c1);
    COMPS_ObjList *envs = comps_doc_environments(c1);
    COMPS_ObjDict *langpacks, *d1, *d2;

    void *tmpdata;
    res = COMPS_OBJECT_CREATE(COMPS_Doc, (COMPS_Object*[]){(COMPS_Object*)
                                                           c1->encoding});

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroup_idcmp);

    for (it = groups ? groups->first : NULL; it != NULL; it = it->next) {
        comps_set_add(set, comps_object_copy(it->comps_obj));
    }
    COMPS_OBJECT_DESTROY(groups);
    groups = comps_doc_groups(c2);
    for (it = groups ? groups->first : NULL; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            tmpgroup = comps_docgroup_union(
                                (COMPS_DocGroup*)comps_set_data_at(set,
                                                               it->comps_obj),
                                (COMPS_DocGroup*)it->comps_obj);
            tmpdata = comps_set_data_at(set, it->comps_obj);
            comps_set_remove(set, it->comps_obj);
            COMPS_OBJECT_DESTROY(tmpdata);
            comps_set_add(set, tmpgroup);
        } else {
            comps_set_add(set, comps_object_copy(it->comps_obj));
        }
    }
    COMPS_OBJECT_DESTROY(groups);
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_doc_add_group(res, hsit->data);
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_doccategory_idcmp);
    for (it = categories ? categories->first : NULL; it != NULL; it = it->next) {
        comps_set_add(set, comps_object_copy(it->comps_obj));
    }
    COMPS_OBJECT_DESTROY(categories);
    categories = comps_doc_categories(c2);
    for (it = categories ? categories->first : NULL; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            tmpcat = comps_doccategory_union(
                                (COMPS_DocCategory*)comps_set_data_at(set,
                                                                it->comps_obj),
                                (COMPS_DocCategory*)it->comps_obj);
            tmpdata = comps_set_data_at(set, it->comps_obj);
            comps_set_remove(set, it->comps_obj);
            COMPS_OBJECT_DESTROY(tmpdata);
            comps_set_add(set, tmpcat);
        } else {
            comps_set_add(set, comps_object_copy(it->comps_obj));
        }
    }
    COMPS_OBJECT_DESTROY(categories);
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_doc_add_category(res, hsit->data);
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_docenv_idcmp);
    if (envs) {
        for (it = envs->first; it != NULL; it = it->next) {
            comps_set_add(set, comps_object_copy(it->comps_obj));
        }
    }
    COMPS_OBJECT_DESTROY(envs);
    envs = comps_doc_environments(c2);
    if (envs) {
        for (it = envs->first; it != NULL; it = it->next) {
            if (comps_set_in(set, it->comps_obj)) {
                tmpenv = comps_docenv_union(
                                    (COMPS_DocEnv*)comps_set_data_at(set,
                                                              it->comps_obj),
                                    (COMPS_DocEnv*)it->comps_obj);
                tmpdata = comps_set_data_at(set, it->comps_obj);
                comps_set_remove(set, it->comps_obj);
                comps_object_destroy((COMPS_Object*)tmpdata);
                comps_set_add(set, tmpenv);
            } else {
                comps_set_add(set, comps_object_copy(it->comps_obj));
            }
        }
    }
    COMPS_OBJECT_DESTROY(envs);
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_doc_add_environment(res, hsit->data);
    }
    comps_set_destroy(&set);

    d1 = comps_doc_langpacks(c1);
    d2 = comps_doc_langpacks(c2);
    langpacks = comps_objdict_union(d1,d2);
    COMPS_OBJECT_DESTROY(d1);
    COMPS_OBJECT_DESTROY(d2);
    comps_doc_set_langpacks(res, langpacks);
    COMPS_OBJECT_DESTROY(langpacks);
    return res;
}

/**
 * Make intersection of two existing COMPS_Doc objects. Result intersection is
 * completly new COMPS_Doc object (deep copy of those two).
 * Intersection is made by objects ids. Objects of intersection is created
 * by intersecting specific objects.
 * @see comps_docgroup_intersect
 * @see comps_doccategory_intersect
 * @see comps_docenv_intersect
 * @param c1 COMPS_Doc object
 * @param c2 COMPS_Doc object
 * @return new COMPS_Doc object
 */
COMPS_Doc* comps_doc_intersect(COMPS_Doc *c1, COMPS_Doc *c2) {
    COMPS_ObjListIt *it;
    COMPS_Set *set;
    COMPS_Doc *res;
    COMPS_DocGroup *tmpgroup;
    COMPS_DocCategory *tmpcat;
    COMPS_DocEnv *tmpenv;
    COMPS_ObjList *groups = comps_doc_groups(c1);
    COMPS_ObjList *categories = comps_doc_categories(c1);
    COMPS_ObjList *envs = comps_doc_environments(c1);

    res = COMPS_OBJECT_CREATE(COMPS_Doc, (COMPS_Object*[])
                                         {(COMPS_Object*)c1->encoding});

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroup_idcmp);

    for (it = groups->first; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    COMPS_OBJECT_DESTROY(groups);
    groups = comps_doc_groups(c2);
    for (it = groups->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            tmpgroup = comps_docgroup_intersect(
                                (COMPS_DocGroup*)it->comps_obj,
                                (COMPS_DocGroup*)comps_set_data_at(set,
                                                                   it->comps_obj));
            comps_doc_add_group(res, tmpgroup);
        }
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_doccategory_idcmp);
    for (it = categories->first; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    COMPS_OBJECT_DESTROY(categories);
    categories = comps_doc_categories(c2);
    for (it = categories->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            tmpcat = comps_doccategory_intersect(
                                (COMPS_DocCategory*)it->comps_obj,
                                (COMPS_DocCategory*)comps_set_data_at(set,
                                                                    it->comps_obj));
            comps_doc_add_category(res, tmpcat);
        }
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_docenv_idcmp);
    for (it = envs->first; it != NULL; it = it->next) {
        comps_set_add(set, it->comps_obj);
    }
    COMPS_OBJECT_DESTROY(envs);
    envs = comps_doc_environments(c2);
    for (it = envs->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            tmpenv = comps_docenv_intersect(
                                (COMPS_DocEnv*)it->comps_obj,
                                (COMPS_DocEnv*)comps_set_data_at(set,it->comps_obj));
            comps_doc_add_environment(res, tmpenv);
        }
    }
    comps_set_destroy(&set);
    COMPS_OBJECT_DESTROY(groups);
    COMPS_OBJECT_DESTROY(categories);
    COMPS_OBJECT_DESTROY(envs);

    return res;
}

COMPS_ObjList* comps_doc_get_groups(COMPS_Doc *doc, char *id, char *name,
                                    char *desc, char *lang, int flags) {
    COMPS_ObjListIt *it;
    COMPS_ObjList *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    COMPS_ObjList *groups = comps_doc_groups(doc);
    COMPS_Str *objid, *objname, *objdesc, *tmp_prop;
    objid = comps_str(id);
    objname = comps_str(name);
    objdesc = comps_str(desc);

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    #define group ((COMPS_DocGroup*)it->comps_obj)

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = (groups)?groups->first:NULL; it != NULL; it = it->next) {
        matched = 0;
        tmp_prop = (COMPS_Str*)comps_docgroup_get_id(group);
        if (id && tmp_prop && comps_str_fnmatch(tmp_prop, id, flags)) {
            matched++;
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
        tmp_prop = (COMPS_Str*)comps_docgroup_get_name(group);
        if (name && !lang && tmp_prop && comps_str_fnmatch(tmp_prop, name, flags))
            matched++;
        else if (name && lang != NULL) {
            COMPS_OBJECT_DESTROY(tmp_prop);
            tmp_prop = (COMPS_Str*)comps_objdict_get(group->name_by_lang, lang);
            //printf("tmp_prop %s\n", tmp_prop->val);
            if (tmp_prop && comps_str_fnmatch(tmp_prop, name, flags)) matched++;
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
        tmp_prop = (COMPS_Str*)comps_docgroup_get_desc(group);
        if (desc && tmp_prop && comps_str_fnmatch(tmp_prop, desc, flags) == 1)
            matched++;
        else if (desc && lang != NULL) {
            COMPS_OBJECT_DESTROY(tmp_prop);
            tmp_prop = (COMPS_Str*)comps_objdict_get(group->desc_by_lang, lang);
            if (tmp_prop && comps_str_fnmatch(tmp_prop, desc, flags)) matched++;
        }
        if (matched == matched_max) {
            comps_objlist_append(ret, (COMPS_Object*)group);
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
    }
    COMPS_OBJECT_DESTROY(objid);
    COMPS_OBJECT_DESTROY(objname);
    COMPS_OBJECT_DESTROY(objdesc);
    COMPS_OBJECT_DESTROY(groups);
    return ret;
    #undef group
}

COMPS_ObjList* comps_doc_get_categories(COMPS_Doc *doc, char *id, char *name,
                                        char *desc, char *lang, int flags) {
    COMPS_ObjListIt *it;
    COMPS_ObjList *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    COMPS_ObjList *categories = comps_doc_categories(doc);
    //COMPS_Object *tmp_prop;
    COMPS_Str *objid, *objname, *objdesc, *tmp_prop;
    objid = comps_str(id);
    objname = comps_str(name);
    objdesc = comps_str(desc);

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    #define category ((COMPS_DocCategory*)it->comps_obj)

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = (categories)?categories->first:NULL; it != NULL; it = it->next) {
        matched = 0;
        tmp_prop = (COMPS_Str*)comps_doccategory_get_id(category);
        if (id && tmp_prop && comps_str_fnmatch(tmp_prop, id, flags)) {
            matched++;
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
        tmp_prop = (COMPS_Str*)comps_doccategory_get_name(category);
        if (name && !lang && tmp_prop && comps_str_fnmatch(tmp_prop, name, flags))
            matched++;
        else if (name && lang != NULL) {
            COMPS_OBJECT_DESTROY(tmp_prop);
            tmp_prop = (COMPS_Str*)comps_objdict_get(category->name_by_lang, lang);
            if (tmp_prop && comps_str_fnmatch(tmp_prop, name, flags)) matched++;
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
        tmp_prop = (COMPS_Str*)comps_doccategory_get_desc(category);
        if (desc && !lang && tmp_prop && comps_str_fnmatch(tmp_prop, desc, flags))
            matched++;
        else if (desc && lang != NULL) {
            COMPS_OBJECT_DESTROY(tmp_prop);
            tmp_prop = (COMPS_Str*)comps_objdict_get(category->desc_by_lang, lang);
            if (tmp_prop && comps_str_fnmatch(tmp_prop, desc, flags)) matched++;
        }
        if (matched == matched_max) {
            comps_objlist_append(ret, (COMPS_Object*)category);
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
    }
    COMPS_OBJECT_DESTROY(objid);
    COMPS_OBJECT_DESTROY(objname);
    COMPS_OBJECT_DESTROY(objdesc);
    COMPS_OBJECT_DESTROY(categories);
    return ret;
    #undef category
}

COMPS_ObjList* comps_doc_get_envs(COMPS_Doc *doc, char *id, char *name,
                                  char *desc, char *lang, int flags) {
    COMPS_ObjListIt *it;
    COMPS_ObjList *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    COMPS_ObjList *envs = comps_doc_environments(doc);
    //COMPS_Object *tmp_prop;
    COMPS_Str *objid, *objname, *objdesc, *tmp_prop;
    objid = comps_str(id);
    objname = comps_str(name);
    objdesc = comps_str(desc);

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    #define env ((COMPS_DocEnv*)it->comps_obj)

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = (envs)?envs->first:NULL; it != NULL; it = it->next) {
        matched = 0;
        tmp_prop = (COMPS_Str*)comps_docenv_get_id(env);
        if (id && tmp_prop && comps_str_fnmatch(tmp_prop, id, flags)) {
            matched++;
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
        tmp_prop = (COMPS_Str*)comps_docenv_get_name(env);
        if (name && !lang && tmp_prop && comps_str_fnmatch(tmp_prop, name, flags))
            matched++;
        else if (name && lang != NULL) {
            COMPS_OBJECT_DESTROY(tmp_prop);
            tmp_prop = (COMPS_Str*)comps_objdict_get(env->name_by_lang, lang);
            if (tmp_prop && comps_str_fnmatch(tmp_prop, name, flags)) matched++;
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
        tmp_prop = (COMPS_Str*)comps_docenv_get_desc(env);
        if (desc && !lang && tmp_prop && comps_str_fnmatch(tmp_prop, desc, flags))
            matched++;
        else if (desc && lang != NULL) {
            COMPS_OBJECT_DESTROY(tmp_prop);
            tmp_prop = (COMPS_Str*)comps_objdict_get(env->desc_by_lang, lang);
            if (tmp_prop && comps_str_fnmatch(tmp_prop, desc, flags)) matched++;
        }
        if (matched == matched_max) {
            comps_objlist_append(ret, (COMPS_Object*)env);
        }
        COMPS_OBJECT_DESTROY(tmp_prop);
    }
    COMPS_OBJECT_DESTROY(objid);
    COMPS_OBJECT_DESTROY(objname);
    COMPS_OBJECT_DESTROY(objdesc);
    COMPS_OBJECT_DESTROY(envs);
    return ret;
    #undef env
}

static signed char comps_doc_xml(COMPS_Doc *doc, xmlTextWriterPtr writer,
                                 COMPS_XMLOptions *xml_options,
                                 COMPS_DefaultsOptions *def_options) {
    COMPS_ObjListIt *it;
    COMPS_ObjList *list;
    COMPS_ObjDict *dict;
    COMPS_ObjMDict *mdict;
    COMPS_HSList *hslist;
    COMPS_HSListItem *hsit;
    int retc;
    signed char ret = 0, tmpret;

    retc = xmlTextWriterStartDTD(writer, (const xmlChar*)doc->doctype_name->val,
                                  (const xmlChar*)doc->doctype_pubid->val,
                                  (const xmlChar*)doc->doctype_sysid->val);
    xmlTextWriterEndDTD(writer);
    if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) return -1;

    retc = xmlTextWriterStartElement(writer, BAD_CAST "comps");
    if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) return -1;
    list = comps_doc_groups(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            tmpret = comps_docgroup_xml((COMPS_DocGroup*)it->comps_obj,
                                        writer, doc->log, xml_options,
                                        def_options);
            if (tmpret == -1) return -1;
            else ret += tmpret;
        }
    }
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_categories(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            tmpret = comps_doccategory_xml((COMPS_DocCategory*)it->comps_obj,
                                           writer, doc->log, xml_options,
                                           def_options);
            if (tmpret == -1) return -1;
            else ret += tmpret;
        }
    }
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_environments(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            tmpret = comps_docenv_xml((COMPS_DocEnv*)it->comps_obj,
                                      writer, doc->log, xml_options,
                                      def_options);
            if (tmpret == -1) return -1;
            else ret += tmpret;
        }
    }
    COMPS_OBJECT_DESTROY(list);
    dict = comps_doc_langpacks(doc);
    if (dict && dict->len) {
        retc = xmlTextWriterStartElement(writer, BAD_CAST "langpacks");
        if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
            COMPS_OBJECT_DESTROY(dict);
            return -1;
        }
        hslist = comps_objrtree_pairs(dict);

        for (hsit = hslist->first; hsit != NULL; hsit = hsit->next) {
            retc = xmlTextWriterStartElement(writer, BAD_CAST "match");
            if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) { 
                COMPS_OBJECT_DESTROY(dict);
                comps_hslist_destroy(&hslist);
                return -1;
            }

            xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                    (xmlChar*) ((COMPS_ObjRTreePair*)hsit->data)->key);

            char *tmp = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
            xmlTextWriterWriteAttribute(writer, BAD_CAST "install", BAD_CAST tmp);
            free(tmp);

            retc = xmlTextWriterEndElement(writer);
            if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
                COMPS_OBJECT_DESTROY(dict);
                comps_hslist_destroy(&hslist);
                return -1;
            }
        }
        comps_hslist_destroy(&hslist);

        retc = xmlTextWriterEndElement(writer);
        if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
            COMPS_OBJECT_DESTROY(dict);
            return -1;
        }
    }
    COMPS_OBJECT_DESTROY(dict);

    mdict = comps_doc_blacklist(doc);
    if (mdict && mdict->len) {
        retc = xmlTextWriterStartElement(writer, BAD_CAST "blacklist");
        if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
            return -1;
        }
        hslist = comps_objmrtree_pairs(mdict);

        for (hsit = hslist->first; hsit != NULL; hsit = hsit->next) {
            for (it = ((COMPS_ObjMRTreePair*)hsit->data)->data->first;
                 it != NULL; it = it->next) {
                retc = xmlTextWriterStartElement(writer, BAD_CAST "package");
                if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
                    comps_hslist_destroy(&hslist);
                    return -1;
                }
                xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                        (xmlChar*) ((COMPS_ObjRTreePair*)hsit->data)->key);

                char *tmp = comps_object_tostr(it->comps_obj);
                xmlTextWriterWriteAttribute(writer, BAD_CAST "arch", BAD_CAST tmp);
                free(tmp);

                retc = xmlTextWriterEndElement(writer);
                if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
                    comps_hslist_destroy(&hslist);
                    return -1;
                }
            }
        }
        comps_hslist_destroy(&hslist);

        retc = xmlTextWriterEndElement(writer);
        if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
            return -1;
        }
    }
    COMPS_OBJECT_DESTROY(mdict);
    mdict = comps_doc_whiteout(doc);
    if (mdict && mdict->len) {
        retc = xmlTextWriterStartElement(writer, BAD_CAST "whiteout");
        if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
            COMPS_OBJECT_DESTROY(mdict);
            return -1;
        }
        hslist = comps_objmrtree_pairs(mdict);

        for (hsit = hslist->first; hsit != NULL; hsit = hsit->next) {
            for (it = ((COMPS_ObjMRTreePair*)hsit->data)->data->first;
                 it != NULL; it = it->next) {
                retc = xmlTextWriterStartElement(writer, BAD_CAST "ignoredep");
                if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
                    comps_hslist_destroy(&hslist);
                    return -1;
                }

                xmlTextWriterWriteAttribute(writer, BAD_CAST "requires",
                        (xmlChar*) ((COMPS_ObjRTreePair*)hsit->data)->key);

                char *tmp = comps_object_tostr(it->comps_obj);
                xmlTextWriterWriteAttribute(writer, BAD_CAST "package", BAD_CAST tmp);
                free(tmp);

                retc = xmlTextWriterEndElement(writer);
                if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
                    comps_hslist_destroy(&hslist);
                    return -1;
                }
            }
        }
        comps_hslist_destroy(&hslist);

        retc = xmlTextWriterEndElement(writer);
        if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) {
            return -1;
        }
    }
    COMPS_OBJECT_DESTROY(mdict);
    retc = xmlTextWriterEndElement(writer);
    if (__comps_check_xml_get(retc, (COMPS_Object*)doc->log) < 0) return -1;
    return ret;
}

COMPS_Str* comps_doc_doctype_name_get(COMPS_Doc* doc) {
    return (COMPS_Str*)COMPS_OBJECT_INCREF(doc->doctype_name);
}
COMPS_Str* comps_doc_doctype_pubid_get(COMPS_Doc* doc) {
    return (COMPS_Str*)COMPS_OBJECT_INCREF(doc->doctype_pubid);
}
COMPS_Str* comps_doc_doctype_sysid_get(COMPS_Doc* doc) {
    return (COMPS_Str*)COMPS_OBJECT_INCREF(doc->doctype_sysid);
}
void comps_doc_doctype_name_set(COMPS_Doc* doc, COMPS_Str *val) {
    COMPS_OBJECT_REPLACE(doc->doctype_name, COMPS_Str, val);
}
void comps_doc_doctype_sysid_set(COMPS_Doc* doc, COMPS_Str *val) {
    COMPS_OBJECT_REPLACE(doc->doctype_name, COMPS_Str, val);
}
void comps_doc_doctype_pubid_set(COMPS_Doc* doc, COMPS_Str *val) {
    COMPS_OBJECT_REPLACE(doc->doctype_name, COMPS_Str, val);
}


COMPS_Doc* comps_doc_arch_filter(COMPS_Doc *source, COMPS_ObjList *arches) {
    COMPS_ObjList *list, *arches2;
    COMPS_Doc *ret;
    COMPS_DocCategory *cat;
    COMPS_DocGroup *group;
    COMPS_DocEnv *env;

    ret = COMPS_OBJECT_CREATE(COMPS_Doc, (COMPS_Object*[])
                                         {(COMPS_Object*)source->encoding});
    list = comps_doc_categories(source);
    for (COMPS_ObjListIt *it = list->first; it != NULL; it = it->next) {
        arches2 = comps_doccategory_arches((COMPS_DocCategory*)it->comps_obj);
        if ((!arches2) || (__comps_objlist_intersected(arches, arches2))) {
            cat = comps_doccategory_arch_filter((COMPS_DocCategory*)it->comps_obj,
                                                arches);
            comps_doc_add_category(ret, cat);
        }
        COMPS_OBJECT_DESTROY(arches2);
    }
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_groups(source);
    for (COMPS_ObjListIt *it = list->first; it != NULL; it = it->next) {
        arches2 = comps_docgroup_arches((COMPS_DocGroup*)it->comps_obj);
        if ((!arches2) || (__comps_objlist_intersected(arches, arches2))) {
            group = comps_docgroup_arch_filter((COMPS_DocGroup*)it->comps_obj,
                                                arches);
            comps_doc_add_group(ret, group);
        }
        COMPS_OBJECT_DESTROY(arches2);
    }
    COMPS_OBJECT_DESTROY(list);
    list = comps_doc_environments(source);
    for (COMPS_ObjListIt *it = list->first; it != NULL; it = it->next) {
        arches2 = comps_docenv_arches((COMPS_DocEnv*)it->comps_obj);
        if ((!arches2) || (__comps_objlist_intersected(arches, arches2))) {
            env = comps_docenv_arch_filter((COMPS_DocEnv*)it->comps_obj,
                                           arches);
            comps_doc_add_environment(ret, env);
        }
        COMPS_OBJECT_DESTROY(arches2);
    }
    COMPS_OBJECT_DESTROY(list);
    return ret;
}

/*COMPS_Doc* comps_doc_set_lang(COMPS_Doc *comps, const char * lang) {
    COMPS_OBJECT_DESTROY(comps->lang);
    comps->lang = comps_str(lang);
}*/

COMPS_ValGenResult* comps_doc_listobjs_validate(COMPS_Object *object,
                                                COMPS_Object *objlist,
                                                COMPS_ValRuleGeneric** rules) {
    (void)object;
    COMPS_ValGenResult *valres = NULL, *tmpres;
    size_t x = 0;
    char *str;
    for (COMPS_ObjListIt *it = ((COMPS_ObjList*)objlist)->first;
         it != NULL;
         it = it->next, x++) {
        tmpres = comps_validate_execute(it->comps_obj, rules);
        if (tmpres->obj_info == &COMPS_ValErrResult_ObjInfo) {
            if (!valres) {
                valres = (COMPS_ValGenResult*)
                         COMPS_OBJECT_CREATE(COMPS_ValErrResult, NULL);
            }
            str = malloc(digits_count(x)*(sizeof(char)+2));
            sprintf(str, "%zu.", x);
            comps_valgenres_prefix(tmpres, str);
            free(str);
            comps_valgenres_concat(&valres, tmpres);
        }
        COMPS_OBJECT_DESTROY(tmpres);
    }
    if (!valres) {
        valres = (COMPS_ValGenResult*)COMPS_OBJECT_CREATE(COMPS_ValOkResult,
                                                          NULL);
    }
    return valres;
}

COMPS_ValGenResult* comps_docgroups_validate(COMPS_Object *object,
                                             COMPS_Object *objlist) {
    return comps_doc_listobjs_validate(object, objlist,
                                       COMPS_DocGroup_ValidateRules);
}
COMPS_ValGenResult* comps_doccategories_validate(COMPS_Object *object,
                                             COMPS_Object *objlist) {
    return comps_doc_listobjs_validate(object, objlist,
                                       COMPS_DocCategory_ValidateRules);
}
COMPS_ValGenResult* comps_docenvs_validate(COMPS_Object *object,
                                             COMPS_Object *objlist) {
    return comps_doc_listobjs_validate(object, objlist,
                                       COMPS_DocEnv_ValidateRules);
}

COMPS_ValRuleGeneric* COMPS_Doc_ValidateRules[] = {
    (COMPS_ValRuleGeneric*)&(COMPS_ValRuleList2){COMPS_VAL_RULE_LIST2,
                         .verbose_msg = "Groups check:",
                         .get_f = (COMPS_Object*(*)(COMPS_Object*))comps_doc_groups,
                         .check_f = &comps_docgroups_validate},
    (COMPS_ValRuleGeneric*)&(COMPS_ValRuleList2){COMPS_VAL_RULE_LIST2,
                         .verbose_msg = "Categories check:",
                         .get_f = (COMPS_Object*(*)(COMPS_Object*))comps_doc_categories,
                         .check_f = &comps_doccategories_validate},
    (COMPS_ValRuleGeneric*)&(COMPS_ValRuleList2){COMPS_VAL_RULE_LIST2,
                         .verbose_msg = "Environments check:",
                         .get_f = (COMPS_Object*(*)(COMPS_Object*))
                                                   comps_doc_environments,
                         .check_f = &comps_docenvs_validate},
    NULL
};

COMPS_ObjectInfo COMPS_Doc_ObjInfo = {
    .obj_size = sizeof(COMPS_Doc),
    .constructor = &comps_doc_create_u,
    .destructor = &comps_doc_destroy_u,
    .copy = &comps_doc_copy_u,
    .obj_cmp = &comps_doc_cmp_u
};
