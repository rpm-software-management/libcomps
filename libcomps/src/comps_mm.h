#ifndef COMPS_MM_H
#define COMPS_MM_H

#include <stdlib.h>
#include <string.h>


typedef struct {
    size_t ref_count;
    void (*destructor)(void*);
    void *obj;
} COMPS_RefC;

COMPS_RefC* comps_refc_create(void *obj, void (*destructor)(void*));
void comps_refc_destroy(COMPS_RefC *refc);
void comps_refc_destroy_v(void *refc);
void comps_refc_decref(COMPS_RefC *refc);
void comps_refc_incref(COMPS_RefC *refc);

#endif //COMPS_MM_H
