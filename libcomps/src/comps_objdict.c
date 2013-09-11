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

#include "comps_objdict.h"

inline COMPS_ObjDict* comps_objdict_create(void* (*data_constructor)(void*),
                                     void* (*data_cloner)(void*),
                                     void (*data_destructor)(void*)) {
    return (COMPS_ObjRTree*) comps_rtree_create(data_constructor,
                                             data_cloner, data_destructor);
}

inline void comps_objdict_destroy(COMPS_ObjDict *rt) {
    comps_rtree_destroy((COMPS_ObjRTree*)rt);
}
inline void comps_objdict_destroy_v(void *rt) {
    comps_rtree_destroy((COMPS_ObjRTree*)rt);
}
inline void comps_objdict_set(COMPS_ObjDict *rt, char *key, void *data) {
    comps_rtree_set((COMPS_ObjRTree*) rt, key, data);
}
inline void comps_objdict_set_n(COMPS_ObjDict *rt, char *key, unsigned int len,
                            void *data) {
    comps_rtree_set_n((COMPS_ObjRTree*) rt, key, len, data);
}

inline void* comps_objdict_get(COMPS_ObjDict *rt, const char *key) {
    return comps_rtree_get((COMPS_ObjRTree*) rt, key);
}
inline void comps_objdict_unset(COMPS_ObjDict * rt, const char * key) {
    comps_rtree_unset((COMPS_ObjRTree*) rt, key);
}
inline void comps_objdict_clear(COMPS_ObjDict * rt) {
    comps_rtree_clear((COMPS_ObjRTree*) rt);
}
inline COMPS_HSList * comps_objdict_values(COMPS_ObjDict * rt) {
    return comps_rtree_values((COMPS_ObjRTree*)rt);
}
inline void comps_objdict_values_walk(COMPS_ObjRTree * rt, void* udata,
                              void (*walk_f)(void*, void*)) {
    comps_rtree_values_walk((COMPS_ObjRTree*)rt, udata, walk_f);
}
inline COMPS_ObjDict * comps_objdict_clone(COMPS_ObjDict * rt) {
    return (COMPS_ObjRTree*) comps_rtree_clone((COMPS_ObjRTree*)rt);
}
inline void * comps_objdict_clone_v(void * rt) {
    return (COMPS_ObjRTree*) comps_rtree_clone((COMPS_ObjRTree*)rt);
}
inline COMPS_HSList* comps_objdict_keys(COMPS_ObjDict * rt) {
    return comps_rtree_keys((COMPS_ObjRTree*)rt);
}
inline COMPS_HSList* comps_objdict_pairs(COMPS_ObjDict *rt) {
    return comps_rtree_pairs((COMPS_ObjRTree*)rt);
}
inline COMPS_ObjDict* comps_objdict_union(COMPS_ObjDict *d1, COMPS_ObjDict *d2) {
    return comps_rtree_union((COMPS_ObjRTree*)d1, (COMPS_ObjRTree*)d2);
}

/*
inline COMPS_MDict* comps_mdict_create(void* (*data_constructor)(void*),
                                       void* (*data_cloner)(void*),
                                       void (*data_destructor)(void*)) {
    return (COMPS_MRTree*) comps_mrtree_create(data_constructor,
                                               data_cloner, data_destructor);
}

inline void comps_mdict_destroy(COMPS_MDict *rt) {
    comps_mrtree_destroy((COMPS_MRTree*)rt);
}
inline void comps_mdict_destroy_v(void *rt) {
    comps_mrtree_destroy((COMPS_MRTree*)rt);
}

inline void comps_mdict_set(COMPS_MDict *rt, char *key, void *data) {
    comps_mrtree_set((COMPS_MRTree*) rt, key, data);
}

inline void comps_mdict_set_n(COMPS_MDict *rt, char *key, unsigned int len,
                            void *data) {
    comps_mrtree_set_n((COMPS_MRTree*) rt, key, len, data);
}

inline COMPS_HSList* comps_mdict_get(COMPS_MDict *rt, const char *key) {
    return comps_mrtree_get((COMPS_MRTree*) rt, key);
}
inline COMPS_HSList** comps_mdict_getp(COMPS_MDict *rt, const char *key) {
    return comps_mrtree_getp((COMPS_MRTree*) rt, key);
}
inline void comps_mdict_unset(COMPS_MDict * rt, const char * key) {
    comps_mrtree_unset((COMPS_MRTree*) rt, key);
}
inline void comps_mdict_clear(COMPS_MDict * rt) {
    comps_mrtree_clear((COMPS_MRTree*) rt);
}
inline void comps_mdict_values_walk(COMPS_MDict * rt, void* udata,
                              void (*walk_f)(void*, void*)) {
    comps_mrtree_values_walk((COMPS_MRTree*)rt, udata, walk_f);
}

inline COMPS_MDict* comps_mdict_clone(COMPS_MDict * rt) {
    return (COMPS_MRTree*) comps_mrtree_clone((COMPS_MRTree*)rt);
}
inline void* comps_mdict_clone_v(void *rt) {
    return (COMPS_MRTree*) comps_mrtree_clone((COMPS_MRTree*)rt);
}
inline COMPS_HSList* comps_mdict_keys(COMPS_MDict * rt) {
    return comps_mrtree_keys((COMPS_MRTree*)rt);
}
inline void comps_mdict_unite(COMPS_MDict * d1, COMPS_MDict *d2) {
    comps_mrtree_unite((COMPS_MRTree*)d1, (COMPS_MRTree*)d2);
}
*/

COMPS_ObjectInfo COMPS_ObjDict_ObjInfo = COMPS_ObjRTree_ObjInfo;
