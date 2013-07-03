#include "comps.h"
#include <stdio.h>
#include <assert.h>

char __comps_prop_pair_cmp(void *ppair1, void *ppair2) {
    if (strcmp(((COMPS_RTreePair*)ppair1)->key,((COMPS_RTreePair*)ppair2)->key)
        != 0)
        return 0;
    return comps_doc_prop_cmp_v(((COMPS_RTreePair*)ppair1)->data,
                                ((COMPS_RTreePair*)ppair2)->data);
}

char comps_props_cmp(COMPS_Dict *p1, COMPS_Dict *p2) {
    COMPS_HSList *pairs1, *pairs2;
    COMPS_HSListItem *hsit;
    COMPS_Set *set1, *set2;
    char ret;

    set1 = comps_set_create();
    comps_set_init(set1, NULL, NULL, NULL, &__comps_prop_pair_cmp);
    set2 = comps_set_create();
    comps_set_init(set2, NULL, NULL, NULL, &__comps_prop_pair_cmp);

    pairs1 = comps_rtree_pairs(p1);
    pairs2 = comps_rtree_pairs(p2);
    for (hsit = pairs1->first; hsit != NULL; hsit = hsit->next) {
    //printf("%s=", ((COMPS_RTreePair*)hsit->data)->key);
    if (((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop_type == COMPS_PROP_STR){
        //printf("%s\n", ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.str);
    } else
        //printf("%d\n", ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.num);

        comps_set_add(set1, hsit->data);
    }
    for (hsit = pairs2->first; hsit != NULL; hsit = hsit->next) {
    //printf("%s=", ((COMPS_RTreePair*)hsit->data)->key);
    if (((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop_type == COMPS_PROP_STR){
        //printf("%s\n", ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.str);
    } else
        //printf("%d\n", ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.num);

        comps_set_add(set2, hsit->data);
    }

    ret = (comps_set_cmp(set1, set2) == 0);
    comps_set_destroy(&set1);
    comps_set_destroy(&set2);
    comps_hslist_destroy(&pairs1);
    comps_hslist_destroy(&pairs2);
    return ret;
}

void __comps_doc_char_setter(void **attr, char *val, char copy) {
    char *new;
    if (copy && val != NULL) {
        if ((new = malloc(sizeof(char)*(strlen(val)+1))) == NULL) return;
        memcpy(new, val, sizeof(char)*(strlen(val)+1));
    } else new = val;
    if (*attr != NULL) free(*attr);
    *attr = new;
}

inline char __comps_doccategory_idcmp(void* cat1, void *cat2) {
    return __comps_strcmp(
  ((COMPS_Prop*)
   comps_dict_get(((COMPS_DocCategory*)cat1)->properties, "id"))->prop.str,
  ((COMPS_Prop*)
   comps_dict_get(((COMPS_DocCategory*)cat2)->properties, "id"))->prop.str);
}
inline char __comps_docgroup_idcmp(void* g1, void *g2) {
    return __comps_strcmp(
    ((COMPS_Prop*)comps_dict_get(((COMPS_DocGroup*)g1)->properties, "id"))->prop.str,
    ((COMPS_Prop*)comps_dict_get(((COMPS_DocGroup*)g2)->properties, "id"))->prop.str);
}
inline char __comps_docenv_idcmp(void* e1, void *e2) {
    return __comps_strcmp(
        ((COMPS_Prop*)
         comps_dict_get(((COMPS_DocEnv*)e1)->properties, "id"))->prop.str,
        ((COMPS_Prop*)
         comps_dict_get(((COMPS_DocEnv*)e2)->properties, "id"))->prop.str);
}

inline void __comps_doc_add_lang_prop(COMPS_Dict *dict, char *lang, char *prop,
                               char copy) {
    char *nprop;
    if (copy) {
        nprop = __comps_str_clone(prop);
        comps_dict_set(dict, lang, nprop);
    } else
        comps_dict_set(dict, lang, prop);
}

inline char __comps_doc_write_simple_prop(xmlTextWriterPtr writer,
                                        char *prop, char *val) {
    if (val != NULL) {
        xmlTextWriterStartElement(writer, (xmlChar*)prop);
        xmlTextWriterWriteString(writer, (xmlChar*)val);
        xmlTextWriterEndElement(writer);
        return 1;
    } return 0;
}

inline void __comps_doc_write_lang_prop(xmlTextWriterPtr writer,
                                        COMPS_Dict *lang_dict, char *prop) {
    COMPS_HSList * hslist;
    COMPS_HSListItem *hsit;
    char *value;

    if (!lang_dict) return;
    hslist = comps_dict_keys(lang_dict);
    for (hsit = hslist->first; hsit != NULL; hsit = hsit->next) {
        value = comps_dict_get(lang_dict, (char*)hsit->data);
        xmlTextWriterStartElement(writer, (xmlChar*)prop);
        xmlTextWriterWriteAttribute(writer, (xmlChar*) "xml:lang",
                                            (xmlChar*) hsit->data);
        xmlTextWriterWriteString(writer, (xmlChar*)value);
        xmlTextWriterEndElement(writer);
    }
    comps_hslist_destroy(&hslist);
}

/**
 * COMPS_Doc constructor
 * Only creates COMPS_Doc object.
 * @see comps_doc_destroy
 * @return COMPS_Doc
 */
COMPS_Doc* comps_doc_create(const char *encoding)
{
    COMPS_Doc *doc;
    if ((doc = malloc(sizeof(COMPS_Doc))) == NULL)
        return NULL;
    doc->lobjects = comps_dict_create(NULL, NULL, &comps_list_destroy_v);
    doc->dobjects = comps_dict_create(NULL, &comps_dict_clone_v,
                                            &comps_dict_destroy_v);
    doc->mdobjects = comps_dict_create(NULL, &comps_mdict_clone_v,
                                            &comps_mdict_destroy_v);
    doc->log = comps_log_create(0);
    doc->encoding = encoding;
    return doc;
}

/**
 * COMPS_Doc initializer
 * @param doc COMPS_Doc object
 */
inline void comps_doc_init(COMPS_Doc *doc)
{
    (void)doc;
}

/**
 * Clear all data in COMPS_Doc object
 * @param doc COMPS_Doc object
 */
inline void comps_doc_clear(COMPS_Doc *doc)
{
    if (doc == NULL) return;
    comps_list_clear(comps_doc_groups(doc));
    comps_list_clear(comps_doc_categories(doc));
    comps_list_clear(comps_doc_environments(doc));
}

/**
 * COMPS_Doc destructor
 * Destroy COMPS_Doc object set its reference to NULL
 * @param doc reference to COMPS_Doc object
 */
void comps_doc_destroy(COMPS_Doc **doc) {
    if (*doc != NULL) {
        comps_dict_destroy((*doc)->lobjects);
        comps_dict_destroy((*doc)->dobjects);
        comps_dict_destroy((*doc)->mdobjects);
        comps_log_destroy((*doc)->log);
        free(*doc);
        doc = NULL;
    }
}

void comps_doc_destroy_v(void *doc)
{
    if (doc) {
        comps_dict_destroy(((COMPS_Doc*)doc)->lobjects);
        comps_dict_destroy(((COMPS_Doc*)doc)->dobjects);
        comps_dict_destroy(((COMPS_Doc*)doc)->mdobjects);
        comps_log_destroy(((COMPS_Doc*)doc)->log);
        free(doc);
    }
}

/**
 * Get list of groups from COMPS_Doc object matching passed arguments
 * Matching is evaluated by AND style (id-match && name-mach && desc-match)
 * Result is only shadow filter of original list (if you destroy original list
 * shadow list will become invalid)
 * @param doc COMPS_Doc object
 * @param id group id which is compared against other groups ids.
 * NULL means exclusion from final matching filter
 * @param name group name which is compared against other groups names
 * NULL means exclusion from final matching filter
 * @param desc group description which is compared against other groups description
 * NULL means exclusion from final matching filter
 * @return COMPS_List object of matched COMPS_DocGroup objects
 * @see COMPS_List
 * @see COMPS_DocGroup
*/
COMPS_List* comps_doc_get_groups(COMPS_Doc *doc, char *id, char *name,
                                                 char *desc, char *lang) {
    COMPS_ListItem *it, *it2;
    COMPS_List *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    COMPS_List *groups = comps_doc_groups(doc);
    char *lang_match;
    COMPS_Prop *tmp_prop;

    ret = comps_list_create();
    comps_list_init(ret);

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = groups->first; it != NULL; it = it->next) {
        matched = 0;
        tmp_prop = comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "id");
        if (id != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, id) == 0)
            matched++;
        tmp_prop = comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "name");
        if (name != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, name) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocGroup*)it->data)->name_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, name)) matched++;
        }
        tmp_prop = comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "desc");
        if (desc != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, desc) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocGroup*)it->data)->desc_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, desc)) matched++;
        }
        if (matched == matched_max) {
            it2 = comps_list_item_create(it->data, NULL, NULL);
            comps_list_append(ret, it2);
        }
    }
    return ret;
}

