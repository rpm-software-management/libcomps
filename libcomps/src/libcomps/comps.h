/*! \file comps.h
 * \brief A Documented file.
 * Details.
 * */

#ifndef COMPS_H
#define COMPS_H

#include <stdio.h>
#include <stdarg.h>

#include "comps_dict.h"
#include "comps_list.h"
#include "comps_set.h"
#include "comps_logger.h"
#include "comps_hslist.h"
#include "comps_utils.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>

/**
 * COMPS Document
 *\par COMPS_Doc creating/destroying
 * @see comps_doc_create
 * @see comps_doc_init
 * @see comps_doc_clear
 * @see comps_doc_destroy
 * \par COMPS_Doc adding content
 * @see comps_doc_add_group
 * @see comps_doc_add_category
 * @see comps_doc_add_env
 * \par COMPS_Doc receiving content
 * @see comps_doc_get_groups
 * @see comps_doc_get_categories
 * @see comps_doc_get_envs
 * \par COMPS_Doc removing content
 * @see comps_doc_del_group
 * @see comps_doc_del_category
 * @see comps_doc_del_env
 * \par COMPS_Doc set operations
 * @see comps_doc_union
 * @see comps_doc_intersect
 * \par COMPS_Doc xml conversion
 * @see comps2xml_f
 * @see comps2xml_str
 */
typedef struct {
    /** List of document groups @see COMPS_DocGroup*/
    COMPS_Dict *lobjects;
    COMPS_Dict *dobjects;
    COMPS_Dict *mdobjects;

    //COMPS_List *groups;
    /** List of document categories @see COMPS_DocCategory*/
    //COMPS_List *categories;
    /** List of document environemnts @see COMPS_DocEnv*/
    //COMPS_List *envs;
    /** Logger for logging xml convert process @see COMPS_Logger*/
    void *reserved;
    COMPS_Logger *log;
    const char *encoding;
    } COMPS_Doc;


/**
 * COMPS Document category
 * \par COMPS_DocCategory create/destroy
 * @see comps_doccategory_create
 * @see comps_doccategory_destroy
 * @see comps_doccategory_clone
 * \par COMPS_DocCategory properties setters
 * @see comps_doccategory_set_id
 * @see comps_doccategory_set_name
 * @see comps_doccategory_set_desc
 * @see comps_doccategory_set_displayorder
 * \par COMPS_DocCategory groupid adding
 * @see comps_doccategory_add_groupid
 * \par COMPS_DocCategory set operations
 * @see comps_doccategory_union
 * @see comps_doccategory_intersect
 */
typedef struct {
    /** category id */
    char *id;
    /** category name*/
    char *name;
    COMPS_Dict *name_by_lang;
    /** category description */
    char *desc;
    COMPS_Dict *desc_by_lang;
    /** category display order */
    unsigned display_order;
    /** category groups ids*/
    COMPS_List *group_ids;
    void *reserved;
} COMPS_DocCategory;

/**
 * COMPS Document Group
 * \par COMPS_DocGroup create/destroy
 * @see comps_docgroup_create
 * @see comps_docgroup_destroy
 * @see comps_docgroup_clone
 * \par COMPS_DocGroup properties setters
 * @see comps_docgroup_set_id
 * @see comps_docgroup_set_name
 * @see comps_docgroup_set_desc
 * @see comps_docgroup_set_default
 * @see comps_docgroup_set_uservisible
 * @see comps_docgroup_set_langonly
 * \par COMPS_DocGroup package management
 * @see comps_docgroup_add_package
 * @see comps_docgroup_get_packages
 * \par COMPS_DocGroup set operations
 * @see comps_docgroup_union
 * @see comps_docgroup_intersect
 */
typedef struct {
    /** group id*/
    char *id;
    /** group name*/
    char *name;
    COMPS_Dict *name_by_lang;
    /** group description*/
    char *desc;
    COMPS_Dict *desc_by_lang;
    /** group default*/
    unsigned def;
    /** group lang only*/
    char *lang_only;
    /**group user visible*/
    unsigned uservisible;
    /** group list of packages. @see COMPS_DocGroupPackage*/
    COMPS_List *packages;
    void *reserved;
} COMPS_DocGroup;

