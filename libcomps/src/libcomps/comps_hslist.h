#include "comps_set.h"

#ifndef COMPS_HSLIST_H
#define COMPS_HSLIST_H


struct _COMPS_HSListItem {
    struct _COMPS_HSListItem * next;
    void * data;
};

typedef struct _COMPS_HSListItem COMPS_HSListItem;

typedef struct {
    COMPS_HSListItem * first;
    COMPS_HSListItem * last;
    void(*data_destructor)(void*);
    void*(*data_cloner)(void*);
    void*(*data_constructor)(void*);
} COMPS_HSList;

COMPS_HSList * comps_hslist_create();
void comps_hslist_destroy(COMPS_HSList ** hlist);
void comps_hslist_destroy_v(void ** hlist);

void comps_hslist_init(COMPS_HSList * hlist,
                       void*(*data_constructor)(void* data),
                       void*(*data_cloner)(void* data),
                       void(*data_destructor)(void* data));
void comps_hslist_append(COMPS_HSList * hlist, void * data,
                                                        unsigned construct);
void comps_hslist_remove(COMPS_HSList * hlist, COMPS_HSListItem * it);
void* comps_hslist_data_at(COMPS_HSList * hlist, unsigned int index);

COMPS_HSList* comps_hslist_clone(COMPS_HSList * hslist);
void comps_hslist_clear(COMPS_HSList * hslist);
unsigned comps_hslist_values_equal(COMPS_HSList *hlist1, COMPS_HSList *hlist2,
                                   char (*cmpf)(void*, void*));
void comps_hslist_unique(COMPS_HSList *hslist1, char (*cmpf)(void*, void*));

#endif
