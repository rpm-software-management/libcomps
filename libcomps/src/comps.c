#include "comps.h"
#include <stdio.h>
#include <assert.h>

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
    return (strcmp(((COMPS_DocCategory*)cat1)->id,
                   ((COMPS_DocCategory*)cat2)->id)== 0);
}
inline char __comps_docgroup_idcmp(void* g1, void *g2) {
    return (strcmp(((COMPS_DocGroup*)g1)->id, ((COMPS_DocGroup*)g2)->id)== 0);
}
inline char __comps_docenv_idcmp(void* e1, void *e2) {
    return (strcmp(((COMPS_DocEnv*)e1)->id, ((COMPS_DocEnv*)e2)->id)== 0);
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
inline COMPS_List* __comps_get_lobject(COMPS_Doc *doc, const char *obj) {
    return (COMPS_List*)comps_dict_get(doc->lobjects, obj);
}
inline void __comps_set_lobject(COMPS_Doc *doc, const char *obj,
                                                COMPS_List *lobj) {
    comps_dict_set(doc->lobjects, (char*)obj, lobj);
}
inline void __comps_set_dobject(COMPS_Doc *doc, const char *obj,
                                                COMPS_Dict *dobj) {
    comps_dict_set(doc->dobjects, (char*)obj, dobj);
}
inline void __comps_set_mdobject(COMPS_Doc *doc, const char *obj,
                                                COMPS_MDict *mdobj) {
    comps_dict_set(doc->dobjects, (char*)obj, mdobj);
}
inline char __comps_doc_add_lobject(COMPS_Doc *doc, char *parent, void *obj,
                                   void(*obj_destructor)(void*)) {
    if (doc == NULL) return 0;
    COMPS_List *list = (COMPS_List*)comps_dict_get(doc->lobjects, parent);
    if (list == NULL) {
        list = comps_list_create();
        comps_list_init(list);
        comps_dict_set(doc->lobjects, parent, list);
        list = (COMPS_List*)comps_dict_get(doc->lobjects, parent);
    }
    return comps_list_append(list,
                             comps_list_item_create(obj, NULL, obj_destructor));
}
inline char __comps_doc_remove_lobject(COMPS_Doc *doc, char *parent, void *obj){
    if (doc == NULL) return 0;
    COMPS_List *list = (COMPS_List*)comps_dict_get(doc->lobjects, parent);
    if (list == NULL) return 0;
    return comps_list_remove_data(list, obj);
}

inline COMPS_Dict* __comps_get_dobject(COMPS_Doc *doc, const char *obj) {
    return (COMPS_Dict*)comps_dict_get(doc->dobjects, obj);
}
inline char __comps_doc_add_dobject(COMPS_Doc *doc, char *parent, char *key,
                                    char *val) {
    if (doc == NULL) return 0;
    COMPS_Dict *dict = (COMPS_Dict*)comps_dict_get(doc->dobjects, parent);
    if (dict == NULL) {
        dict = comps_dict_create(NULL, &__comps_str_clone, &free);
        comps_dict_set(doc->dobjects, parent, dict);
    }
    if (key != NULL)
        comps_dict_set(dict, key, val);
    return 1;
}

inline COMPS_MDict* __comps_get_mdobject(COMPS_Doc *doc, const char *obj) {
    return (COMPS_MDict*)comps_dict_get(doc->mdobjects, obj);
}
inline char __comps_doc_add_mdobject(COMPS_Doc *doc, char *parent, char *key,
                                    char *val) {
    if (doc == NULL) return 0;
    COMPS_MDict *dict = (COMPS_MDict*)comps_dict_get(doc->mdobjects, parent);
    if (dict == NULL) {
        dict = comps_mdict_create(NULL, &__comps_str_clone, &free);
        comps_dict_set(doc->mdobjects, parent, dict);
    }
    if (key != NULL)
        comps_mdict_set(dict, key, val);
    return 1;
}


inline COMPS_List *comps_doc_groups(COMPS_Doc *doc) {
    return __comps_get_lobject(doc, "groups");
}
inline COMPS_List *comps_doc_categories(COMPS_Doc *doc) {
    return __comps_get_lobject(doc, "categories");
}
inline COMPS_List *comps_doc_environments(COMPS_Doc *doc) {
    return __comps_get_lobject(doc, "envs");
}
inline COMPS_Dict *comps_doc_langpacks(COMPS_Doc *doc) {
    return __comps_get_dobject(doc, "langpacks");
}
inline COMPS_MDict *comps_doc_whiteout(COMPS_Doc *doc) {
    return __comps_get_mdobject(doc, "whiteout");
}
inline COMPS_MDict *comps_doc_blacklist(COMPS_Doc *doc) {
    return __comps_get_mdobject(doc, "blacklist");
}

inline void comps_doc_set_groups(COMPS_Doc *doc, COMPS_List *groups) {
    __comps_set_lobject(doc, "groups", groups);
}
inline void comps_doc_set_cats(COMPS_Doc *doc, COMPS_List *cats) {
    __comps_set_lobject(doc, "categories", cats);
}
inline void comps_doc_set_envs(COMPS_Doc *doc, COMPS_List *envs) {
    __comps_set_lobject(doc, "envs", envs);
}
inline void comps_doc_set_langpacks(COMPS_Doc *doc, COMPS_Dict *langs) {
    __comps_set_dobject(doc, "langpacks", langs);
}
inline void comps_doc_set_whiteout(COMPS_Doc *doc, COMPS_MDict *whiteout) {
    __comps_set_mdobject(doc, "whiteout", whiteout);
}
inline void comps_doc_set_blacklist(COMPS_Doc *doc, COMPS_MDict *blist) {
    __comps_set_mdobject(doc, "blacklist", blist);
}

char* comps_doc_get_langpack(COMPS_Doc *doc, char *name){
    return comps_dict_get(__comps_get_dobject(doc, "langpacks"), name);
}
char comps_doc_add_langpack(COMPS_Doc *doc, char *name, char *install)
{
    return __comps_doc_add_dobject(doc, "langpacks", name, install);
}

COMPS_HSList* comps_doc_get_blacklist(COMPS_Doc *doc, char *name){
    return comps_mdict_get(__comps_get_mdobject(doc, "blacklist"), name);
}
char comps_doc_add_blacklist(COMPS_Doc *doc, char *name, char *arch)
{
    return __comps_doc_add_mdobject(doc, "blacklist", name, arch);
}

COMPS_HSList* comps_doc_get_whiteout(COMPS_Doc *doc, char *name){
    return comps_mdict_get(__comps_get_mdobject(doc, "whiteout"), name);
}
char comps_doc_add_whiteout(COMPS_Doc *doc, char *requires, char *package)
{
        return __comps_doc_add_mdobject(doc, "whiteout", requires, package);
}
/**
 * Append COMPS_DocGroup object to group list of COMPS_Doc object
 * @param doc COMPS_Doc object
 * @param group COMPS_DocGroup object
 * @return 1 if append was sucessfull 0 otherwise
 */
char comps_doc_add_group(COMPS_Doc *doc, COMPS_DocGroup *group)
{
   return __comps_doc_add_lobject(doc, "groups", group, &comps_docgroup_destroy);
}

/**
 * Remove COMPS_DocGroup object from group list of COMPS_Doc object.
 * Group is removed and destroyed
 * @param doc COMPS_Doc object
 * @param group COMPS_DocGroup object
 * @return 1 if remove was sucessfull 0 otherwise
 * @see COMPS_DocGroup
 */
inline char comps_doc_del_group(COMPS_Doc * doc,
                                   COMPS_DocGroup *group) {
    return __comps_doc_remove_lobject(doc, "groups", group);
}

/**
 * Append COMPS_DocEnv object to env list of COMPS_Doc object
 * @param doc COMPS_Doc object
 * @param env COMPS_DocEnv object
 * @return 1 if append was sucessfull 0 otherwise
 * @see COMPS_DocEnv
 */
char comps_doc_add_env(COMPS_Doc *doc, COMPS_DocEnv *env) {
   return __comps_doc_add_lobject(doc, "envs", env, &comps_docenv_destroy);
}

/**
 * Remove COMPS_DocEnv object from env list of COMPS_Doc object
 * enviroment is removed and destroyed
 * @param doc COMPS_Doc object
 * @param env COMPS_DocEnv object
 * @return 1 if remove was sucessfull 0 otherwise
 * @see COMPS_DocEnv
 */
inline char comps_doc_del_env(COMPS_Doc * doc, COMPS_DocEnv *env) {
    return __comps_doc_remove_lobject(doc, "envs", env);
}

/**
 * Append COMPS_DocCategory object to category list of COMPS_Doc object
 * @param doc COMPS_Doc object
 * @param category COMPS_DocCategory object
 * @return 1 if append was sucessfull 0 otherwise
 * @see COMPS_DocCategory
 */
char comps_doc_add_category(COMPS_Doc *doc, COMPS_DocCategory *category)
{
   return __comps_doc_add_lobject(doc, "categories", category,
                                 &comps_doccategory_destroy);
}

/**
 * Remove COMPS_DocCategory object from category list of COMPS_Doc object.
 * Category is removed and destroyed
 * @param doc COMPS_Doc object
 * @param category COMPS_DocCategory object
 * @return 1 if remove was sucessfull 0 otherwise
 * @see COMPS_DocCategory
 */
inline char comps_doc_del_category(COMPS_Doc * doc,
                                   COMPS_DocCategory *category) {
    return __comps_doc_remove_lobject(doc, "categories", category);
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

    ret = comps_list_create();
    comps_list_init(ret);

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = groups->first; it != NULL; it = it->next) {
        matched = 0;
        if ((id != NULL) && strcmp(((COMPS_DocGroup*)it->data)->id, id) == 0)
            matched++;
        if ((name != NULL) && strcmp(((COMPS_DocGroup*)it->data)->name, name) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocGroup*)it->data)->name_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, name)) matched++;
        }
        if ((desc != NULL) && strcmp(((COMPS_DocGroup*)it->data)->desc, desc) == 0)
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

    ret = comps_list_create();
    comps_list_init(ret);

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = categories->first; it != NULL; it = it->next) {
        matched = 0;
        if ((id != NULL) && \
            __comps_strcmp(((COMPS_DocCategory*)it->data)->id, id) == 0)
            matched++;
        if ((name != NULL) && \
            __comps_strcmp(((COMPS_DocCategory*)it->data)->name, name) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocCategory*)it->data)->name_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, name)) matched++;
        }
        if ((desc != NULL) && strcmp(((COMPS_DocCategory*)it->data)->desc, desc) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocCategory*)it->data)->desc_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, name)) matched++;
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

    ret = comps_list_create();
    comps_list_init(ret);

    if (id != NULL) matched_max++;
    if (name != NULL) matched_max++;
    if (desc != NULL) matched_max++;

    for (it = envs->first; it != NULL; it = it->next) {
        matched = 0;
        if ((id != NULL) && strcmp(((COMPS_DocEnv*)it->data)->id, id) == 0)
            matched++;
        if ((name != NULL) && strcmp(((COMPS_DocEnv*)it->data)->name, name) == 0)
            matched++;
        else if (lang != NULL) {
            lang_match = comps_dict_get(
                                ((COMPS_DocEnv*)it->data)->name_by_lang,
                                lang);
            if (__comps_strcmp(lang_match, name)) matched++;
        }
        if ((desc != NULL) && strcmp(((COMPS_DocEnv*)it->data)->desc, desc) == 0)
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
    }
    groups = comps_doc_groups(c2);
    for (it = groups ? groups->first : NULL; it != NULL; it = it->next) {
        if (comps_set_in(set, it->data)) {
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
    group->id = NULL;
    group->name = NULL;
    group->name_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    group->desc = NULL;
    group->desc_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    group->def = 2;
    group->uservisible = 2;
    group->packages = NULL;
    group->lang_only = NULL;
    return group;
}

/**
 * COMPS_DocGroup destructor
 * @param group COMPS_DocGroup object
 */
void comps_docgroup_destroy(void *group)
{
    free(((COMPS_DocGroup*)group)->id);
    free(((COMPS_DocGroup*)group)->name);
    free(((COMPS_DocGroup*)group)->lang_only);
    comps_dict_destroy(((COMPS_DocGroup*)group)->name_by_lang);
    free(((COMPS_DocGroup*)group)->desc);
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
 * @param copy if copy indicator
 */
inline void comps_docgroup_set_id(COMPS_DocGroup *group, char *id, char copy) {
     __comps_doc_char_setter((void**)&group->id, id, copy);
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
    __comps_doc_char_setter((void**)&group->name, name, copy);
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
    __comps_doc_char_setter((void**)&group->desc, desc, copy);
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
    group->def = def;
}

/**
 * Set group uservisible flag
 * @param group COMPS_DocGroup
 * @param uservisible flag
 */
inline void comps_docgroup_set_uservisible(COMPS_DocGroup *group,
                                           unsigned uservisible) {
    group->uservisible = uservisible;
}

/**
 * Set group lang_only flag
 * @param group COMPS_DocGroup
 * @param langonly flag
 */
inline void comps_docgroup_set_langonly(COMPS_DocGroup *group,
                                        char *langonly, char copy) {
    __comps_doc_char_setter((void**)&group->lang_only, langonly, copy);
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
    char *err_msg;
    COMPS_ListItem *it;

    xmlTextWriterStartElement(writer, (xmlChar*)"group");

    if (comps_xml_elems_required[COMPS_ELEM_ID]
        && !__comps_doc_write_simple_prop(writer, "id", group->id)) {
        comps_log_error(log, "<id>", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    if (comps_xml_elems_required[COMPS_ELEM_NAME]
        && !__comps_doc_write_simple_prop(writer, "name", group->name)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<name>") + strlen(group->id)));
        sprintf(err_msg, elem_missing, "<name>", group->id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, group->name_by_lang, "name");
    if (comps_xml_elems_required[COMPS_ELEM_DESC]
        && !__comps_doc_write_simple_prop(writer, "description", group->desc)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<description>") + strlen(group->id)));
        sprintf(err_msg, elem_missing, "<description>", group->id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, group->desc_by_lang, "description");


    xmlTextWriterStartElement(writer, (xmlChar*)"default");
    xmlTextWriterWriteFormatString(writer, "%s",
                                   (group->def)?"true":"false");
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, (xmlChar*)"uservisible");
    xmlTextWriterWriteFormatString(writer, "%s",
                                   (group->uservisible)?"true":"false");
    xmlTextWriterEndElement(writer);

    if (group->lang_only) {
        xmlTextWriterStartElement(writer, (xmlChar*)"langonly");
        xmlTextWriterWriteFormatString(writer, "%s", group->lang_only);
        xmlTextWriterEndElement(writer);
    } else if (comps_xml_elems_required[COMPS_ELEM_LANGONLY]) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<langonly>") + strlen(group->id)));
        sprintf(err_msg, elem_missing, "<langonly>", group->id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    if (group->packages) {
        xmlTextWriterStartElement(writer, (xmlChar*)"packagelist");
        if (!group->packages->first
           && comps_xml_elems_required[COMPS_ELEM_PACKAGEREQ]) {
            err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                             + strlen("<packagereq>") + strlen(group->id)));
            sprintf(err_msg, elem_missing, "<packagereq>", group->id);
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
    comps_docgroup_set_name(res, g->name, 1);
    comps_docgroup_set_id(res, g->id, 1);
    comps_docgroup_set_desc(res, g->desc, 1);
    res->def = g->def;
    res->lang_only = __comps_str_clone(g->lang_only);
    res->uservisible = g->uservisible;

    comps_dict_destroy(res->name_by_lang);
    comps_dict_destroy(res->desc_by_lang);
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

    res = comps_docgroup_create();
    if (g1->name == NULL) {
        comps_docgroup_set_name(res, g2->name, 1);
    } else if (g2->name == NULL) {
        comps_docgroup_set_name(res, g1->name, 1);
    } else
        comps_docgroup_set_name(res, g1->name, 1);

    if (g1->id == NULL) {
        comps_docgroup_set_id(res, g2->id, 1);
    } else if (g2->id == NULL) {
        comps_docgroup_set_id(res, g1->id, 1);
    } else
        comps_docgroup_set_id(res, g1->id, 1);

    if (g1->desc == NULL) {
        comps_docgroup_set_desc(res, g2->desc, 1);
    } else if (g2->desc == NULL) {
        comps_docgroup_set_desc(res, g1->desc, 1);
    } else
        comps_docgroup_set_desc(res, g1->desc, 1);

    if (g1->lang_only == NULL) {
        comps_docgroup_set_langonly(res, g2->lang_only, 1);
    } else if (g2->lang_only == NULL) {
        comps_docgroup_set_langonly(res, g1->lang_only, 1);
    } else
        comps_docgroup_set_langonly(res, g1->lang_only, 1);
    res->uservisible = g1->uservisible;
    res->def = g1->def;

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_docpackage_cmp);
    for (it = g1->packages->first; it != NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    for (it = g2->packages->first; it != NULL; it = it->next) {
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
    COMPS_Set *set;
    COMPS_DocGroupPackage *newpkg;

    res = comps_docgroup_create();
    if (g1->name == NULL) {
        comps_docgroup_set_name(res, g2->name, 1);
    } else if (g2->name == NULL) {
        comps_docgroup_set_name(res, g1->name, 1);
    } else
        comps_docgroup_set_name(res, g1->name, 1);

    if (g1->id == NULL) {
        comps_docgroup_set_name(res, g2->id, 1);
    } else if (g2->id == NULL) {
        comps_docgroup_set_name(res, g1->id, 1);
    } else
        comps_docgroup_set_name(res, g1->id, 1);

    if (g1->desc == NULL) {
        comps_docgroup_set_name(res, g2->desc, 1);
    } else if (g2->desc == NULL) {
        comps_docgroup_set_name(res, g1->desc, 1);
    } else
        comps_docgroup_set_name(res, g1->desc, 1);

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
/**
 * COMPS_DocCategory constructor
 * @return new COMPS_DocCategory object
 */
COMPS_DocCategory* comps_doccategory_create()
{
    COMPS_DocCategory *category;
    if ((category = malloc(sizeof(COMPS_DocCategory))) == NULL)
        return NULL;
    category->id = NULL;
    category->name = NULL;
    category->name_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    category->desc = NULL;
    category->desc_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    category->group_ids = NULL;
    category->display_order = 0;
    return category;
}

/**
 * COMPS_DocCategory destructor
 * @param category COMPS_DocCategory object
 */
void comps_doccategory_destroy(void *category)
{
    if (category == NULL) return;
    free(((COMPS_DocCategory*)category)->id);
    free(((COMPS_DocCategory*)category)->name);
    comps_dict_destroy(((COMPS_DocCategory*)category)->name_by_lang);
    free(((COMPS_DocCategory*)category)->desc);
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
    __comps_doc_char_setter((void**)&category->id, id, copy);
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
    __comps_doc_char_setter((void**)&category->name, name, copy);
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
    __comps_doc_char_setter((void**)&category->desc, desc, copy);
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
    category->display_order = display_order;
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
    char *err_msg;
    static char *elem_missing = "%s at category with id=%s";

    COMPS_ListItem *it;
    xmlTextWriterStartElement(writer, (xmlChar*)"category");
    if (comps_xml_elems_required[COMPS_ELEM_ID]
        && !__comps_doc_write_simple_prop(writer, "id", cat->id)) {
        comps_log_error(log, "<id>", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    if (comps_xml_elems_required[COMPS_ELEM_NAME]
        && !__comps_doc_write_simple_prop(writer, "name", cat->name)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<name>") + strlen(cat->id)));
        sprintf(err_msg, elem_missing, "<name>", cat->id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, cat->name_by_lang, "name");
    if (comps_xml_elems_required[COMPS_ELEM_DESC]
        && !__comps_doc_write_simple_prop(writer, "description", cat->desc)) {
        err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<description>") + strlen(cat->id)));
        sprintf(err_msg, elem_missing, "<description>", cat->id);
        comps_log_error(log, err_msg, COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
        free(err_msg);
    }
    __comps_doc_write_lang_prop(writer, cat->desc_by_lang, "description");

    if (cat->display_order) {
        xmlTextWriterStartElement(writer, (xmlChar*)"display_order");
        xmlTextWriterWriteFormatString(writer, "%u", cat->display_order);
        xmlTextWriterEndElement(writer);
    }
    if (cat->group_ids) {
        xmlTextWriterStartElement(writer, (xmlChar*)"grouplist");
        if (!cat->group_ids->first
           && comps_xml_elems_required[COMPS_ELEM_GROUPID]) {
            err_msg = malloc(sizeof(char)*((strlen(elem_missing)-2)
                         + strlen("<groupid>") + strlen(cat->id)));
            sprintf(err_msg, elem_missing, "<groupid>", cat->id);
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
                     + strlen("<grouplist>") + strlen(cat->id)));
        sprintf(err_msg, elem_missing, "<grouplist>", cat->id);
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
    comps_doccategory_set_name(res, c->name, 1);
    comps_doccategory_set_id(res, c->id, 1);
    comps_doccategory_set_desc(res, c->desc, 1);
    res->display_order = c->display_order;

    comps_dict_destroy(res->name_by_lang);
    comps_dict_destroy(res->desc_by_lang);
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

    res = comps_doccategory_create();
    if (c1->name == NULL || c1->name[0] == 0) {
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
    COMPS_Set *set;

    res = comps_doccategory_create();
    if (c1->name == NULL) {
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

/**
 * COMPS_DocEnv constructor
 * @return new COMPS_DocEnv object
 */
COMPS_DocEnv *comps_docenv_create() {
    COMPS_DocEnv *env;
    if ((env = malloc(sizeof(COMPS_DocEnv))) == NULL)
        return NULL;
    env->id = NULL;
    env->name = NULL;
    env->name_by_lang = comps_dict_create(NULL, &__comps_str_clone, &free);
    env->desc = NULL;
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
    comps_list_destroy(&((COMPS_DocEnv*)env)->group_list);
    comps_list_destroy(&((COMPS_DocEnv*)env)->option_list);
    free(((COMPS_DocEnv*)env)->id);
    free(((COMPS_DocEnv*)env)->desc);
    comps_dict_destroy(((COMPS_DocEnv*)env)->desc_by_lang);
    free(((COMPS_DocEnv*)env)->name);
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
    __comps_doc_char_setter((void**)&env->id, id, copy);
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
    __comps_doc_char_setter((void**)&env->name, name, copy);
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
    __comps_doc_char_setter((void**)&env->desc, desc, copy);
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
    env->display_order = display_order;
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
    COMPS_ListItem *it;
    xmlTextWriterStartElement(writer, (xmlChar*)"environment");
    if (comps_xml_elems_required[COMPS_ELEM_ID]
        && !__comps_doc_write_simple_prop(writer, "id", env->id)) {
        comps_log_error(log, "id", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    if (comps_xml_elems_required[COMPS_ELEM_NAME]
        && !__comps_doc_write_simple_prop(writer, "name", env->name)) {
        comps_log_error(log, "name", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    __comps_doc_write_lang_prop(writer, env->name_by_lang, "name");

    if (comps_xml_elems_required[COMPS_ELEM_DESC]
        && !__comps_doc_write_simple_prop(writer, "description", env->desc)) {
        comps_log_error(log, "description", COMPS_ERR_ELEM_REQUIRED, 0, 0, 0);
    }
    __comps_doc_write_lang_prop(writer, env->desc_by_lang, "description");

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
    comps_docenv_set_name(res, e->name, 1);
    comps_docenv_set_id(res, e->id, 1);
    comps_docenv_set_desc(res, e->desc, 1);

    comps_dict_destroy(res->name_by_lang);
    comps_dict_destroy(res->desc_by_lang);
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

    res = comps_docenv_create();
    if (e1->name == NULL && e2->name == NULL) {
        res->name = NULL;
    } else if (e1->name == NULL) {
        comps_docenv_set_name(res, e2->name, 1);
    } else if (e2->name == NULL) {
        comps_docenv_set_name(res, e1->name, 1);
    } else
        comps_docenv_set_name(res, e1->name, 1);

    if (e1->id == NULL && e2->id == NULL) {
        res->id = NULL;
    } else if (e1->id == NULL) {
        comps_docenv_set_id(res, e2->id, 1);
    } else if (e2->id == NULL) {
        comps_docenv_set_id(res, e1->id, 1);
    } else
        comps_docenv_set_id(res, e1->id, 1);

    if (e1->desc == NULL && e2->desc == NULL) {
        res->desc = NULL;
    } else if (e1->desc == NULL) {
        comps_docenv_set_desc(res, e2->desc, 1);
    } else if (e2->desc == NULL) {
        comps_docenv_set_desc(res, e1->desc, 1);
    } else
        comps_docenv_set_desc(res, e1->desc, 1);
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

    res = comps_docenv_create();
    if (e1->name == NULL) {
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
