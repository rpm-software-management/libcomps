/*! \file comps.h
 * \brief A Documented file.
 * Details.
 * */

#ifndef COMPS_H
#define COMPS_H

#include <stdio.h>
#include <stdarg.h>

#include "comps_doc_types.h"
#include "comps_getset.h"
#include "comps_dict.h"
#include "comps_list.h"
#include "comps_set.h"
#include "comps_logger.h"
#include "comps_hslist.h"
#include "comps_utils.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>

COMPS_Prop * __comps_doccat_get_prop(void *cat, const char *key);
COMPS_Prop * __comps_docenv_get_prop(void *env, const char *key);
COMPS_Prop * __comps_docgroup_get_prop(void *group, const char *key);


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
void __comps_doc_add_prop(COMPS_Dict *dict, char *key, COMPS_Prop * prop);
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
char comps_doc_cmp(COMPS_Doc *c1, COMPS_Doc *c2);

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
void comps_docgroup_set_displayorder(COMPS_DocGroup *group,
                                     unsigned display_order);
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
char comps_docgroup_cmp(COMPS_DocGroup *g1, COMPS_DocGroup *g2);
char comps_docgroup_cmp_v(void *g1, void *g2);


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
char comps_doccategory_cmp(COMPS_DocCategory *c1, COMPS_DocCategory *c2);
char comps_doccategory_cmp_v(void *c1, void *c2);

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
char comps_docenv_cmp(COMPS_DocEnv *e1, COMPS_DocEnv *e2);
char comps_docenv_cmp_v(void *e1, void *e2);

COMPS_DocGroupPackage* comps_docpackage_create();
COMPS_DocGroupPackage* comps_docpackage_clone(COMPS_DocGroupPackage * pkg);
void comps_docpackage_set_name(COMPS_DocGroupPackage *package, char *name,
                               char copy);
void comps_docpackage_set_type(COMPS_DocGroupPackage *package,
                               COMPS_PackageType type);
void comps_docpackage_set_requires(COMPS_DocGroupPackage *package,
                                   char *requires, char copy);

void comps_docpackage_destroy(void *pkg);
void comps_docpackage_xml(COMPS_DocGroupPackage *pkg, xmlTextWriterPtr writer,
                          COMPS_Logger *log);
char comps_docpackage_cmp(void* pkg1, void *pkg2);
const char* comps_docpackage_type_str(COMPS_PackageType type);

#endif