/**
 * Get list of categories from COMPS_Doc object matching passed arguments
 * Matching is evaluated by AND style (id-match && name-mach && desc-match)
 * Result is only shadow filter of original list (if you destroy original list
 * shadow list will become invalid)
 * @param doc COMPS_Doc object
 * @param id category id which is compared against other categories ids.
 * NULL means exclusion from final matching filter
 * @param name category name which is compared against other categories names
 * NULL means exclusion from final matching filter
 * @param desc category description which is compared against other categories
 * descriptions
 * NULL means exclusion from final matching filter
 * @return COMPS_List object of matched COMPS_DocCategory objects
 * @see COMPS_List
 * @see COMPS_DocCategory
*/
COMPS_List* comps_doc_get_categories(COMPS_Doc *doc, char *id, char *name,
                                     char *desc, char*lang) {
    COMPS_ListItem *it, *it2;
    COMPS_List *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    COMPS_List *categories = comps_doc_categories(doc);
    char *lang_match;
    COMPS_Prop *tmp_prop;

    ret = comps_list_create();
    comps_list_init(ret);

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = categories->first; it != NULL; it = it->next) {
        matched = 0;
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "id");
        if (id != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, id) == 0)
            matched++;
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "name");
        if (name != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, name) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocCategory*)it->data)->name_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, name)) matched++;
        }
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "desc");
        if (desc != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, desc) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocCategory*)it->data)->desc_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, desc)) matched++;
        }
        if (matched == matched_max) {
            it2 = comps_list_item_create(it->data, NULL, NULL);
            comps_list_append(ret, it2);
        }
    }
    return ret;
}

/**
 * Get list of environments from COMPS_Doc object matching passed arguments
 * Matching is evaluated by AND style (id-match && name-mach && desc-match).
 * Result is only shadow filter of original list (if you destroy original list
 * shadow list will become invalid)
 * @param doc COMPS_Doc object
 * @param id environment id which is compared against other environment ids.
 * NULL means exclusion from final matching filter
 * @param name environment name which is compared against other environments
 * names
 * NULL means exclusion from final matching filter
 * @param desc environment description which is compared against other
 * environments descriptions
 * NULL means exclusion from final matching filter
 * @return COMPS_List object of matched COMPS_DocCategory objects
 * @see COMPS_List
 * @see COMPS_DocCategory
*/
COMPS_List* comps_doc_get_envs(COMPS_Doc *doc, char *id, char *name,
                                                 char *desc, char *lang) {
    COMPS_ListItem *it, *it2;
    COMPS_List *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    COMPS_List *envs = comps_doc_environments(doc);
    char *lang_match;
    COMPS_Prop *tmp_prop;

    ret = comps_list_create();
    comps_list_init(ret);

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = envs->first; it != NULL; it = it->next) {
        matched = 0;
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "id");
        if (id != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, id) == 0)
            matched++;
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "name");
        if (name != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, name) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocEnv*)it->data)->name_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, name)) matched++;
        }
        tmp_prop = comps_dict_get(((COMPS_DocCategory*)it->data)->properties, "desc");
        if (desc != NULL && tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR
            && __comps_strcmp(tmp_prop->prop.str, desc) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocEnv*)it->data)->desc_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, desc)) matched++;
        }
        if (matched == matched_max) {
            it2 = comps_list_item_create(it->data, NULL, NULL);
            comps_list_append(ret, it2);
        }
    }
    return ret;
}

/**
 * Export COMPS_Doc object to xml file
 * @param doc * COMPS_Doc object
 * @param filename output filename
 */
void comps2xml_f(COMPS_Doc * doc, char *filename, char stdoutredirect) {
    xmlDocPtr xmldoc;
    int retc;

    doc->log->redirect2output = stdoutredirect;
    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&xmldoc, 0);

    retc = xmlTextWriterStartDocument(writer, NULL, doc->encoding, NULL);
    if (retc<0)
        comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
    comps_doc_xml(doc, writer);
    retc = xmlTextWriterEndDocument(writer);
    if (retc<0)
        comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);

    retc = xmlSaveFormatFileEnc(filename, xmldoc, NULL, 1);
    if (retc<0)
        comps_log_error(doc->log, filename, COMPS_ERR_WRITEF, 0, 0, 0);

    xmlFreeTextWriter(writer);
    xmlFreeDoc(xmldoc);
    xmlCleanupParser();
    xmlMemoryDump();
}

/**
 * Export COMPS_Doc object to string. Result need to be freed.
 * @param doc * COMPS_Doc object
 * @return string containing xml representation of COMPS_Doc object
 */
char *comps2xml_str(COMPS_Doc * doc) {
    xmlDocPtr xmldoc;
    const char *xmlstr;
    char * ret;
    int retc;

    xmlBuffer * xmlbuff = xmlBufferCreate();
    xmlOutputBuffer * xmlobuff = xmlOutputBufferCreateBuffer(xmlbuff, 0);
    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&xmldoc, 0);

    retc = xmlTextWriterStartDocument(writer, NULL, doc->encoding, NULL);
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
    comps_doc_xml(doc, writer);
    retc = xmlTextWriterEndDocument(writer);
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
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

void comps_doc_xml(COMPS_Doc *doc, xmlTextWriterPtr writer) {
    COMPS_ListItem *it;
    COMPS_List *list;
    int retc;

    retc = xmlTextWriterWriteDTD(writer, BAD_CAST "comps",
                                         BAD_CAST "-//Red Hat, Inc.//DTD Comps"
                                                  " info//EN",
                                         BAD_CAST "comps.dtd", NULL);
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);

    retc = xmlTextWriterStartElement(writer, BAD_CAST "comps");
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
    list = comps_doc_groups(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            comps_docgroup_xml((COMPS_DocGroup*)it->data, writer, doc->log);
        }
    }
    list = comps_doc_categories(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            comps_doccategory_xml((COMPS_DocCategory*)it->data, writer, doc->log);
        }
    }
    list = comps_doc_environments(doc);
    if (list) {
        for (it = list->first; it != NULL; it = it->next) {
            comps_docenv_xml((COMPS_DocEnv*)it->data, writer, doc->log);
        }
    }
    retc = xmlTextWriterEndElement(writer);
    if (retc<0) comps_log_error(doc->log, NULL, COMPS_ERR_XMLGEN, 0, 0, 0);
}

/**
 * Make union of two existing COMPS_Doc objects. Result union is completly
 * new COMPS_Doc object (deep copy of those two). If both of COMPS_Doc objects
 * contain groups,categories or environments object with same id, resulting
 * group/category/environment object is created by union of
 * incrimined groups/categories/environments objects.
 * @see comps_docgroup_union
 * @see comps_doccategory_union
 * @see comps_docenv_union
 * @param c1 COMPS_Doc object
 * @param c2 COMPS_Doc object
 * @return new COMPS_Doc object
 */
COMPS_Doc* comps_doc_union(COMPS_Doc *c1, COMPS_Doc *c2) {
    COMPS_ListItem *it;
    COMPS_Set *set;
    COMPS_Doc *res;
    COMPS_DocGroup *tmpgroup;
    COMPS_DocCategory *tmpcat;
    COMPS_DocEnv *tmpenv;
    COMPS_HSListItem *hsit;
    COMPS_List *groups = comps_doc_groups(c1);
    COMPS_List *categories = comps_doc_categories(c1);
    COMPS_List *envs = comps_doc_environments(c1);

    void *tmpdata;
    res = comps_doc_create(c1->encoding);
    comps_doc_init(res);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroup_idcmp);

    for (it = groups ? groups->first : NULL; it != NULL; it = it->next) {
        comps_set_add(set, comps_docgroup_clone(it->data));
        //printf("add roup with id: %s \n",
       //((COMPS_Prop*)comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "id"))->prop.str);
    }
    groups = comps_doc_groups(c2);
    for (it = groups ? groups->first : NULL; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
            //printf("group with id: %s found\n",
       //((COMPS_Prop*)comps_dict_get(((COMPS_DocGroup*)it->data)->properties, "id"))->prop.str);
            tmpgroup = comps_docgroup_union(
                                (COMPS_DocGroup*)it->data,
                                (COMPS_DocGroup*)comps_set_data_at(set,
                                                                   it->data));
            tmpdata = comps_set_data_at(set, it->data);
            comps_set_remove(set, it->data);
            comps_docgroup_destroy(tmpdata);
            comps_set_add(set, tmpgroup);
        } else {
            comps_set_add(set, comps_docgroup_clone(it->data));
        }
    }
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_doc_add_group(res, hsit->data);
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_doccategory_idcmp);
    for (it = categories ? categories->first : NULL; it != NULL; it = it->next) {
        comps_set_add(set, comps_doccategory_clone(it->data));
    }
    categories = comps_doc_categories(c2);
    for (it = categories ? categories->first : NULL; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
            tmpcat = comps_doccategory_union(
                                (COMPS_DocCategory*)it->data,
                                (COMPS_DocCategory*)comps_set_data_at(set,
                                                                    it->data));
            tmpdata = comps_set_data_at(set, it->data);
            comps_set_remove(set, it->data);
            comps_doccategory_destroy(tmpdata);
            comps_set_add(set, tmpcat);
        } else {
            comps_set_add(set, comps_doccategory_clone(it->data));
        }
    }
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_doc_add_category(res, hsit->data);
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_docenv_idcmp);
    if (envs) {
        for (it = envs->first; it != NULL; it = it->next) {
            comps_set_add(set, comps_docenv_clone(it->data));
        }
    }
    envs = comps_doc_environments(c2);
    if (envs) {
        for (it = envs->first; it != NULL; it = it->next) {
            if (comps_set_in(set, it->data)) {
                tmpenv = comps_docenv_union(
                                    (COMPS_DocEnv*)it->data,
                                    (COMPS_DocEnv*)comps_set_data_at(set,
                                                                     it->data));
                tmpdata = comps_set_data_at(set, it->data);
                comps_set_remove(set, it->data);
                comps_docenv_destroy(tmpdata);
                comps_set_add(set, tmpenv);
            } else {
                comps_set_add(set, comps_docenv_clone(it->data));
            }
        }
    }
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_doc_add_env(res, hsit->data);
    }
    comps_set_destroy(&set);
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
    COMPS_ListItem *it;
    COMPS_Set *set;
    COMPS_Doc *res;
    COMPS_DocGroup *tmpgroup;
    COMPS_DocCategory *tmpcat;
    COMPS_DocEnv *tmpenv;
    COMPS_List *groups = comps_doc_groups(c1);
    COMPS_List *categories = comps_doc_categories(c1);
    COMPS_List *envs = comps_doc_environments(c1);

    set = comps_set_create();
    res = comps_doc_create(c1->encoding);
    comps_doc_init(res);

    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroup_idcmp);

    for (it = groups->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    groups = comps_doc_groups(c2);
    for (it = groups->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
            tmpgroup = comps_docgroup_intersect(
                                (COMPS_DocGroup*)it->data,
                                (COMPS_DocGroup*)comps_set_data_at(set,
                                                                   it->data));
            comps_doc_add_group(res, tmpgroup);
        }
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_doccategory_idcmp);
    for (it = categories->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    categories = comps_doc_categories(c2);
    for (it = categories->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
            tmpcat = comps_doccategory_intersect(
                                (COMPS_DocCategory*)it->data,
                                (COMPS_DocCategory*)comps_set_data_at(set,
                                                                    it->data));
            comps_doc_add_category(res, tmpcat);
        }
    }
    comps_set_clear(set);

    comps_set_init(set, NULL, NULL, NULL, &__comps_docenv_idcmp);
    for (it = envs->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    envs = comps_doc_environments(c2);
    for (it = envs->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
            tmpenv = comps_docenv_intersect(
                                (COMPS_DocEnv*)it->data,
                                (COMPS_DocEnv*)comps_set_data_at(set,it->data));
            comps_doc_add_env(res, tmpenv);
        }
    }
    comps_set_destroy(&set);
    return res;
}

