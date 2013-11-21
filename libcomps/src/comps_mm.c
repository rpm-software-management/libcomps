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

#include "comps_mm.h"
COMPS_RefC* comps_refc_create(void *obj, void (*destructor)(void*)) {
    COMPS_RefC *refc;
    refc = malloc(sizeof(*refc));
    if (!refc) {
        raise(SIGABRT);
        return NULL;
    }
    refc->ref_count = 0;
    refc->obj = obj;
    refc->destructor = destructor;
    return refc;
}

void comps_refc_destroy(COMPS_RefC *refc) {
    if (!refc->ref_count) {
        if (refc->destructor) refc->destructor(refc->obj);
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
    //COMPS_Check_NULL(refc, )
    refc->ref_count++;
}
