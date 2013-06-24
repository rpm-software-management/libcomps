#include "comps_dict.h"

inline COMPS_Dict* comps_dict_create(void* (*data_constructor)(void*),
                                     void* (*data_cloner)(void*),
                                     void (*data_destructor)(void*)) {
    return (COMPS_RTree*) comps_rtree_create(data_constructor,
                                             data_cloner, data_destructor);
}

inline void comps_dict_destroy(COMPS_Dict *rt) {
    comps_rtree_destroy((COMPS_RTree*)rt);
}
inline void comps_dict_destroy_v(void *rt) {
    comps_rtree_destroy((COMPS_RTree*)rt);
}
inline void comps_dict_set(COMPS_Dict *rt, char *key, void *data) {
    comps_rtree_set((COMPS_RTree*) rt, key, data);
}
inline void comps_dict_set_n(COMPS_Dict *rt, char *key, unsigned int len,
                            void *data) {
    comps_rtree_set_n((COMPS_RTree*) rt, key, len, data);
}

inline void* comps_dict_get(COMPS_Dict *rt, const char *key) {
    return comps_rtree_get((COMPS_RTree*) rt, key);
}
inline void comps_dict_unset(COMPS_Dict * rt, const char * key) {
    comps_rtree_unset((COMPS_RTree*) rt, key);
}
inline void comps_dict_clear(COMPS_Dict * rt) {
    comps_rtree_clear((COMPS_RTree*) rt);
}
inline COMPS_HSList * comps_dict_values(COMPS_Dict * rt) {
    return comps_rtree_values((COMPS_RTree*)rt);
}
inline void comps_dict_values_walk(COMPS_RTree * rt, void* udata,
                              void (*walk_f)(void*, void*)) {
    comps_rtree_values_walk((COMPS_RTree*)rt, udata, walk_f);
}
inline COMPS_Dict * comps_dict_clone(COMPS_Dict * rt) {
    return (COMPS_RTree*) comps_rtree_clone((COMPS_RTree*)rt);
}
inline void * comps_dict_clone_v(void * rt) {
    return (COMPS_RTree*) comps_rtree_clone((COMPS_RTree*)rt);
}
inline COMPS_HSList* comps_dict_keys(COMPS_Dict * rt) {
    return comps_rtree_keys((COMPS_RTree*)rt);
}
inline COMPS_HSList* comps_dict_pairs(COMPS_Dict *rt) {
    return comps_rtree_pairs((COMPS_RTree*)rt);
}


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
