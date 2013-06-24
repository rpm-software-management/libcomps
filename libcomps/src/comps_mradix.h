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

COMPS_MRTree * comps_mrtree_create(void* (*data_constructor)(void*),
                                   void* (*data_cloner)(void*),
                                   void (*data_destructor)(void*));
void comps_mrtree_destroy(COMPS_MRTree *rt);

void comps_mrtree_set(COMPS_MRTree *rt, char *key, void *data);
void comps_mrtree_set_n(COMPS_MRTree *rt, char *key, unsigned int len,
                        void *data);

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