/**
 * COMPS_DocGroup constructor
 * @return COMPS_DocGroup object
 */
COMPS_DocGroup* comps_docgroup_create()
{
    COMPS_DocGroup *group;
    if ((group = malloc(sizeof(COMPS_DocGroup))) == NULL)
        return NULL;
    group->properties = comps_dict_create(NULL, &comps_doc_prop_clone_v,
                                                &comps_doc_prop_destroy_v);
    /*group->id = NULL;
    group->name = NULL;*/
    group->name_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    //group->desc = NULL;
    group->desc_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    //group->def = 2;
    //group->uservisible = 2;
    group->packages = NULL;
    //group->lang_only = NULL;
    return group;
}

/**
 * COMPS_DocGroup destructor
 * @param group COMPS_DocGroup object
 */
void comps_docgroup_destroy(void *group)
{
    //free(((COMPS_DocGroup*)group)->id);
    //free(((COMPS_DocGroup*)group)->name);
    //free(((COMPS_DocGroup*)group)->lang_only);
    //free(((COMPS_DocGroup*)group)->desc);
    comps_dict_destroy(((COMPS_DocGroup*)group)->properties);
    comps_dict_destroy(((COMPS_DocGroup*)group)->name_by_lang);
    comps_dict_destroy(((COMPS_DocGroup*)group)->desc_by_lang);
    comps_list_destroy(&((COMPS_DocGroup*)group)->packages);
    free(group);
}

/**
 * Set group id. If copy param is 1, copy passed id buffer to new buffer and set
 * id to newly created buffer. If copy param is 0, only reassign buffer pointer
 * \warning Destroying group or another comps_docgroup_set_id call
 * \warning free current group id. Passing copy=0 only prevents from
 * \warning double copying
 * @param group COMPS_DocGroup object
 * @param id new group id
 * @)param copy if copy indicator
 */
inline void comps_docgroup_set_id(COMPS_DocGroup *group, char *id, char copy) {
    if (id) {
        __comps_doc_add_prop(group->properties, "id",
                             comps_doc_prop_str_create(id, copy));
    }
//    __comps_doc_char_setter((void**)&group->id, id, copy);
}

/**
 * Set group name. If copy param is 1, copy passed name buffer to new buffer
 * and set name to newly created buffer. If copy param is 0, only reassign
 * buffer pointer
 * \warning Destroying group or another comps_docgroup_set_name call
 * \warning free current group id. Passing copy=0 only prevents from
 * \warning double copying
 * @param group COMPS_DocGroup object
 * @param name new group name
 * @param copy if copy indicator
 */
inline void comps_docgroup_set_name(COMPS_DocGroup *group, char *name,
                                    char copy) {
    if (name) {
        __comps_doc_add_prop(group->properties, "name",
                             comps_doc_prop_str_create(name, copy));
    }
//    __comps_doc_char_setter((void**)&group->name, name, copy);
}
inline void comps_docgroup_add_lang_name(COMPS_DocGroup *group, char *lang,
                                  char *name, char copy) {
    __comps_doc_add_lang_prop(group->name_by_lang, lang, name, copy);
}

/**
 * Set group description. If copy param is 1, copy passed description buffer
 * to new buffer and set description to newly created buffer.
 * If copy param is 0, only reassign buffer pointer
 * \warning Destroying group or another comps_docgroup_set_desc call
 * \warning free current group desc. Passing copy=0 only prevents from
 * \warning double copying
 * @param group COMPS_DocGroup object
 * @param desc new group description
 * @param copy if copy indicator
 */
inline void comps_docgroup_set_desc(COMPS_DocGroup *group, char *desc,
                                    char copy) {
    if (desc) {
        __comps_doc_add_prop(group->properties, "desc",
                             comps_doc_prop_str_create(desc, copy));
    }
    //__comps_doc_char_setter((void**)&group->desc, desc, copy);
}
inline void comps_docgroup_add_lang_desc(COMPS_DocGroup *group, char *lang,
                                  char *desc, char copy) {
    __comps_doc_add_lang_prop(group->desc_by_lang, lang, desc, copy);
}

/**
 * Set group default flag
 * @param group COMPS_DocGroup
 * @param def default flag
 */
inline void comps_docgroup_set_default(COMPS_DocGroup *group, unsigned def) {
    __comps_doc_add_prop(group->properties, "default",
                         comps_doc_prop_num_create(def));
    //group->def = def;
}

/**
 * Set group uservisible flag
 * @param group COMPS_DocGroup
 * @param uservisible flag
 */
inline void comps_docgroup_set_uservisible(COMPS_DocGroup *group,
                                           unsigned uservisible) {
    __comps_doc_add_prop(group->properties, "uservisible",
                         comps_doc_prop_num_create(uservisible));
    //group->uservisible = uservisible;
}

inline void comps_docgroup_set_displayorder(COMPS_DocGroup *group,
                                           unsigned display_order) {
    __comps_doc_add_prop(group->properties, "display_order",
                         comps_doc_prop_num_create(display_order));
    //group->uservisible = uservisible;
}

/**
 * Set group lang_only flag
 * @param group COMPS_DocGroup
 * @param langonly flag
 */
inline void comps_docgroup_set_langonly(COMPS_DocGroup *group,
                                        char *langonly, char copy) {
    if (langonly) {
        __comps_doc_add_prop(group->properties, "lang_only",
                             comps_doc_prop_str_create(langonly, copy));
    }
    //__comps_doc_char_setter((void**)&group->lang_only, langonly, copy);
}
/**
 * Add COMPS_DocGroupPackage object to group
 * @param group COMPS_DocGroup object
 * @param package COMPS_DocGroupPackage object
 */
void comps_docgroup_add_package(COMPS_DocGroup *group,
                                COMPS_DocGroupPackage *package)
{
    if (group == NULL) {
        assert(0);
        return;
    }
    if (group->packages == NULL) {
        group->packages = comps_list_create();
        comps_list_init(group->packages);
    }
    if (package == NULL) {
        assert(0);
        return;
    }
    COMPS_ListItem *it;
    it = comps_list_item_create((void*)package,
                                NULL, &comps_docpackage_destroy);
    comps_list_append(group->packages, it);
}

/**
 * Get COMPS_List of COMPS_DocGroupPackage objects matching specified package
 * name and type (AND matching - match name && match type). Returned list is
 * only shadow filter of original list (if original is destroyed, result is
 * invalid)
 * @param group COMPS_DocGroup object
 * @param name filtered name of package
 * @param type filtered type of package
 */
COMPS_List* comps_docgroup_get_packages(COMPS_DocGroup *group, char *name,
                                       COMPS_PackageType type) {
    COMPS_ListItem *it, *it2;
    COMPS_List *ret;
    unsigned int matched, matched_max;
    matched_max = 0;
    
    ret = comps_list_create();
    comps_list_init(ret);

    if (name != NULL) matched_max++;
    if (type != COMPS_PACKAGE_UNKNOWN) matched_max++;

    for (it = group->packages->first; it != NULL; it = it->next) {
        matched = 0;
        if ((name != NULL)
            && strcmp(((COMPS_DocGroupPackage*)it->data)->name, name) == 0)
            matched++;
        if ((type != COMPS_PACKAGE_UNKNOWN)
            && ((COMPS_DocGroupPackage*)it->data)->type  == type)
            matched++;
        if (matched == matched_max) {
            it2 = comps_list_item_create(it->data, NULL, NULL);
            comps_list_append(ret, it2);
        }
    }
    return ret;
}

