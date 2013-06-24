#ifndef COMPS_RADIX_H
#define COMPS_RADIX_H

#include <stdlib.h>
#include <string.h>
#include "comps_hslist.h"

typedef struct {
    char * key;
    unsigned is_leaf;
    COMPS_HSList * subnodes;
    void * data;
    void (**data_destructor)(void*);
} COMPS_RTreeData;

typedef struct {
    COMPS_HSList *  subnodes;
    void* (*data_constructor)(void*);
    void* (*data_cloner)(void*);
    void (*data_destructor)(void*);
} COMPS_RTree;

typedef struct {
    char * key;
    void * data;
} COMPS_RTreePair;

COMPS_RTreeData * __comps_rtree_data_create(COMPS_RTree *rt, char *key,
                                                   unsigned int keylen,
                                                   void *data);
COMPS_HSList* __comps_rtree_all(COMPS_RTree * rt, char pairorkey);

void comps_rtree_data_destroy(COMPS_RTreeData * rtd);
void comps_rtree_data_destroy_v(void * rtd);

COMPS_RTreeData * comps_rtree_data_create(COMPS_RTree *rt, char * key,
                                          void * data);
COMPS_RTreeData * comps_rtree_data_create_n(COMPS_RTree *rt, char * key,
                                            unsigned int len,
                                            void * data);

COMPS_RTree * comps_rtree_create(void* (*data_constructor)(void*),
                                 void* (*data_cloner)(void*),
                                 void (*data_destructor)(void*));
void comps_rtree_destroy(COMPS_RTree * rt);

void comps_rtree_set(COMPS_RTree *rt, char *key, void *data);
void comps_rtree_set_n(COMPS_RTree *rt, char *key, unsigned int len,
                       void *data);

void* comps_rtree_get(COMPS_RTree * rt, const char * key);
void comps_rtree_unset(COMPS_RTree * rt, const char * key);
void comps_rtree_clear(COMPS_RTree * rt);

void comps_rtree_values_walk(COMPS_RTree *rt, void* udata,
                                               void (*walk_f)(void*, void*));
COMPS_HSList * comps_rtree_values(COMPS_RTree *rt);
COMPS_HSList* comps_rtree_keys(COMPS_RTree * rt);
COMPS_HSList* comps_rtree_pairs(COMPS_RTree * rt);
COMPS_RTree * comps_rtree_clone(COMPS_RTree * rt);

COMPS_RTreePair * comps_rtree_pair_create(char * key, void * data,
                                          void (*data_destructor(void*)));
void comps_rtree_pair_destroy(COMPS_RTreePair * pair);
void comps_rtree_pair_destroy_v(void * pair);

#endif
