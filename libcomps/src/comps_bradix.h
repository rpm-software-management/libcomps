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

#ifndef COMPS_BRADIX_H
#define COMPS_BRADIX_H

#include <stdlib.h>
#include <string.h>
#include "comps_hslist.h"

typedef struct {
    void *key;
    unsigned is_leaf;
    COMPS_HSList *subnodes;
    void *data;
    void (*key_destroy)(void *key);
    void (*data_destructor)(void*);
} COMPS_BRTreeData;

typedef struct {
    COMPS_HSList *subnodes;
    void* (*data_constructor)(void*);
    void* (*data_cloner)(void*);
    void (*data_destructor)(void*);

    void* (*key_clone)(void *key, unsigned int len);
    void (*key_destroy)(void *key);
    unsigned int (*key_cmp)(void *key1, void *key2,
                            unsigned int offset1,
                            unsigned int offset2,
                            unsigned int len,
                            char *ended);
    void* (*subkey)(void *key, unsigned int offset, unsigned int len);
    unsigned int (*key_len)(void *key);
    void* (*key_concat)(void*, void*);
} COMPS_BRTree;

typedef struct {
    void *key;
    void *data;
    void (*key_destroy)(void *key);
} COMPS_BRTreePair;

COMPS_BRTreeData * __comps_brtree_data_create(COMPS_BRTree *rt, void *key,
                                                   unsigned int keylen,
                                                   void *data);
COMPS_HSList* __comps_brtree_all(COMPS_BRTree * rt, char pairorkey);

void comps_brtree_data_destroy(COMPS_BRTreeData * rtd);
void comps_brtree_data_destroy_v(void * rtd);

COMPS_BRTreeData * comps_brtree_data_create(COMPS_BRTree *rt, void *key,
                                          void *data);
COMPS_BRTreeData * comps_brtree_data_create_n(COMPS_BRTree *rt, void *key,
                                            unsigned int len,
                                            void *data);

COMPS_BRTree * comps_brtree_create(void* (*data_constructor)(void*),
                                   void* (*data_cloner)(void*),
                                   void (*data_destructor)(void*),
                                   void* (*key_clone)(void*, unsigned int),
                                   void (*key_destroy)(void*),
                                   unsigned int (*key_cmp)(void*, void*,
                                                           unsigned int,
                                                           unsigned int,
                                                           unsigned int, char*),
                                   unsigned int (*key_len)(void*),
                                   void* (*subkey)(void*,
                                                   unsigned int,
                                                   unsigned int),
                                   void* (*key_concat)(void*, void*));

void comps_brtree_destroy(COMPS_BRTree * rt);

void comps_brtree_set(COMPS_BRTree *rt, void *key, void *data);
void comps_brtree_set_n(COMPS_BRTree *rt, void *key, unsigned int len,
                       void *data);

void* comps_brtree_get(COMPS_BRTree * rt, void * key);
void** comps_brtree_getp(COMPS_BRTree *brt, void *key);
void comps_brtree_unset(COMPS_BRTree * rt, void * key);
void comps_brtree_clear(COMPS_BRTree * rt);

void comps_brtree_values_walk(COMPS_BRTree *rt, void* udata,
                                               void (*walk_f)(void*, void*));
COMPS_HSList * comps_brtree_values(COMPS_BRTree *rt);
COMPS_HSList* comps_brtree_keys(COMPS_BRTree * rt);
COMPS_HSList* comps_brtree_pairs(COMPS_BRTree * rt);
COMPS_BRTree * comps_brtree_clone(COMPS_BRTree * rt);

COMPS_BRTreePair * comps_brtree_pair_create(char * key, void * data,
                                          void (*data_destructor(void*)));
void comps_brtree_pair_destroy(COMPS_BRTreePair * pair);
void comps_brtree_pair_destroy_v(void * pair);

COMPS_HSList* __comps_brtree_all(COMPS_BRTree * rt, char pairorkey);

#endif
