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

#ifndef COMPS_OBJMRADIX_H
#define COMPS_OBJMRADIX_H

#include <stdlib.h>
#include <string.h>

#include "comps_obj.h"
#include "comps_utils.h"
#include "comps_hslist.h"
#include "comps_objlist.h"

typedef struct {
    char * key;
    unsigned is_leaf;
    COMPS_HSList * subnodes;
    COMPS_ObjList * data;
} COMPS_ObjMRTreeData;

typedef struct {
    COMPS_Object_HEAD;
    COMPS_HSList *  subnodes;
    unsigned int len;
} COMPS_ObjMRTree;

typedef struct {
    char *key;
    COMPS_ObjList *data;
} COMPS_ObjMRTreePair;

void comps_objmrtree_data_destroy_v(void * rtd);
void comps_objmrtree_pair_destroy_v(void * pair);

void comps_objmrtree_data_destroy(COMPS_ObjMRTreeData * rtd);
void comps_objmrtree_data_destroy_v(void * rtd);
COMPS_ObjMRTreeData * comps_objmrtree_data_create(char *key, COMPS_Object *data);

void comps_objmrtree_create_u(COMPS_Object *mrtree, COMPS_Object **args);
void comps_objmrtree_destroy_u(COMPS_Object * rt);
void comps_objmrtree_copy_shallow(COMPS_ObjMRTree *ret, COMPS_ObjMRTree *rt);
void comps_objmrtree_copy_u(COMPS_Object *rt1, COMPS_Object *rt2);
signed char comps_objmrtree_cmp_u(COMPS_Object *ort1, COMPS_Object *ort2);

void __comps_objmrtree_set(COMPS_ObjMRTree *rt, char *key,
                           size_t len, COMPS_Object *ndata);
void comps_objmrtree_set(COMPS_ObjMRTree * rt, char * key, COMPS_Object * ndata);
void comps_objmrtree_set_x(COMPS_ObjMRTree *rt, char *key, COMPS_Object *data);
void comps_objmrtree_set_n(COMPS_ObjMRTree *rt, char *key, size_t len,
                           void *ndata);

COMPS_ObjList * comps_objmrtree_get(COMPS_ObjMRTree * rt, const char * key);

void comps_objmrtree_unset(COMPS_ObjMRTree *rt, const char *key);
void comps_objmrtree_clear(COMPS_ObjMRTree *rt);

COMPS_HSList * comps_objmrtree_values(COMPS_ObjMRTree * rt);
void comps_objmrtree_values_walk(COMPS_ObjMRTree *rt, void *udata,
                              void (*walk_f)(void*, void*));
COMPS_ObjMRTree * comps_objmrtree_clone(COMPS_ObjMRTree *rt);
COMPS_HSList* comps_objmrtree_keys(COMPS_ObjMRTree *rt);
void comps_objmrtree_unite(COMPS_ObjMRTree *rt1, COMPS_ObjMRTree *rt2);

COMPS_HSList* comps_objmrtree_pairs(COMPS_ObjMRTree * rt);

extern COMPS_ObjectInfo COMPS_ObjMRTree_ObjInfo;

#endif