void comps_docgroup_xml(COMPS_DocGroup *group, xmlTextWriterPtr writer,
                        COMPS_Logger *log) {
    static char *elem_missing = "%s at group with id=%s";
    char *err_msg, *tmp_ch, *id;
    int tmp_num;
    COMPS_Prop * tmp_prop;
    COMPS_ListItem *it;

    xmlTextWriterStartElement(writer, (xmlChar*)"group");

    tmp_prop = comps_dict_get(group->properties, "id");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        id = tmp_prop->prop.str;
    else id = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_ID]
        && !__comps_doc_write_simple_prop(writer, "id", id)) {
        comps_log_error(log, "<id>", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }

    tmp_prop = comps_dict_get(group->properties, "name");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        tmp_ch = tmp_prop->prop.str;
    else tmp_ch = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_NAME]
        && !__comps_doc_write_simple_prop(writer, "name", tmp_ch)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<name>") + strlen(id)));
        sprintf(err_msg, elem_missing, "<name>", id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, group->name_by_lang, "name");

    tmp_prop = comps_dict_get(group->properties, "desc");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        tmp_ch = tmp_prop->prop.str;
    else tmp_ch = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_DESC]
        && !__comps_doc_write_simple_prop(writer, "description", tmp_ch)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<description>") + strlen(id)));
        sprintf(err_msg, elem_missing, "<description>", id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, group->desc_by_lang, "description");


    tmp_prop = comps_dict_get(group->properties, "def");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_NUM) {
        tmp_num = tmp_prop->prop.num;
        xmlTextWriterStartElement(writer, (xmlChar*)"default");
        xmlTextWriterWriteFormatString(writer, "%s", (tmp_num)?"true":"false");
        xmlTextWriterEndElement(writer);
    }

    tmp_prop = comps_dict_get(group->properties, "uservisible");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_NUM) {
        tmp_num = tmp_prop->prop.num;
        xmlTextWriterStartElement(writer, (xmlChar*)"uservisible");
        xmlTextWriterWriteFormatString(writer, "%s",(tmp_num)?"true":"false");
        xmlTextWriterEndElement(writer);
    }

    tmp_prop = comps_dict_get(group->properties, "display_order");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_NUM) {
        tmp_num = tmp_prop->prop.num;
        xmlTextWriterStartElement(writer, (xmlChar*)"display_order");
        xmlTextWriterWriteFormatString(writer, "%d", tmp_num);
        xmlTextWriterEndElement(writer);
    }

    tmp_prop = comps_dict_get(group->properties, "lang_only");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        tmp_ch = tmp_prop->prop.str;
    else tmp_ch = NULL;
    if (tmp_ch) {
        xmlTextWriterStartElement(writer, (xmlChar*)"langonly");
        xmlTextWriterWriteFormatString(writer, "%s", tmp_ch);
        xmlTextWriterEndElement(writer);
    } else if (comps_xml_elems_required[COMPS_ELEM_LANGONLY]) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<langonly>") + strlen(id)));
        sprintf(err_msg, elem_missing, "<langonly>", id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    if (group->packages) {
        xmlTextWriterStartElement(writer, (xmlChar*)"packagelist");
        if (!group->packages->first
           && comps_xml_elems_required[COMPS_ELEM_PACKAGEREQ]) {
            err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                             + strlen("<packagereq>") + strlen(id)));
            sprintf(err_msg, elem_missing, "<packagereq>", id);
            comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
            free(err_msg);
        }
        for (it = group->packages->first; it != NULL; it = it->next) {
            comps_docpackage_xml((COMPS_DocGroupPackage*)it->data, writer,
                                 log);
        }
        xmlTextWriterEndElement(writer);
    } else if (comps_xml_elems_required[COMPS_ELEM_PACKAGELIST]) {
        comps_log_error(log, "packagelist", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    xmlTextWriterEndElement(writer);
}
/**
 * Deep copy of COMPS_DocGroup object
 * @param g COMPS_DocGroup object
 * @return new copy of group
 */
COMPS_DocGroup* comps_docgroup_clone(COMPS_DocGroup *g) {
    COMPS_DocGroup *res;
    COMPS_ListItem *it;

    res = comps_docgroup_create();
    //comps_docgroup_set_name(res, g->name, 1);
    //comps_docgroup_set_id(res, g->id, 1);
    //comps_docgroup_set_desc(res, g->desc, 1);
    //res->def = g->def;
    //res->lang_only = __comps_str_clone(g->lang_only);
    //res->uservisible = g->uservisible;
    comps_dict_destroy(res->name_by_lang);
    comps_dict_destroy(res->desc_by_lang);
    comps_dict_destroy(res->properties);
    res->properties = comps_dict_clone(g->properties);
    res->name_by_lang = comps_dict_clone(g->name_by_lang);
    res->desc_by_lang = comps_dict_clone(g->desc_by_lang);

    res->packages = comps_list_create();
    comps_list_init(res->packages);
    if (g->packages) {
        for (it = g->packages->first; it != NULL; it = it->next) {
            comps_docgroup_add_package(res, comps_docpackage_clone(it->data));
        }
    }
    return res;
}

/**
 * Union two groups and return completly new COMPS_DocGroup object of union
 * result. Packages in groups are compared by name and type.
 * Name, id, description union policy:
 *  if name/id/desc of one group isn't set, result will have name/id/decs of
 *  other one.
 *  if name/id/desc is set in both groups, result will have name/id/desc of
 *  first group
 *  otherwise name/ids/desc won't be set in result
 * @param g1 COMPS_DocGroup object
 * @param g2 COMPS_DocGroup object
 * @return new COMPS_DocGroup object of union
 */
COMPS_DocGroup* comps_docgroup_union(COMPS_DocGroup *g1, COMPS_DocGroup *g2) {
    COMPS_DocGroup *res;
    COMPS_ListItem *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_DocGroupPackage *newpkg;
    COMPS_HSList *pairs;

    res = comps_docgroup_create();
    comps_dict_destroy(res->properties);
    /*pairs = comps_dict_pairs(g2->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        printf("%s = ", ((COMPS_RTreePair*)hsit->data)->key);
        printf("%s\n", ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.str);
    }
    comps_hslist_destroy(&pairs);*/

    res->properties = comps_dict_clone(g2->properties);
    pairs = comps_dict_pairs(g1->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        comps_dict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                       comps_doc_prop_clone(((COMPS_RTreePair*)hsit->data)->data));
    }
    comps_hslist_destroy(&pairs);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_docpackage_cmp);
    it = g1->packages?g1->packages->first:NULL;
    for (; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    it = g2->packages?g2->packages->first:NULL;
    for (; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    res->packages = comps_list_create();
    comps_list_init(res->packages);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        newpkg = comps_docpackage_clone((COMPS_DocGroupPackage*)hsit->data);
        comps_docgroup_add_package(res, newpkg);
    }
    comps_set_destroy(&set);
    return res;
}

/**
 * Intersect two groups and return completly new COMPS_DocGroup object of
 * intersection result. Packages in groups are compared by name and type.
 * Name, id, description intersect policy:
 *  if name/id/desc of one group isn't set, result will have name/id/decs of
 *  other one.
 *  if name/id/desc is set in both groups, result will have name/id/desc of
 *  first group
 *  otherwise name/ids/desc won't be set in result
 * @param g1 COMPS_DocGroup object
 * @param g2 COMPS_DocGroup object
 * @return new COMPS_DocGroup object of intersection result
 */
COMPS_DocGroup* comps_docgroup_intersect(COMPS_DocGroup *g1,
                                         COMPS_DocGroup *g2) {
    COMPS_DocGroup *res;
    COMPS_ListItem *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_DocGroupPackage *newpkg;
    COMPS_HSList *pairs1, *pairs2;

    res = comps_docgroup_create();
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_prop_pair_cmp);

    pairs1 = comps_dict_pairs(g1->properties);
    for (hsit = pairs1->first; hsit != NULL; hsit = hsit->next) {
        comps_set_add(set, hsit->data);
    }
    pairs2 = comps_dict_pairs(g2->properties);
    for (hsit = pairs2->first; hsit != NULL; hsit = hsit->next) {
        if (comps_set_in(set, hsit->data)) {
            comps_dict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                  comps_doc_prop_clone_v(((COMPS_RTreePair*)hsit->data)->data));
        }
    }
    comps_hslist_destroy(&pairs1);
    comps_hslist_destroy(&pairs2);
    comps_set_destroy(&set);
    res->packages = comps_list_create();
    comps_list_init(res->packages);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_docpackage_cmp);

    for (it = g1->packages->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    for (it = g2->packages->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
            newpkg = comps_docpackage_clone((COMPS_DocGroupPackage*)it->data);
            comps_docgroup_add_package(res, newpkg);
        }
    }
    comps_set_destroy(&set);
    return res;
}

inline char comps_docgroup_cmp(COMPS_DocGroup *g1, COMPS_DocGroup *g2) {
    COMPS_ListItem *it, *it2;
    it = (((COMPS_DocGroup*)g1)->packages)
         ?((COMPS_DocGroup*)g1)->packages->first: NULL;
    it2 = (((COMPS_DocGroup*)g2)->packages)
         ?((COMPS_DocGroup*)g2)->packages->first: NULL;
    
    for (; it != NULL && it2 != NULL; it = it->next, it2 = it2->next) {
        if (!__comps_strcmp(((COMPS_DocGroupPackage*)it->data)->name,
                             ((COMPS_DocGroupPackage*)it2->data)->name)) {
            return 0;
        }
        if (((COMPS_DocGroupPackage*)it->data)->type !=
            ((COMPS_DocGroupPackage*)it2->data)->type) {
            return 0;
        }
    }
    if (it != NULL || it2 != NULL)
        return 0;
    return comps_props_cmp(g1->properties, g2->properties);
}
inline char comps_docgroup_cmp_v(void *g1, void *g2) {
    return comps_docgroup_cmp((COMPS_DocGroup*)g1, (COMPS_DocGroup*)g2);
}

