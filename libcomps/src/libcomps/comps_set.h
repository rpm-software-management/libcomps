#ifndef COMPS_Set_H
#define COMPS_Set_H

#include "comps_hslist.h"

typedef struct {
    char (*eqf)(void*, void*);
    void (*data_destructor)(void*);
    void* (*data_cloner)(void*);
    void* (*data_constructor)(void*);
    COMPS_HSList *data;
} COMPS_Set;

void* comps_set_index_clone(void *item);
COMPS_Set * comps_set_create();
void comps_set_destroy(COMPS_Set **set);
void comps_set_destroy_v(void *set);
void comps_set_init(COMPS_Set *set,  void* (*data_constructor)(void*),
                                     void* (*data_cloner)(void*),
                                     void (*data_destructor)(void*),
                                     char (*eqf)(void*, void*));

char comps_set_in(COMPS_Set *set, void *item);
void comps_set_add(COMPS_Set *set, void *item);
void* comps_set_remove(COMPS_Set *set, void *item);
char comps_set_is_empty(COMPS_Set *set);
char comps_set_cmp(COMPS_Set *set1, COMPS_Set *set2);
int comps_set_at(COMPS_Set *set, void *item);
void* comps_set_data_at(COMPS_Set * set, void * item);
void comps_set_clear(COMPS_Set *set);

#endif
