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

#ifndef COMPS_DICT_H
#define COMPS_DICT_H

#include "comps_radix.h"
#include "comps_mradix.h"

typedef COMPS_RTree COMPS_Dict;
typedef COMPS_MRTree COMPS_MDict;

COMPS_Dict* comps_dict_create(void* (*data_constructor)(void*),
                                     void* (*data_cloner)(void*),
                                     void (*data_destructor)(void*));
COMPS_MDict* comps_mdict_create(void* (*data_constructor)(void*),
                                       void* (*data_cloner)(void*),
                                       void (*data_destructor)(void*));

void comps_dict_destroy(COMPS_Dict *rt);
void comps_dict_destroy_v(void *rt);
void comps_mdict_destroy(COMPS_MDict *rt);
void comps_mdict_destroy_v(void *rt);

void comps_dict_set(COMPS_Dict *rt, char *key, void *data);
void comps_dict_set_n(COMPS_Dict *rt, char *key, unsigned int len, void *data);
void comps_mdict_set(COMPS_MDict *rt, char *key, void *data);
void comps_mdict_set_n(COMPS_MDict *rt, char *key, unsigned int len,
                            void *data);

void* comps_dict_get(COMPS_Dict *rt, const char *key);
COMPS_HSList * comps_mdict_get(COMPS_MDict *rt, const char *key);
COMPS_HSList ** comps_mdict_getp(COMPS_MDict *rt, const char * key);

void comps_dict_unset(COMPS_Dict * rt, const char * key);
void comps_mdict_unset(COMPS_MDict * rt, const char * key);

void comps_dict_clear(COMPS_Dict * rt);
void comps_mdict_clear(COMPS_MDict * rt);

COMPS_HSList * comps_dict_values(COMPS_Dict * rt);

void comps_dict_values_walk(COMPS_RTree *rt, void *udata,
                              void (*walk_f)(void*, void*));
void comps_mdict_values_walk(COMPS_MDict *rt, void *udata,
                              void (*walk_f)(void*, void*));

COMPS_Dict* comps_dict_clone(COMPS_Dict *rt);
void * comps_dict_clone_v(void * rt);

COMPS_MDict* comps_mdict_clone(COMPS_MDict *rt);
void* comps_mdict_clone_v(void *rt);

COMPS_HSList* comps_mdict_keys(COMPS_MDict *rt);
COMPS_HSList* comps_dict_keys(COMPS_Dict *rt);
COMPS_HSList* comps_dict_pairs(COMPS_Dict *rt);
void comps_mdict_unite(COMPS_MDict *d1, COMPS_MDict *d2);
COMPS_Dict* comps_dict_union(COMPS_Dict *d1, COMPS_Dict *d2);

#endif