/**
 * COMPS_DocCategory constructor
 * @return new COMPS_DocCategory object
 */
COMPS_DocCategory* comps_doccategory_create()
{
    COMPS_DocCategory *category;
    if ((category = malloc(sizeof(COMPS_DocCategory))) == NULL)
        return NULL;
    //category->id = NULL;
    //category->name = NULL;
    //category->desc = NULL;
    category->properties = comps_dict_create(NULL, &comps_doc_prop_clone_v,
                                                   &comps_doc_prop_destroy_v);
    category->name_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    category->desc_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    category->group_ids = NULL;
    //category->display_order = 0;
    return category;
}

/**
 * COMPS_DocCategory destructor
 * @param category COMPS_DocCategory object
 */
void comps_doccategory_destroy(void *category)
{
    if (category == NULL) return;
    //free(((COMPS_DocCategory*)category)->id);
    //free(((COMPS_DocCategory*)category)->name);
    //free(((COMPS_DocCategory*)category)->desc);
    comps_dict_destroy(((COMPS_DocGroup*)category)->properties);
    comps_dict_destroy(((COMPS_DocCategory*)category)->name_by_lang);
    comps_dict_destroy(((COMPS_DocCategory*)category)->desc_by_lang);
    comps_list_destroy(&((COMPS_DocCategory*)category)->group_ids);
    free(category);
}

/**
 * Set category id. If copy param is 1, copy passed id buffer to new
 * buffer and set id to newly created buffer. If copy param is 0, only
 * reassign buffer pointer
 * \warning Destroying group or another comps_doccategory_set_id call
 * \warning free current category id. Passing copy=0 only prevents from
 * \warning double copying
 * @param category COMPS_DocCategory object
 * @param id id new category id
 * @param copy copy indicator
 */
inline void comps_doccategory_set_id(COMPS_DocCategory *category, char *id,
                                     char copy) {
    if (id) {
        __comps_doc_add_prop(category->properties, "id",
                             comps_doc_prop_str_create(id, copy));
    }
    //__comps_doc_char_setter((void**)&category->id, id, copy);
}

/**
 * Set category name. If copy param is 1, copy passed id buffer to new
 * buffer and set name to newly created buffer. If copy param is 0, only
 * reassign buffer pointer
 * \warning Destroying group or another comps_doccategory_set_name call
 * \warning free current category name. Passing copy=0 only prevents from
 * \warning double copying
 * @param category COMPS_DocCategory object
 * @param name new category name
 * @param copy copy indicator
 */
inline void comps_doccategory_set_name(COMPS_DocCategory *category, char *name,
                                       char copy) {
    if (name) {
        __comps_doc_add_prop(category->properties, "name",
                             comps_doc_prop_str_create(name, copy));
    }
    //__comps_doc_char_setter((void**)&category->name, name, copy);
}
inline void comps_doccategory_add_lang_name(COMPS_DocCategory *category,
                                            char *lang, char *name, char copy){
    __comps_doc_add_lang_prop(category->name_by_lang, lang, name, copy);
}

/**
 * Set category description. If copy param is 1, copy passed id buffer to
 * new buffer and set description to newly created buffer. If copy param
 * is 0, only reassign buffer pointer
 * \warning Destroying group or another comps_doccategory_set_desc call
 * \warning free current category description. Passing copy=0 only
 * \warning prevents from double copying
 * @param category COMPS_DocCategory object
 * @param desc new category id
 * @param copy copy indicator
 */
inline void comps_doccategory_set_desc(COMPS_DocCategory *category, char *desc,
                                       char copy) {
    if (desc) {
        __comps_doc_add_prop(category->properties, "desc",
                             comps_doc_prop_str_create(desc, copy));
    }
    //__comps_doc_char_setter((void**)&category->desc, desc, copy);
}
inline void comps_doccategory_add_lang_desc(COMPS_DocCategory *category,
                                            char *lang, char *desc, char copy){
    __comps_doc_add_lang_prop(category->desc_by_lang, lang, desc, copy);
}

/**
 * Set category display order
 * @param category COMPS_DocCategory object
 * @param display_order new display_order of category
 */
inline void comps_doccategory_set_displayorder(COMPS_DocCategory *category,
                                        int display_order) {
    __comps_doc_add_prop(category->properties, "display_order",
                         comps_doc_prop_num_create(display_order));
    //category->display_order = display_order;
}

/**
 * Add new groupid to category. If copy param is 1, copy passed id buffer
 * to new buffer and set description to newly created buffer.
 * If copy param is 0, only reassign buffer pointer
 * \warning Destroying category free current category description.
 * \warning Passing copy=0 only prevents from double copying
 * @param category COMPS_DocCategory object
 * @param id new groupid
 * @param copy copy indicator
 */
void comps_doccategory_add_groupid(COMPS_DocCategory *category,
                                   char *id, char copy)
{
    char *new;
    COMPS_ListItem *it;
    if (category == NULL) {
        return;
    }
    if (category->group_ids == NULL) {
        category->group_ids = comps_list_create();
        comps_list_init(category->group_ids);
    }
    if (copy) {
        if ((new = malloc(sizeof(char)*(strlen(id)+1))) == NULL) return;
        memcpy(new, id, sizeof(char)*(strlen(id)+1));
    } else new = id;
    it = comps_list_item_create(new, NULL, &free);
    comps_list_append(category->group_ids, it);
}

void comps_doccategory_xml(COMPS_DocCategory *cat, xmlTextWriterPtr writer,
                           COMPS_Logger *log) {
    char *err_msg, *tmp_ch, *id;
    static char *elem_missing = "%s at category with id=%s";
    COMPS_ListItem *it;
    int tmp_num;
    COMPS_Prop * tmp_prop;

    xmlTextWriterStartElement(writer, (xmlChar*)"category");

    tmp_prop = comps_dict_get(cat->properties, "id");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        id = tmp_prop->prop.str;
    else id = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_ID]
        && !__comps_doc_write_simple_prop(writer, "id", id)) {
        comps_log_error(log, "<id>", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }

    tmp_prop = comps_dict_get(cat->properties, "name");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        tmp_ch = tmp_prop->prop.str;
    else tmp_ch = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_NAME]
        && !__comps_doc_write_simple_prop(writer, "name", tmp_ch)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<name>") + strlen(id)));
        sprintf(err_msg, elem_missing, "<name>", id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, cat->name_by_lang, "name");

    tmp_prop = comps_dict_get(cat->properties, "desc");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        tmp_ch = tmp_prop->prop.str;
    else tmp_ch = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_DESC]
        && !__comps_doc_write_simple_prop(writer, "description", tmp_ch)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<description>") + strlen(id)));
        sprintf(err_msg, elem_missing, "<description>", id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, cat->desc_by_lang, "description");

    tmp_prop = comps_dict_get(cat->properties, "display_order");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_NUM)
        tmp_num = tmp_prop->prop.num;
    else tmp_num = 0;
    if (tmp_num) {
        xmlTextWriterStartElement(writer, (xmlChar*)"display_order");
        xmlTextWriterWriteFormatString(writer, "%u", tmp_num);
        xmlTextWriterEndElement(writer);
    }
    if (cat->group_ids) {
        xmlTextWriterStartElement(writer, (xmlChar*)"grouplist");
        if (!cat->group_ids->first
           && comps_xml_elems_required[COMPS_ELEM_GROUPID]) {
            err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<groupid>") + strlen(id)));
            sprintf(err_msg, elem_missing, "<groupid>", id);
            comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
            free(err_msg);
        }
        for (it = cat->group_ids->first; it != NULL; it = it->next) {
            xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
            xmlTextWriterWriteString(writer, (xmlChar*) it->data);
            xmlTextWriterEndElement(writer);
        }
        xmlTextWriterEndElement(writer);
    } else if (comps_xml_elems_required[COMPS_ELEM_GROUPLIST]){
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                     + strlen("<grouplist>") + strlen(id)));
        sprintf(err_msg, elem_missing, "<grouplist>", id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    xmlTextWriterEndElement(writer);
}

/**
 * Make deep copy of category
 * @param c COMPS_DocCategory object
 * @return new COMPS_DocCategory object
 */
COMPS_DocCategory* comps_doccategory_clone(COMPS_DocCategory *c) {
    COMPS_DocCategory *res;
    COMPS_ListItem *it;

    res = comps_doccategory_create();
    //comps_doccategory_set_name(res, c->name, 1);
    //comps_doccategory_set_id(res, c->id, 1);
    //comps_doccategory_set_desc(res, c->desc, 1);
    //res->display_order = c->display_order;

    comps_dict_destroy(res->properties);
    comps_dict_destroy(res->name_by_lang);
    comps_dict_destroy(res->desc_by_lang);
    res->properties = comps_dict_clone(c->properties);
    res->name_by_lang = comps_dict_clone(c->name_by_lang);
    res->desc_by_lang = comps_dict_clone(c->desc_by_lang);

    res->group_ids = comps_list_create();
    comps_list_init(res->group_ids);
    if (c->group_ids) { 
        for (it = c->group_ids->first; it != NULL; it = it->next) {
            comps_doccategory_add_groupid(res, it->data, 1);
        }
    }
    return res;
}