/**
 * COMPS Document Environment
 *\par COMPS_DocEnv create/destroy
 * @see comps_docenv_create
 * @see comps_docenv_clone
 * @see comps_docenv_destroy
 * \par COMPS_DocEnv properties setters
 * @see comps_docenv_set_id
 * @see comps_docenv_set_name
 * @see comps_docenv_set_desc
 * @see comps_docenv_set_displayorder
 * \par COMPS_DocEnv content adding
 * @see comps_docenv_add_optionid
 * @see comps_docenv_add_groupid
 * \par COMPS_DocEnv set operations
 * @see comps_docenv_union
 * @see comps_docenv_intersect
 */
typedef struct {
    /** environment id*/
    char *id;
    /** environment name*/
    char *name;
    COMPS_Dict *name_by_lang;
    /** environment description*/
    char *desc;
    COMPS_Dict *desc_by_lang;
    /** environment display order*/
    unsigned display_order;
    /** environment grouid list*/
    COMPS_List *group_list;
    /** environment optionid list*/
    COMPS_List *option_list;
    void *reserved;
} COMPS_DocEnv;

typedef enum {COMPS_PACKAGE_DEFAULT, COMPS_PACKAGE_OPTIONAL,
              COMPS_PACKAGE_CONDITIONAL, COMPS_PACKAGE_MANDATORY,
              COMPS_PACKAGE_UNKNOWN} COMPS_PackageType;

/**
 * COMPS Document package
 * @see comps_docpackage_cmp
 * \par COMPS_DocGroupPackage create/destroy
 * @see comps_docpackage_create
 * @see comps_docpackage_clone
 * @see comps_docpackage_destroy
 * \par COMPS_DocGroupPackage properties setters
 * @see comps_docpackage_set_name
 * @see comps_docpackage_set_type
 */
typedef struct {
    /** package type @see COMPS_PackageType */
    COMPS_PackageType type;
    /** name of package */
    char *name;
    void *reserved;
} COMPS_DocGroupPackage;

//typedef COMPS_Dict COMPS_LangPack;
//typedef COMPS_Dict COMPS_LangPack;
//typedef COMPS_Dict COMPS_LangPack;


void __comps_doc_char_setter(void **attr, char *val, char copy);
char __comps_doc_add_lobject(COMPS_Doc *doc, char *parent, void *obj,
                                   void(*obj_destructor)(void*));
COMPS_List* __comps_get_lobject(COMPS_Doc *doc, const char *obj);
void __comps_set_lobject(COMPS_Doc *doc, const char *obj, COMPS_List *lobj);
char __comps_doc_remove_lobject(COMPS_Doc *doc, char *parent, void *obj);

COMPS_Dict* __comps_get_dobject(COMPS_Doc *doc, const char *obj);
void __comps_set_dobject(COMPS_Doc *doc, const char *obj, COMPS_Dict *dobj);
char __comps_doc_add_dobject(COMPS_Doc *doc, char *parent, char *key,
                              char *val);

COMPS_MDict* __comps_get_mdobject(COMPS_Doc *doc, const char *obj);
void __comps_set_mdobject(COMPS_Doc *doc, const char *obj, COMPS_MDict *mdobj);
char __comps_doc_add_mdobject(COMPS_Doc *doc, char *parent, char *key,
                              char *val);

void __comps_doc_add_lang_prop(COMPS_Dict *dict, char *lang, char *prop,
                               char copy);
char __comps_doc_write_simple_prop(xmlTextWriterPtr writer, char *prop,
                                   char *val);
void __comps_doc_write_lang_prop(xmlTextWriterPtr writer, COMPS_Dict *lang_dict,
                                 char *prop);


void comps2xml_f(COMPS_Doc * doc, char *filename, char stdoutredirect);
char *comps2xml_str(COMPS_Doc *doc);

COMPS_Doc *comps_doc_create(const char *encoding);
void comps_doc_init(COMPS_Doc *doc);
void comps_doc_clear(COMPS_Doc *doc);
void comps_doc_destroy(COMPS_Doc **doc);
void comps_doc_destroy_v(void *doc);

