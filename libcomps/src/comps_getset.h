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

#ifndef COMPS_GETSET_H
#define COMPS_GETSET_H

#include "comps.h"

COMPS_List* __comps_get_lobject(COMPS_Doc *doc, const char *obj);
void __comps_set_lobject(COMPS_Doc *doc, const char *obj, COMPS_List *lobj);
void __comps_set_dobject(COMPS_Doc *doc, const char *obj,COMPS_Dict *dobj);
void __comps_set_mdobject(COMPS_Doc *doc, const char *obj,COMPS_MDict *mdobj);
char __comps_doc_add_lobject(COMPS_Doc *doc, char *parent,
                             void *obj,
                             void(*obj_destructor)(void*));
char __comps_doc_remove_lobject(COMPS_Doc *doc, char *parent, void *obj);
COMPS_Dict* __comps_get_dobject(COMPS_Doc *doc, const char *obj);
char __comps_doc_add_dobject(COMPS_Doc *doc, char *parent, char *key, char *val);
COMPS_MDict* __comps_get_mdobject(COMPS_Doc *doc, const char *obj);
char __comps_doc_add_mdobject(COMPS_Doc *doc, char *parent, char *key, char *val);

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

char comps_doc_add_group(COMPS_Doc *doc, COMPS_DocGroup *group);
char comps_doc_add_env(COMPS_Doc *doc, COMPS_DocEnv *env);
char comps_doc_add_category(COMPS_Doc *doc, COMPS_DocCategory *category);

char comps_doc_del_group(COMPS_Doc *doc, COMPS_DocGroup *group);
char comps_doc_del_env(COMPS_Doc *doc, COMPS_DocEnv *env);
char comps_doc_del_category(COMPS_Doc *doc, COMPS_DocCategory *category);

char* comps_doc_get_langpack(COMPS_Doc *doc, char *name);
COMPS_HSList* comps_doc_get_blacklist(COMPS_Doc *doc, char *name);
COMPS_HSList* comps_doc_get_whiteout(COMPS_Doc *doc, char *name);

char comps_doc_add_blacklist(COMPS_Doc *doc, char *name, char *arch);

void __comps_doc_add_prop(COMPS_Dict *dict, char *key, COMPS_Prop *prop);
COMPS_Prop* comps_doc_prop_str_create(char *str, char copy);
COMPS_Prop* comps_doc_prop_num_create(int num);
COMPS_Prop* comps_doc_prop_clone(COMPS_Prop *prop);
void* comps_doc_prop_clone_v(void *prop);
void comps_doc_prop_destroy(COMPS_Prop *prop);
void comps_doc_prop_destroy_v(void *prop);
char comps_doc_prop_cmp(COMPS_Prop *prop1, COMPS_Prop *prop2);
char comps_doc_prop_cmp_v(void *prop1, void *prop2);


#endif //COMPS_GETSET_H