/**
 * Union two categories and return completly new COMPS_DocCategory object
 * of union result.
 * Name, id, description union policy:
 *  if name/id/desc of one category isn't set, result will have
 *  name/id/decs of other one.
 *  if name/id/desc is set in both categories, result will have
 *  name/id/desc of first category
 *  otherwise name/ids/desc won't be set in result
 * @param c1 COMPS_DocCategory object
 * @param c2 COMPS_DocCategory object
 * @return new COMPS_DocCategory object of union
 */
COMPS_DocCategory* comps_doccategory_union(COMPS_DocCategory *c1,
                                           COMPS_DocCategory *c2) {
    COMPS_DocCategory *res;
    COMPS_ListItem *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_HSList *pairs;

    res = comps_doccategory_create();
    comps_dict_destroy(res->properties);

//    pairs = comps_dict_pairs(c2->properties);
//    for (hsit = pairs->first; hsit != NULL; hsit= hsit->next)     {
//        printf("%s=", ((COMPS_RTreePair*)hsit->data)->key);
//        if (((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop_type == COMPS_PROP_STR)
//            printf("%s\n",
//                   ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.str);
//        else
//            printf("%d\n",
//                   ((COMPS_Prop*)((COMPS_RTreePair*)hsit->data)->data)->prop.num);
//    }
//    comps_hslist_destroy(&pairs);

    res->properties = comps_dict_clone(c2->properties);
    pairs = comps_dict_pairs(c1->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        comps_dict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                   comps_doc_prop_clone(((COMPS_RTreePair*)hsit->data)->data));
    }
    comps_hslist_destroy(&pairs);
    /*if (c1->name == NULL || c1->name[0] == 0) {
        comps_doccategory_set_name(res, c2->name, 1);
    } else if (c2->name == NULL || c2->name[0] == 0) {
        comps_doccategory_set_name(res, c1->name, 1);
    } else
        comps_doccategory_set_name(res, c1->name, 1);

    if (c1->id == NULL || c1->id[0] == 0) {
        comps_doccategory_set_id(res, c2->id, 1);
    } else if (c2->id == NULL || c2->id[0] == 0) {
        comps_doccategory_set_id(res, c1->id, 1);
    } else
        comps_doccategory_set_id(res, c1->id, 1);

    if (c1->desc == NULL || c1->desc[0] == 0) {
        comps_doccategory_set_desc(res, c2->desc, 1);
    } else if (c2->desc == NULL || c2->desc[0] == 0) {
        comps_doccategory_set_desc(res, c1->desc, 1);
    } else
        comps_doccategory_set_desc(res, c1->desc, 1);
    res->display_order = c1->display_order;
    */
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_strcmp);
    for (it = c1->group_ids->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    for (it = c2->group_ids->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    res->group_ids = comps_list_create();
    comps_list_init(res->group_ids);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_doccategory_add_groupid(res, (char*)hsit->data, 1);
    }
    comps_set_destroy(&set);
    return res;
}

/**
 * Intersect two categories and return completly new COMPS_DocCategory
 * object of intersection result.
 * Name, id, description intersect policy:
 *  if name/id/desc of one category isn't set, result will have
 *  name/id/decs of other one.
 *  if name/id/desc is set in both categories, result will have
 *  name/id/desc of first category
 *  otherwise name/ids/desc won't be set in result
 * @param c1 COMPS_DocCategory object
 * @param c2 COMPS_DocCategory object
 * @return new COMPS_DocCategory object of intersection
 */
COMPS_DocCategory* comps_doccategory_intersect(COMPS_DocCategory *c1,
                                         COMPS_DocCategory *c2) {
    COMPS_DocCategory *res;
    COMPS_ListItem *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;

    COMPS_HSList *pairs1, *pairs2;

    res = comps_doccategory_create();
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_prop_pair_cmp);

    pairs1 = comps_dict_pairs(c1->properties);
    for (hsit = pairs1->first; hsit != NULL; hsit = hsit->next) {
        comps_set_add(set, hsit->data);
    }
    pairs2 = comps_dict_pairs(c2->properties);
    for (hsit = pairs2->first; hsit != NULL; hsit = hsit->next) {
        if (comps_set_in(set, hsit->data)) {
            comps_dict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                  comps_doc_prop_clone_v(((COMPS_RTreePair*)hsit->data)->data));
        }
    }
    comps_hslist_destroy(&pairs1);
    comps_hslist_destroy(&pairs2);
    comps_set_destroy(&set);
    /*if (c1->name == NULL) {
        comps_doccategory_set_name(res, c2->name, 1);
    } else if (c2->name == NULL) {
        comps_doccategory_set_name(res, c1->name, 1);
    } else
        comps_doccategory_set_name(res, c1->name, 1);

    if (c1->id == NULL) {
        comps_doccategory_set_name(res, c2->id, 1);
    } else if (c2->id == NULL) {
        comps_doccategory_set_name(res, c1->id, 1);
    } else
        comps_doccategory_set_name(res, c1->id, 1);

    if (c1->desc == NULL) {
        comps_doccategory_set_name(res, c2->desc, 1);
    } else if (c2->desc == NULL) {
        comps_doccategory_set_name(res, c1->desc, 1);
    } else
        comps_doccategory_set_name(res, c1->desc, 1);
    */
    
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_strcmp);

    res->group_ids = comps_list_create();
    comps_list_init(res->group_ids);

    for (it = c1->group_ids->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    for (it = c2->group_ids->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
            comps_doccategory_add_groupid(res, (char*)it->data, 1);
        }
    }
    comps_set_destroy(&set);
    return res;
}

inline char comps_doccategory_cmp(COMPS_DocCategory *c1, COMPS_DocCategory *c2) {
    COMPS_ListItem *it, *it2;
    char ret;

    it = (c1->group_ids)?c1->group_ids->first: NULL;
    it2 = (c2->group_ids)?c2->group_ids->first: NULL;
    
    for (; it != NULL && it2 != NULL; it = it->next, it2 = it2->next) {
        if (!__comps_strcmp(it->data, it2->data)) {
            return 0;
        }
    }
    if (it != NULL || it2 != NULL) {
        return 0;
    }
    ret = comps_props_cmp(c1->properties, c2->properties);
    //printf("set cmp:%d\n", ret);
    return ret;
}
inline char comps_doccategory_cmp_v(void *c1, void *c2) {
    return comps_doccategory_cmp((COMPS_DocCategory*)c1, (COMPS_DocCategory*)c2);
}

/**
 * COMPS_DocEnv constructor
 * @return new COMPS_DocEnv object
 */
COMPS_DocEnv *comps_docenv_create() {
    COMPS_DocEnv *env;
    if ((env = malloc(sizeof(COMPS_DocEnv))) == NULL)
        return NULL;
    //env->id = NULL;
    //env->name = NULL;
    //env->desc = NULL;
    env->properties = comps_dict_create(NULL, &comps_doc_prop_clone_v,
                                              &comps_doc_prop_destroy_v);
    env->name_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    env->desc_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    env->group_list = NULL;
    env->option_list = NULL;
    return env;
}

/**
 * COMPS_DocEnv destructor
 * @param env COMPS_DocEnv object
 */
void comps_docenv_destroy(void *env) {
    //free(((COMPS_DocEnv*)env)->id);
    //free(((COMPS_DocEnv*)env)->desc);
    //free(((COMPS_DocEnv*)env)->name);
    comps_dict_destroy(((COMPS_DocEnv*)env)->properties);
    comps_list_destroy(&((COMPS_DocEnv*)env)->group_list);
    comps_list_destroy(&((COMPS_DocEnv*)env)->option_list);
    comps_dict_destroy(((COMPS_DocEnv*)env)->desc_by_lang);
    comps_dict_destroy(((COMPS_DocEnv*)env)->name_by_lang);
    free((COMPS_DocEnv*)env);
}

/**
 * Set environment id. If copy param is 1, copy passed id buffer to new
 * buffer and set id to newly created buffer. If copy param is 0, only
 * reassign buffer pointer
 * \warning Destroying environment or another comps_docenv_set_id call
 * \warning free current environment id. Passing copy=0 only prevents from
 * \warning double copying
 * @param env COMPS_DocEnvironment object
 * @param id new environment id
 * @param copy copy indicator
 */
inline void comps_docenv_set_id(COMPS_DocEnv * env, char *id, char copy) {
    //return 
    if (id)
        __comps_doc_add_prop(env->properties, "id",
                             comps_doc_prop_str_create(id, copy));
    //__comps_doc_char_setter((void**)&env->id, id, copy);
}

/**
 * Set environment name. If copy param is 1, copy passed name buffer to new
 * buffer and set name to newly created buffer. If copy param is 0, only
 * reassign buffer pointer
 * \warning Destroying environment or another comps_docenv_set_name
 * \warning call free current environment name. Passing copy=0 only prevents
 * \warning from double copying
 * @param env COMPS_DocEnvironment object
 * @param name new environment name
 * @param copy copy indicator
 */
