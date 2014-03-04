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

#ifndef COMPS_MRADIX_H
#define COMPS_MRADIX_H

#include <stdlib.h>
#include <string.h>
#include "comps_hslist.h"

typedef struct {
    char * key;
    unsigned is_leaf;
    COMPS_HSList * subnodes;
    COMPS_HSList * data;
} COMPS_MRTreeData;

typedef struct {
    COMPS_HSList *  subnodes;
    void* (*data_constructor)(void*);
    void* (*data_cloner)(void*);
    void (*data_destructor)(void*);
} COMPS_MRTree;

void comps_mrtree_data_destroy(COMPS_MRTreeData * rtd);
void comps_mrtree_data_destroy_v(void * rtd);
COMPS_MRTreeData * comps_mrtree_data_create(COMPS_MRTree* tree,
                                            char * key, void * data);
COMPS_MRTreeData * comps_mrtree_data_create_n(COMPS_MRTree * tree, char * key,
                                              size_t keylen, void * data);

COMPS_MRTree * comps_mrtree_create(void* (*data_constructor)(void*),
                                   void* (*data_cloner)(void*),
                                   void (*data_destructor)(void*));
void comps_mrtree_destroy(COMPS_MRTree *rt);

void comps_mrtree_set(COMPS_MRTree *rt, char *key, void *data);
void comps_mrtree_set_n(COMPS_MRTree * rt, char * key, size_t len, void * data);

COMPS_HSList* comps_mrtree_get(COMPS_MRTree *rt, const char *key);
COMPS_HSList** comps_mrtree_getp(COMPS_MRTree *rt, const char *key);

void comps_mrtree_unset(COMPS_MRTree *rt, const char *key);
void comps_mrtree_clear(COMPS_MRTree *rt);

void comps_mrtree_values_walk(COMPS_MRTree *rt, void *udata,
                              void (*walk_f)(void*, void*));
COMPS_MRTree * comps_mrtree_clone(COMPS_MRTree *rt);
COMPS_HSList* comps_mrtree_keys(COMPS_MRTree *rt);
void comps_mrtree_unite(COMPS_MRTree *rt1, COMPS_MRTree *rt2);

#endif
