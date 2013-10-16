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

#ifndef COMPS_OBJDICT_H
#define COMPS_OBJDICT_H

#include "comps_objradix.h"
#include "comps_objmradix.h"

typedef COMPS_ObjRTree COMPS_ObjDict;
//typedef COMPS_ObjRTree_ObjInfo COMPS_ObjDict_ObjInfo;
typedef COMPS_ObjMRTree COMPS_ObjMDict;

COMPS_ObjDict* comps_objdict_create();
COMPS_ObjMDict* comps_objmdict_create();


void comps_objdict_destroy(COMPS_ObjDict *rt);
void comps_objdict_destroy_v(void *rt);
void comps_objmdict_destroy(COMPS_ObjMDict *rt);
void comps_objmdict_destroy_v(void *rt);

void comps_objdict_set(COMPS_ObjDict *rt, char *key, COMPS_Object *data);
void comps_objdict_set_x(COMPS_ObjDict *rt, char *key, COMPS_Object *data);
void comps_objdict_set_n(COMPS_ObjDict *rt, char *key, unsigned int len,
                         COMPS_Object *data);
void comps_objmdict_set(COMPS_ObjMDict *rt, char *key, COMPS_Object *data);
void comps_objmdict_set_x(COMPS_ObjMDict *rt, char *key, COMPS_Object *data);
void comps_objmdict_set_n(COMPS_ObjMDict *rt, char *key, unsigned int len,
                            COMPS_Object *data);

COMPS_Object* comps_objdict_get(COMPS_ObjDict *rt, const char *key);
COMPS_Object* comps_objdict_get_x(COMPS_ObjRTree * rt, const char * key);
COMPS_ObjList * comps_objmdict_get(COMPS_ObjMDict *rt, const char *key);

void comps_objdict_unset(COMPS_ObjDict * rt, const char * key);
void comps_objmdict_unset(COMPS_ObjMDict * rt, const char * key);

void comps_objdict_clear(COMPS_ObjDict * rt);
void comps_objmdict_clear(COMPS_ObjMDict * rt);

COMPS_HSList * comps_objdict_values(COMPS_ObjDict * rt);
COMPS_HSList * comps_objmdict_values(COMPS_ObjMDict * rt);

void comps_objdict_values_walk(COMPS_ObjRTree * rt, void* udata,
                              void (*walk_f)(void*, COMPS_Object*));
void comps_objmdict_values_walk(COMPS_ObjMDict *rt, void *udata,
                              void (*walk_f)(void*, void*));

COMPS_ObjDict* comps_objdict_clone(COMPS_ObjDict *rt);
void * comps_objdict_clone_v(void * rt);

COMPS_ObjMDict* comps_objmdict_clone(COMPS_ObjMDict *rt);
void* comps_objmdict_clone_v(void *rt);

COMPS_HSList* comps_objmdict_keys(COMPS_ObjMDict *rt);
COMPS_HSList* comps_objdict_keys(COMPS_ObjDict *rt);
COMPS_HSList* comps_objdict_pairs(COMPS_ObjDict *rt);
COMPS_HSList* comps_objmdict_pairs(COMPS_ObjMDict *rt);
//void comps_mdict_unite(COMPS_MDict *d1, COMPS_MDict *d2);
COMPS_ObjDict* comps_objdict_union(COMPS_ObjDict *d1, COMPS_ObjDict *d2);

extern COMPS_ObjectInfo COMPS_ObjDict_ObjInfo;
extern COMPS_ObjectInfo COMPS_ObjMDict_ObjInfo;

#endif