inline void comps_docenv_set_name(COMPS_DocEnv * env, char *name, char copy) {
    //return 
    if (name)
        __comps_doc_add_prop(env->properties, "name",
                             comps_doc_prop_str_create(name, copy));
    //__comps_doc_char_setter((void**)&env->name, name, copy);
}
inline void comps_docenv_add_lang_name(COMPS_DocEnv *env,
                                       char *lang, char *name, char copy){
    __comps_doc_add_lang_prop(env->name_by_lang, lang, name, copy);
}

/**
 * Set environment description. If copy param is 1, copy passed description
 * buffer to new buffer and set description to newly created buffer.
 * If copy param is 0, only reassign buffer pointer
 * \warning Destroying environment or another comps_docenv_set_desc
 * \warning call free current environment description. Passing copy=0 only
 * \warning prevents from double copying
 * @param env COMPS_DocEnvironment object
 * @param desc new environment description
 * @param copy copy indicator
 */
inline void comps_docenv_set_desc(COMPS_DocEnv * env, char *desc, char copy) {
    //return 
    if (desc)
        __comps_doc_add_prop(env->properties, "desc",
                             comps_doc_prop_str_create(desc, copy));
    //__comps_doc_char_setter((void**)&env->desc, desc, copy);
}
inline void comps_docenv_add_lang_desc(COMPS_DocEnv *env,
                                       char *lang, char *desc, char copy){
    __comps_doc_add_lang_prop(env->desc_by_lang, lang, desc, copy);
}


/**
 * Set display order of COMPS_DocEnv object
 * @param env COMPS_DocEnv object
 * @param display_order new environment display order
 */
inline void comps_docenv_set_displayorder(COMPS_DocEnv *env, int display_order){
    __comps_doc_add_prop(env->properties, "display_order",
                         comps_doc_prop_num_create(display_order));
}

/**
 * Add new groupid to environment. If copy param is 1, copy passed id buffer
 * to new buffer and set description to newly created buffer.
 * If copy param is 0, only reassign buffer pointer
 * \warning Destroying environment free groupids buffers.
 * \warning Passing copy=0 only prevents from double copying
 * @param env COMPS_DocEnvironment object
 * @param id new groupid
 * @param copy copy indicator
 */
void comps_docenv_add_groupid(COMPS_DocEnv *env, char *id, char copy)
{
    COMPS_ListItem *it;
    if (env == NULL) {
        return;
    }
    if (env->group_list == NULL) {
        env->group_list = comps_list_create();
        comps_list_init(env->group_list);
    }
    char *new;
    if (copy && env != NULL) {
        if ((new = malloc(sizeof(char)*(strlen(id)+1))) == NULL) return;
        memcpy(new, id, sizeof(char)*(strlen(id)+1));
    } else new = id;
    it = comps_list_item_create(new, NULL, &free);
    comps_list_append(env->group_list, it);
}

/**
 * Add new optionid to environment. If copy param is 1, copy passed id buffer
 * to new buffer and set description to newly created buffer.
 * If copy param is 0, only reassign buffer pointer
 * \warning Destroying environment free optionids buffers.
 * \warning Passing copy=0 only prevents from double copying
 * @param env COMPS_DocEnvironment object
 * @param id new optionid
 * @param copy copy indicator
 */
void comps_docenv_add_optionid(COMPS_DocEnv *env, char *id, char copy)
{
    COMPS_ListItem *it;
    if (env == NULL) {
        return;
    }
    if (env->option_list == NULL) {
        env->option_list = comps_list_create();
        comps_list_init(env->option_list);
    }
    char *new;
    if (copy) {
        if ((new = malloc(sizeof(char)*(strlen(id)+1))) == NULL) return;
        memcpy(new, id, sizeof(char)*(strlen(id)+1));
    } else new = id;
    it = comps_list_item_create(new, NULL, &free);
    comps_list_append(env->option_list, it);
}

void comps_docenv_xml(COMPS_DocEnv *env, xmlTextWriterPtr writer,
                      COMPS_Logger * log) {
    char *tmp_ch, *id;
    int tmp_num;
    COMPS_ListItem *it;
    COMPS_Prop * tmp_prop;

    xmlTextWriterStartElement(writer, (xmlChar*)"environment");

    tmp_prop = comps_dict_get(env->properties, "id");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        id = tmp_prop->prop.str;
    else id = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_ID]
        && !__comps_doc_write_simple_prop(writer, "id", id)) {
        comps_log_error(log, "id", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }

    tmp_prop = comps_dict_get(env->properties, "name");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        tmp_ch = tmp_prop->prop.str;
    else tmp_ch = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_NAME]
        && !__comps_doc_write_simple_prop(writer, "name", tmp_ch)) {
        comps_log_error(log, "name", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    __comps_doc_write_lang_prop(writer, env->name_by_lang, "name");

    tmp_prop = comps_dict_get(env->properties, "desc");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_STR)
        tmp_ch = tmp_prop->prop.str;
    else tmp_ch = NULL;
    if (comps_xml_elems_required[COMPS_ELEM_DESC]
        && !__comps_doc_write_simple_prop(writer, "description", tmp_ch)) {
        comps_log_error(log, "description", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    __comps_doc_write_lang_prop(writer, env->desc_by_lang, "description");

    tmp_prop = comps_dict_get(env->properties, "display_order");
    if (tmp_prop && tmp_prop->prop_type == COMPS_PROP_NUM)
        tmp_num = tmp_prop->prop.num;
    else tmp_num = 0;
    if (tmp_num) {
        xmlTextWriterStartElement(writer, (xmlChar*)"display_order");
        xmlTextWriterWriteFormatString(writer, "%u", tmp_num);
        xmlTextWriterEndElement(writer);
    }

    if (env->group_list) {
        xmlTextWriterStartElement(writer, (xmlChar*)"grouplist");
        if (!env->group_list->first
           && comps_xml_elems_required[COMPS_ELEM_GROUPID]) {
            comps_log_error(log, "groupid", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        }
        for (it = env->group_list->first; it != NULL; it = it->next) {
            xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
            xmlTextWriterWriteString(writer, (xmlChar*) it->data);
            xmlTextWriterEndElement(writer);
        }
        xmlTextWriterEndElement(writer);
    } else if (comps_xml_elems_required[COMPS_ELEM_GROUPLIST]) {
        comps_log_error(log, "grouplist", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }

    if (env->option_list) {
        xmlTextWriterStartElement(writer, (xmlChar*)"optionlist");
        if (!env->option_list->first
           && comps_xml_elems_required[COMPS_ELEM_GROUPID]) {
            comps_log_error(log, "groupid", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        }
        for (it = env->option_list->first; it != NULL; it = it->next) {
            xmlTextWriterStartElement(writer, (xmlChar*)"groupid");
            xmlTextWriterWriteString(writer, (xmlChar*) it->data);
            xmlTextWriterEndElement(writer);
        }
        xmlTextWriterEndElement(writer);
    } else if (comps_xml_elems_required[COMPS_ELEM_OPTLIST]) {
        comps_log_error(log, "optionlist", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    xmlTextWriterEndElement(writer);
}

/**
 * Make deep copy of environment
 * @param e COMPS_DocEnv object
 * @return new COMPS_DocEnv object
 */
COMPS_DocEnv* comps_docenv_clone(COMPS_DocEnv *e) {
    COMPS_DocEnv *res;
    COMPS_ListItem *it;

    res = comps_docenv_create();
    //comps_docenv_set_name(res, e->name, 1);
    //comps_docenv_set_id(res, e->id, 1);
    //comps_docenv_set_desc(res, e->desc, 1);

    comps_dict_destroy(res->properties);
    comps_dict_destroy(res->name_by_lang);
    comps_dict_destroy(res->desc_by_lang);
    res->properties = comps_dict_clone(e->properties);
    res->name_by_lang = comps_dict_clone(e->name_by_lang);
    res->desc_by_lang = comps_dict_clone(e->desc_by_lang);

    res->group_list = comps_list_create();
    comps_list_init(res->group_list);
    res->option_list = comps_list_create();
    comps_list_init(res->option_list);
    if (e->group_list) {
        for (it = e->group_list->first; it != NULL; it = it->next) {
            comps_docenv_add_groupid(res, (char*) it->data, 1);
        }
    }
    if (e->option_list) {
        for (it = e->option_list->first; it != NULL; it = it->next) {
            comps_docenv_add_optionid(res, (char*) it->data, 1);
        }
    }
    return res;
}

/**
 * Union two environment and return completly new COMPS_DocEnv object
 * of union result.
 * Name, id, description union policy:
 *  if name/id/desc of one environment isn't set, result will have
 *  name/id/decs of other one.
 *  if name/id/desc is set in both environments, result will have
 *  name/id/desc of first environment
 *  otherwise name/ids/desc won't be set in result
 * @param e1 COMPS_DocEnv object
 * @param e2 COMPS_DocEnv object
 * @return new COMPS_DocEnv object of union
 */
COMPS_DocEnv* comps_docenv_union(COMPS_DocEnv *e1, COMPS_DocEnv *e2) {
    COMPS_DocEnv *res;
    COMPS_ListItem *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_HSList *pairs;

    res = comps_docenv_create();
    comps_dict_destroy(res->properties);
    res->properties = comps_dict_clone(e2->properties);
    pairs = comps_dict_pairs(e1->properties);
    for (hsit = pairs->first; hsit != NULL; hsit = hsit->next) {
        comps_dict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                comps_doc_prop_clone(((COMPS_RTreePair*)hsit->data)->data));
    }
    comps_hslist_destroy(&pairs);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_strcmp);
    for (it = e1->group_list->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    for (it = e2->group_list->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    res->group_list = comps_list_create();
    comps_list_init(res->group_list);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_groupid(res, (char*)hsit->data, 1);
    }
    comps_set_destroy(&set);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_strcmp);
    if  (e1->option_list)
        for (it = e1->option_list->first; it != NULL; it = it->next)
            comps_set_add(set, it->data);
    if (e2->option_list)
        for (it = e2->option_list->first; it != NULL; it = it->next)
            comps_set_add(set, it->data);
    res->option_list = comps_list_create();
    comps_list_init(res->option_list);
    for (hsit = set->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_optionid(res, (char*)hsit->data, 1);
    }
    comps_set_destroy(&set);
    return res;
}

/**
 * Intersect two environments and return completly new COMPS_DocCategory
 * object of intersection result.
 * Name, id, description intersect policy:
 *  if name/id/desc of one environment isn't set, result will have
 *  name/id/decs of other one.
 *  if name/id/desc is set in both environments, result will have
 *  name/id/desc of first environment
 *  otherwise name/ids/desc won't be set in result
 * @param e1 COMPS_DocEnv object
 * @param e2 COMPS_DocEnv object
 * @return new COMPS_DocEnv object of intersection
 */
COMPS_DocEnv* comps_docenv_intersect(COMPS_DocEnv *e1, COMPS_DocEnv *e2) {
    COMPS_DocEnv *res;
    COMPS_ListItem *it;
    COMPS_HSListItem *hsit;
    COMPS_Set *set, *set2;
    COMPS_HSList *pairs1, *pairs2;

    res = comps_docenv_create();
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_prop_pair_cmp);

    pairs1 = comps_dict_pairs(e1->properties);
    for (hsit = pairs1->first; hsit != NULL; hsit = hsit->next) {
        comps_set_add(set, hsit->data);
    }
    pairs2 = comps_dict_pairs(e2->properties);
    for (hsit = pairs2->first; hsit != NULL; hsit = hsit->next) {
        if (comps_set_in(set, hsit->data)) {
            comps_dict_set(res->properties, ((COMPS_RTreePair*)hsit->data)->key,
                  comps_doc_prop_clone_v(((COMPS_RTreePair*)hsit->data)->data));
        }
    }
    comps_hslist_destroy(&pairs1);
    comps_hslist_destroy(&pairs2);
    comps_set_destroy(&set);

    /*if (e1->name == NULL) {
        comps_docenv_set_name(res, e2->name, 1);
    } else if (e2->name == NULL) {
        comps_docenv_set_name(res, e1->name, 1);
    } else
        comps_docenv_set_name(res, e1->name, 1);

    if (e1->id == NULL) {
        comps_docenv_set_id(res, e2->id, 1);
    } else if (e2->id == NULL) {
        comps_docenv_set_id(res, e1->id, 1);
    } else
        comps_docenv_set_id(res, e1->id, 1);

    if (e1->desc == NULL) {
        comps_docenv_set_desc(res, e2->desc, 1);
    } else if (e2->desc == NULL) {
        comps_docenv_set_desc(res, e1->desc, 1);
    } else
        comps_docenv_set_desc(res, e1->desc, 1);
    */

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_strcmp);
    set2 = comps_set_create();
    comps_set_init(set2, NULL, NULL, NULL, &__comps_strcmp);

    for (it = e1->group_list->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    for (it = e2->group_list->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data))
            comps_set_add(set2, it->data);
    }
    res->group_list = comps_list_create();
    comps_list_init(res->group_list);
    for (hsit = set2->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_groupid(res, (char*)hsit->data, 1);
    }
    comps_set_destroy(&set);
    comps_set_destroy(&set2);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_strcmp);
    set2 = comps_set_create();
    comps_set_init(set2, NULL, NULL, NULL, &__comps_strcmp);

    for (it = e1->option_list->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    for (it = e2->option_list->first; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data))
            comps_set_add(set2, it->data);
    }
    res->option_list = comps_list_create();
    comps_list_init(res->option_list);
    for (hsit = set2->data->first; hsit!= NULL; hsit = hsit->next) {
        comps_docenv_add_optionid(res, (char*)hsit->data, 1);
    }
    comps_set_destroy(&set);
    comps_set_destroy(&set2);
    return res;
}

