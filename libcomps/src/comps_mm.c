#include "h/comps_mm.h"

COMPS_RefC* comps_refc_create(void *obj, void (*destructor)(void*)) {
    COMPS_RefC *refc;
    refc = malloc(sizeof(*refc));
    if (!refc) {
        // TOTO: MALLOC_FAIL
        return NULL;
    }
    refc->ref_count = 0;
    refc->obj = obj;
    refc->destructor = destructor;
    return refc;
}

void comps_refc_destroy(COMPS_RefC *refc) {
    COMPS_Check_NULL(refc, )
    if (!refc->ref_count) {
        refc->destructor(refc->obj);
        free(refc);
    } else {
        refc->ref_count--;
    }
}

inline void comps_refc_destroy_v(void *refc) {
    comps_refc_destroy((COMPS_RefC*)refc);
}

inline void comps_refc_decref(COMPS_RefC *refc) {
    comps_refc_destroy(refc);
}

inline void comps_refc_incref(COMPS_RefC *refc) {
    COMPS_Check_NULL(refc, )
    refc->ref_count++;
}
