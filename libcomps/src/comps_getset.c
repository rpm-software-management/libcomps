#include "comps_getset.h"

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


inline void __comps_doc_add_prop(COMPS_Dict *dict, char *key, COMPS_Prop* prop)
{
        comps_dict_set(dict, key, prop);
}
COMPS_Prop * comps_doc_prop_str_create(char * str, char copy) {
    COMPS_Prop *ret;
    ret = malloc(sizeof(COMPS_Prop));
    ret->prop_type = COMPS_PROP_STR;
    if (copy) {
        ret->prop.str = malloc(sizeof(char) * (strlen(str)+1));
        memcpy(ret->prop.str, str, sizeof(char) * (strlen(str)+1));
    } else {
        ret->prop.str = str;
    }
    return ret;
}

COMPS_Prop * comps_doc_prop_num_create(int num) {
    COMPS_Prop *ret;
    ret = malloc(sizeof(COMPS_Prop));
    ret->prop_type = COMPS_PROP_NUM;
    ret->prop.num = num;
    return ret;
}
COMPS_Prop * comps_doc_prop_clone(COMPS_Prop *prop) {
    COMPS_Prop *ret;
    ret = malloc(sizeof(COMPS_Prop));
    ret->prop_type = prop->prop_type;
    if (prop->prop_type == COMPS_PROP_STR) {
        ret->prop.str = malloc(sizeof(char)* strlen(prop->prop.str));
        memcpy(ret->prop.str, prop->prop.str,
               sizeof(char) * strlen(prop->prop.str));
    } else {
        ret->prop.num = prop->prop.num;
    }
    return ret;
}
inline void* comps_doc_prop_clone_v(void *prop) {
    return (void*)comps_doc_prop_clone(((COMPS_Prop*)prop));
}
void comps_doc_prop_destroy(COMPS_Prop *prop) {
    if (prop->prop_type == COMPS_PROP_STR) {
        free(prop->prop.str);
    }
    free(prop);
}
inline void comps_doc_prop_destroy_v(void *prop) {
    comps_doc_prop_destroy((COMPS_Prop*)prop);
}

char comps_doc_prop_cmp(COMPS_Prop *prop1, COMPS_Prop *prop2) {
    if (prop1->prop_type != prop2->prop_type) {
        return 0;
    }
    if (prop1->prop_type == COMPS_PROP_NUM) {
        if (prop1->prop.num != prop2->prop.num)
            return 0;
    } else {
        if (strcmp(prop1->prop.str, prop2->prop.str) != 0)
            return 0;
    }
    return 1;
}
inline char comps_doc_prop_cmp_v(void *prop1, void *prop2) {
    return comps_doc_prop_cmp((COMPS_Prop*)prop1, (COMPS_Prop*)prop2);
}