inline char comps_docenv_cmp(COMPS_DocEnv *e1, COMPS_DocEnv *e2) {
    COMPS_ListItem *it;
    COMPS_Set *set;

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_strcmp);

    it = (e1->group_list)?e1->group_list->first: NULL;
    for (; it!= NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    it = (e2->group_list)?e2->group_list->first: NULL;
    for (; it!= NULL; it = it->next) {
        if (!comps_set_in(set, it->data)) {
                comps_set_destroy(&set);
                return 0;
        }
    }
    comps_set_clear(set);

    it = (e1->option_list)?e1->option_list->first: NULL;
    for (; it!= NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    it = (e2->option_list)?e2->option_list->first: NULL;
    for (; it!= NULL; it = it->next) {
        if (!comps_set_in(set, it->data)) {
            comps_set_destroy(&set);
            return 0;
        }
    }
    comps_set_destroy(&set);
    return comps_props_cmp(e1->properties, e2->properties);
}
inline char comps_docenv_cmp_v(void *e1, void *e2) {
    return comps_docenv_cmp((COMPS_DocEnv*)e1, (COMPS_DocEnv*)e2);
}

/**
 * COMPS_DocGroupPackage constructor
 * @return new COMPS_DocGroupObject object
 */
inline COMPS_DocGroupPackage* comps_docpackage_create()
{
    COMPS_DocGroupPackage * package;
    if ((package = malloc(sizeof(COMPS_DocGroupPackage))) == NULL)
        return NULL;
    package->name = NULL;
    package->type = COMPS_PACKAGE_UNKNOWN;
    return package;
}

/**
 * Deep copy COMPS_DocGroupPackage
 * @param pkg COMPS_DocGroupPackage object
 * @return new COMPS_DocGroupPackage object
 */
COMPS_DocGroupPackage* comps_docpackage_clone(COMPS_DocGroupPackage * pkg) {
    COMPS_DocGroupPackage *newpkg;
    newpkg = comps_docpackage_create();
    if (newpkg == NULL)
        return NULL;
    comps_docpackage_set_name(newpkg, pkg->name, 1);
    newpkg->type = pkg->type;
    return newpkg;
}

/**
 * Set package name. If copy param is 1, copy passed name buffer to new
 * buffer and set name to newly created buffer. If copy param is 0, only
 * reassign buffer pointer
 * \warning Destroying package or another comps_docpackage_set_name
 * \warning call free current package name. Passing copy=0 only prevents
 * 'warning from double copying
 * @param package COMPS_DocGroupPackage object
 * @param name new package name
 * @param copy copy indicator
 */
inline void comps_docpackage_set_name(COMPS_DocGroupPackage *package, char *name,
                               char copy) {
    //return 
    __comps_doc_char_setter((void**)&package->name, name, copy);
}

/**
 * Set COMPS_DocGroupPackage type
 * @param package COMPS_DocGroupPackage object
 * @param type new package type
 */
inline void comps_docpackage_set_type(COMPS_DocGroupPackage *package,
                               COMPS_PackageType type) {
    package->type = type;
}

/**
 * COMPS_DocGroupPackage destructor
 * @param pkg COMPS_DocGroupPackage object
 */
void comps_docpackage_destroy(void *pkg)
{
    if (pkg == NULL)
        return;
    free(((COMPS_DocGroupPackage*)pkg)->name);
    free(pkg);
}

void comps_docpackage_xml(COMPS_DocGroupPackage * pkg, xmlTextWriterPtr writer,
                          COMPS_Logger *log) {
    (void) log;
    if (pkg->type == COMPS_PACKAGE_OPTIONAL
        || pkg->type == COMPS_PACKAGE_MANDATORY
        || pkg->type == COMPS_PACKAGE_DEFAULT
        || pkg->type == COMPS_PACKAGE_CONDITIONAL) {
    xmlTextWriterStartElement(writer, (xmlChar*) "packagereq");
    if (pkg->type == COMPS_PACKAGE_OPTIONAL)
        xmlTextWriterWriteAttribute(writer, (xmlChar*) "type",
                                            (xmlChar*) "optional");
    else if (pkg->type == COMPS_PACKAGE_MANDATORY)
        xmlTextWriterWriteAttribute(writer, (xmlChar*) "type",
                                            (xmlChar*) "mandatory");
    else if (pkg->type == COMPS_PACKAGE_CONDITIONAL)
        xmlTextWriterWriteAttribute(writer, (xmlChar*) "type",
                                            (xmlChar*) "conditional");
    else
        xmlTextWriterWriteAttribute(writer, (xmlChar*) "type",
                                            (xmlChar*) "default");
    xmlTextWriterWriteString(writer, (xmlChar*) pkg->name);
    xmlTextWriterEndElement(writer);
    }
}

/**
 * Compare two COMPS_DocGroupPackage objects
 * @param pkg1 COMPS_DocGroupPackage object
 * @param pkg2 COMPS_DocGroupPackage object
 * @return 1 if packages have same name and type
 *         0 otherwise
 */
char comps_docpackage_cmp(void* pkg1, void *pkg2) {
    if (((COMPS_DocGroupPackage*)pkg1)->type
        != ((COMPS_DocGroupPackage*)pkg2)->type) return 0;
    if (strcmp(((COMPS_DocGroupPackage*)pkg1)->name,
               ((COMPS_DocGroupPackage*)pkg2)->name) != 0) return 0;
    return 1;
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