COMPS_List *comps_doc_groups(COMPS_Doc *doc);
COMPS_List *comps_doc_categories(COMPS_Doc *doc);
COMPS_List *comps_doc_environments(COMPS_Doc *doc);
COMPS_Dict *comps_doc_langpacks(COMPS_Doc *doc);
COMPS_MDict *comps_doc_whiteout(COMPS_Doc *doc);
COMPS_MDict *comps_doc_blacklist(COMPS_Doc *doc);

void comps_doc_set_groups(COMPS_Doc *doc, COMPS_List *groups);
void comps_doc_set_cats(COMPS_Doc *doc, COMPS_List *cats);
void comps_doc_set_envs(COMPS_Doc *doc, COMPS_List *envs);
void comps_doc_set_langpacks(COMPS_Doc *doc, COMPS_Dict *langs);
void comps_doc_set_whiteout(COMPS_Doc *doc, COMPS_MDict *whiteout);
void comps_doc_set_blacklist(COMPS_Doc *doc, COMPS_MDict *blist);

void comps_doc_set_groups_v(void *doc, COMPS_List *groups);
void comps_doc_set_cats_v(void *doc, COMPS_List *cats);
void comps_doc_set_envs_v(void *doc, COMPS_List *envs);
void comps_doc_set_langpacks_v(void *doc, COMPS_Dict *langs);
void comps_doc_set_whiteout_v(void *doc, COMPS_MDict *whiteout);
void comps_doc_set_blacklist_v(void *doc, COMPS_MDict *blist);
COMPS_List *comps_doc_groups_v(void *doc);
COMPS_List *comps_doc_categories_v(void *doc);
COMPS_List *comps_doc_environments_v(void *doc);
COMPS_Dict *comps_doc_langpacks_v(void *doc);
COMPS_MDict *comps_doc_whiteout_v(void *doc);
COMPS_MDict *comps_doc_blacklist_v(void *doc);

char comps_doc_add_group(COMPS_Doc *doc, COMPS_DocGroup *group);
char comps_doc_add_category(COMPS_Doc *doc, COMPS_DocCategory *category);
char comps_doc_add_env(COMPS_Doc *doc, COMPS_DocEnv *env);
char comps_doc_add_langpack(COMPS_Doc *doc, char *name, char *install);
char comps_doc_add_blacklist(COMPS_Doc *doc, char *name, char *arch);
char comps_doc_add_whiteout(COMPS_Doc *doc, char *requires, char *package);

COMPS_List* comps_doc_get_groups(COMPS_Doc *doc, char *id, char *name,
                                     char *desc, char *lang);
COMPS_List* comps_doc_get_categories(COMPS_Doc *doc, char *id, char *name,
                                     char *desc, char *lang);
COMPS_List* comps_doc_get_envs(COMPS_Doc *doc, char *id, char *name,
                                                 char *desc, char *lang);

char comps_doc_del_group(COMPS_Doc *doc, COMPS_DocGroup *group);
char comps_doc_del_category(COMPS_Doc *doc, COMPS_DocCategory *category);
char comps_doc_del_env(COMPS_Doc * doc, COMPS_DocEnv *env);

void comps_doc_xml(COMPS_Doc * doc, xmlTextWriterPtr writer);
COMPS_Doc* comps_doc_union(COMPS_Doc *c1, COMPS_Doc *c2);
COMPS_Doc* comps_doc_intersect(COMPS_Doc *c1, COMPS_Doc *c2);

COMPS_DocGroup* comps_docgroup_create();
void comps_docgroup_destroy(void *group);
void comps_docgroup_set_id(COMPS_DocGroup *group, char *id, char copy);
void comps_docgroup_set_name(COMPS_DocGroup *group, char *name,
                             char copy);
void comps_docgroup_add_lang_name(COMPS_DocGroup *group, char *lang, char *name,
                                  char copy);
void comps_docgroup_set_desc(COMPS_DocGroup *group, char *desc,
                             char copy);
void comps_docgroup_add_lang_desc(COMPS_DocGroup *group, char *lang, char *desc,
                                  char copy);
void comps_docgroup_set_default(COMPS_DocGroup *group, unsigned def);
void comps_docgroup_set_uservisible(COMPS_DocGroup *group,
                                    unsigned uservisible);
void comps_docgroup_set_langonly(COMPS_DocGroup *group, char *langonly,
                                                        char copy);
void comps_docgroup_add_package(COMPS_DocGroup *group,
                                COMPS_DocGroupPackage *package);
