#ifndef PYCOMPS_CTOPY
#define PYCOMPS_CTOPY
#include <stdlib.h>
#include <string.h>
#include <Python.h>

#define PyCOMPS_CtoPy_PItem_HEAD PyCOMPS_CtoPy_CItem *citem;
typedef struct PyCOMPS_CtoPy_CItem PyCOMPS_CtoPy_CItem;


typedef struct PyCOMPS_CtoPy_PItem {
    PyObject_HEAD
    PyCOMPS_CtoPy_PItem_HEAD
    void *data_object;
} PyCOMPS_CtoPy_PItem;

struct PyCOMPS_CtoPy_CItem {
    int ref_count;
    void *data;
    void (*data_destructor)(void * data);
};

typedef struct PyCOMPS_CtoPy_StrGetSet {
    size_t member_offset;
    char* (*get_f)(PyObject*, size_t);
    void (*set_f)(PyObject*, size_t, char*);
} PyCOMPS_CtoPy_StrGetSet;

typedef struct CtoPyKey {
    char *key;
    unsigned int keylen;
} PyCOMPS_CtoPyKey;

void __ctopy_print_key(void *x);

void ctopy_key_destroy(void *key);
void* ctopy_key_clone(void *key, unsigned int len);
void* ctopy_make_key(void *x);
unsigned int ctopy_key_cmp(void *key1, void *key2,
                     unsigned int offset1, unsigned int offset2,
                     unsigned int len, char *ended);
void* ctopy_subkey(void *key, unsigned int offset, unsigned int len);
unsigned int ctopy_key_len(void *key);
void* ctopy_key_concat(void *key1, void *key2);

PyCOMPS_CtoPy_CItem *ctopy_citem_create(void *data,
                                        void (*data_destructor)(void*));
void ctopy_citem_destroy(PyCOMPS_CtoPy_CItem *citem);
void ctopy_citem_destroy_v(void *citem);
void ctopy_citem_incref(void *citem);
void ctopy_citem_decref(void *citem);

void ctopy_pitem_decref(void *pitem);

#endif
