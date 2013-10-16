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

inline void comps_objdict_set_x(COMPS_ObjDict *rt, char *key, COMPS_Object *data){
    comps_objrtree_set_x((COMPS_ObjRTree*) rt, key, data);
}
inline void comps_objdict_set(COMPS_ObjDict *rt, char *key, COMPS_Object *data){
    comps_objrtree_set((COMPS_ObjRTree*) rt, key, data);
}
inline void comps_objdict_set_n(COMPS_ObjDict *rt, char *key, unsigned int len,
                                COMPS_Object *data) {
    comps_objrtree_set_n((COMPS_ObjRTree*) rt, key, len, data);
}
inline COMPS_Object* comps_objdict_get_x(COMPS_ObjRTree * rt, const char * key) {
    return comps_objrtree_get_x((COMPS_ObjRTree*) rt, key);
}
inline COMPS_Object* comps_objdict_get(COMPS_ObjDict *rt, const char *key) {
    return comps_objrtree_get((COMPS_ObjRTree*) rt, key);
}
inline void comps_objdict_unset(COMPS_ObjDict * rt, const char * key) {
    comps_objrtree_unset((COMPS_ObjRTree*) rt, key);
}
inline void comps_objdict_clear(COMPS_ObjDict * rt) {
    comps_objrtree_clear((COMPS_ObjRTree*) rt);
}
inline COMPS_HSList * comps_objdict_values(COMPS_ObjDict * rt) {
    return comps_objrtree_values((COMPS_ObjRTree*)rt);
}
inline void comps_objdict_values_walk(COMPS_ObjRTree * rt, void* udata,
                              void (*walk_f)(void*, COMPS_Object*)) {
    comps_objrtree_values_walk((COMPS_ObjRTree*)rt, udata, walk_f);
}
inline COMPS_ObjDict * comps_objdict_clone(COMPS_ObjDict * rt) {
    return (COMPS_ObjRTree*) comps_objrtree_clone((COMPS_ObjRTree*)rt);
}
inline void * comps_objdict_clone_v(void * rt) {
    return (void*) comps_objrtree_clone((COMPS_ObjRTree*)rt);
}
inline COMPS_HSList* comps_objdict_keys(COMPS_ObjDict * rt) {
    return comps_objrtree_keys((COMPS_ObjRTree*)rt);
}
inline COMPS_HSList* comps_objdict_pairs(COMPS_ObjDict *rt) {
    return comps_objrtree_pairs((COMPS_ObjRTree*)rt);
}
inline COMPS_ObjDict* comps_objdict_union(COMPS_ObjDict *d1, COMPS_ObjDict *d2) {
    return comps_objrtree_union((COMPS_ObjRTree*)d1, (COMPS_ObjRTree*)d2);
}

inline void comps_objmdict_set_x(COMPS_ObjMDict *rt, char *key, COMPS_Object *data){
    comps_objmrtree_set_x((COMPS_ObjMRTree*) rt, key, data);
}
inline void comps_objmdict_set(COMPS_ObjMDict *rt, char *key, COMPS_Object *data) {
    comps_objmrtree_set((COMPS_ObjMRTree*) rt, key, data);
}

inline void comps_objmdict_set_n(COMPS_ObjMDict *rt, char *key, unsigned int len,
                                 COMPS_Object *data) {
    comps_objmrtree_set_n((COMPS_ObjMRTree*) rt, key, len, data);
}

inline COMPS_ObjList* comps_objmdict_get(COMPS_ObjMDict *rt, const char *key) {
    return comps_objmrtree_get((COMPS_ObjMRTree*) rt, key);
}
/*inline COMPS_HSList** comps_objmdict_getp(COMPS_ObjMDict *rt, const char *key) {
    return comps_obj_mrtree_getp((COMPS_ObjMRTree*) rt, key);
}*/
inline void comps_objmdict_unset(COMPS_ObjMDict * rt, const char * key) {
    comps_objmrtree_unset((COMPS_ObjMRTree*) rt, key);
}
inline void comps_objmdict_clear(COMPS_ObjMDict * rt) {
    comps_objmrtree_clear((COMPS_ObjMRTree*) rt);
}
inline void comps_objmdict_values_walk(COMPS_ObjMDict * rt, void* udata,
                              void (*walk_f)(void*, void*)) {
    comps_objmrtree_values_walk((COMPS_ObjMRTree*)rt, udata, walk_f);
}

inline COMPS_ObjMDict* comps_objmdict_clone(COMPS_ObjMDict * rt) {
    return (COMPS_ObjMDict*) comps_objmrtree_clone((COMPS_ObjMRTree*)rt);
}
inline void* comps_objmdict_clone_v(void *rt) {
    return (COMPS_ObjMRTree*) comps_objmrtree_clone((COMPS_ObjMRTree*)rt);
}
inline COMPS_HSList* comps_objmdict_keys(COMPS_ObjMDict * rt) {
    return comps_objmrtree_keys((COMPS_ObjMRTree*)rt);
}
inline void comps_objmdict_unite(COMPS_ObjMDict * d1, COMPS_ObjMDict *d2) {
    comps_objmrtree_unite((COMPS_ObjMRTree*)d1, (COMPS_ObjMRTree*)d2);
}
inline COMPS_HSList* comps_objmdict_pairs(COMPS_ObjMDict *rt) {
    return comps_objmrtree_pairs((COMPS_ObjMRTree*)rt);
}
inline COMPS_HSList * comps_objmdict_values(COMPS_ObjMDict * rt) {
    return comps_objmrtree_values((COMPS_ObjMRTree*)rt);
}

COMPS_ObjectInfo COMPS_ObjMDict_ObjInfo = {
    .obj_size = sizeof(COMPS_ObjMRTree),
    .constructor = &comps_objmrtree_create_u,
    .destructor = &comps_objmrtree_destroy_u,
    .copy = &comps_objmrtree_copy_u,
    .obj_cmp = &comps_objmrtree_cmp_u
};

COMPS_ObjectInfo COMPS_ObjDict_ObjInfo = {
    .obj_size = sizeof(COMPS_ObjRTree),
    .constructor = &comps_objrtree_create_u,
    .destructor = &comps_objrtree_destroy_u,
    .copy = &comps_objrtree_copy_u,
    .obj_cmp = &comps_objrtree_cmp_u
};