COMPS_List* comps_docgroup_get_packages(COMPS_DocGroup *group, char *name,
                                       COMPS_PackageType type);
void comps_docgroup_xml(COMPS_DocGroup *group, xmlTextWriterPtr writer,
                        COMPS_Logger *log);
COMPS_DocGroup* comps_docgroup_union(COMPS_DocGroup *g1, COMPS_DocGroup *g2);
COMPS_DocGroup* comps_docgroup_intersect(COMPS_DocGroup *g1,
                                         COMPS_DocGroup *g2);
COMPS_DocGroup* comps_docgroup_clone(COMPS_DocGroup *g);
char __comps_docgroup_idcmp(void* g1, void *g2);


COMPS_DocCategory* comps_doccategory_create();
void comps_doccategory_destroy(void *category);
void comps_doccategory_set_id(COMPS_DocCategory *category, char *id, char copy);
void comps_doccategory_set_name(COMPS_DocCategory *category, char *name,
                                char copy);
void comps_doccategory_add_lang_name(COMPS_DocCategory *category, char *lang,
                                     char *name, char copy);
void comps_doccategory_set_desc(COMPS_DocCategory *category, char *desc,
                                char copy);
void comps_doccategory_add_lang_desc(COMPS_DocCategory *category, char *lang,
                                     char *desc, char copy);
void comps_doccategory_set_displayorder(COMPS_DocCategory *category,
                                        int display_order);
void comps_doccategory_add_groupid(COMPS_DocCategory *category,
                                   char *id, char copy);
void comps_doccategory_xml(COMPS_DocCategory *cat, xmlTextWriterPtr writer,
                           COMPS_Logger *log);
COMPS_DocCategory* comps_doccategory_union(COMPS_DocCategory *c1,
                                           COMPS_DocCategory *c2);
COMPS_DocCategory* comps_doccategory_intersect(COMPS_DocCategory *c1,
                                         COMPS_DocCategory *c2);
COMPS_DocCategory* comps_doccategory_clone(COMPS_DocCategory *c);
char __comps_doccategory_idcmp(void* cat1, void *cat2);

COMPS_DocEnv *comps_docenv_create();
void comps_docenv_destroy(void *env);
void comps_docenv_set_id(COMPS_DocEnv * env, char *id, char copy);
void comps_docenv_set_name(COMPS_DocEnv * env, char *name, char copy);
void comps_docenv_add_lang_name(COMPS_DocEnv *env, char *lang,
                                char *name, char copy);
void comps_docenv_set_desc(COMPS_DocEnv * env, char *desc, char copy);
void comps_docenv_add_lang_desc(COMPS_DocEnv *env, char *lang,
                                char *desc, char copy);
void comps_docenv_set_displayorder(COMPS_DocEnv *env, int display_order);
void comps_docenv_add_optionid(COMPS_DocEnv *env, char *id, char copy);
void comps_docenv_add_groupid(COMPS_DocEnv *env, char *id, char copy);
void comps_docenv_xml(COMPS_DocEnv *env, xmlTextWriterPtr writer,
                      COMPS_Logger *log);
COMPS_DocEnv* comps_docenv_union(COMPS_DocEnv *e1, COMPS_DocEnv *e2);
COMPS_DocEnv* comps_docenv_intersect(COMPS_DocEnv *e1, COMPS_DocEnv *e2);
COMPS_DocEnv* comps_docenv_clone(COMPS_DocEnv *g);
char __comps_docenv_idcmp(void* e1, void *e2);

COMPS_DocGroupPackage* comps_docpackage_create();
COMPS_DocGroupPackage* comps_docpackage_clone(COMPS_DocGroupPackage * pkg);
void comps_docpackage_set_name(COMPS_DocGroupPackage *package, char *name,
                               char copy);
void comps_docpackage_set_type(COMPS_DocGroupPackage *package,
                               COMPS_PackageType type);

void comps_docpackage_destroy(void *pkg);
void comps_docpackage_xml(COMPS_DocGroupPackage *pkg, xmlTextWriterPtr writer,
                          COMPS_Logger *log);
char comps_docpackage_cmp(void* pkg1, void *pkg2);
const char* comps_docpackage_type_str(COMPS_PackageType type);

#endif
