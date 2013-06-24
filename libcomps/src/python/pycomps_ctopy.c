#include "pycomps_ctopy.h"

void ctopy_key_destroy(void *key) {
    free(((PyCOMPS_CtoPyKey*)key)->key);
    free(key);
}

void* ctopy_key_clone(void *key, unsigned int len) {
    PyCOMPS_CtoPyKey * ret;
    ret = malloc(sizeof(PyCOMPS_CtoPyKey));
    ret->key = malloc(sizeof(char)*len);
    ret->key = memcpy(ret->key, ((PyCOMPS_CtoPyKey*)key)->key, sizeof(char)*len);
    ret->keylen = len;
    return ret;
}

void* ctopy_make_key(void *x) {
    PyCOMPS_CtoPyKey *ret;
    unsigned char i;
    ret = malloc(sizeof(PyCOMPS_CtoPyKey));
    ret->key = malloc(sizeof(char)*sizeof(x));
    for (i = 0; i<sizeof(x); i++) {
        ret->key[i] = ((char*)&x)[sizeof(x)-i-1];
    }
    ret->keylen = sizeof(x);
    return ret;
}

void __ctopy_print_key(void *x){
    unsigned char i;
    for (i = 0; i < ((PyCOMPS_CtoPyKey*)x)->keylen; i++) {
        printf("%hhu|", ((PyCOMPS_CtoPyKey*)x)->key[i]);
    }
    printf("\n");
}

unsigned int ctopy_key_cmp(void *key1, void *key2,
                     unsigned int offset1, unsigned int offset2,
                     unsigned int len, char *ended) {
    unsigned int x;
    char locended = 0;
    for (x = 0; x < len; x++) {

        if (((PyCOMPS_CtoPyKey*)key1)->keylen == x+offset1) locended += 1;
        if (((PyCOMPS_CtoPyKey*)key2)->keylen == x+offset2) locended += 2;
        if (locended != 0) break;
        if (((char*)((PyCOMPS_CtoPyKey*)key1)->key)[x+offset1]
         != ((char*)((PyCOMPS_CtoPyKey*)key2)->key)[x+offset2]) break;
    }
    *ended = locended;
    return x+1;
}

void* ctopy_subkey(void *key, unsigned int offset, unsigned int len) {
    PyCOMPS_CtoPyKey * ret;

    ret = malloc(sizeof(PyCOMPS_CtoPyKey));
    ret->key = malloc(sizeof(char)*len);

    memcpy(ret->key, ((PyCOMPS_CtoPyKey*)key)->key+offset, len-offset);
    ret->keylen = len-offset;
    return ret;
}

unsigned int ctopy_key_len(void *key) {
    return ((PyCOMPS_CtoPyKey*)key)->keylen;
}

void* ctopy_key_concat(void *key1, void *key2) {
    PyCOMPS_CtoPyKey * ret;
    unsigned int i, len;
    ret = malloc(sizeof(PyCOMPS_CtoPyKey));
    ret->key = malloc(sizeof(char)*(((PyCOMPS_CtoPyKey*)key1)->keylen +\
                                    ((PyCOMPS_CtoPyKey*)key2)->keylen));

    for (i = 0; i < ((PyCOMPS_CtoPyKey*)key1)->keylen; i++)
        ret->key[i] = ((PyCOMPS_CtoPyKey*)key1)->key[i];
    ret->keylen = i;
    len = i;
    for (i = 0; i < ((PyCOMPS_CtoPyKey*)key2)->keylen; i++)
        ret->key[i+len] = ((PyCOMPS_CtoPyKey*)key2)->key[i];
    ret->keylen += i;
    return ret;
}


PyCOMPS_CtoPy_CItem *ctopy_citem_create(void *data,
                                        void (*data_destructor)(void*)) {
    PyCOMPS_CtoPy_CItem *ret;
    ret = malloc(sizeof(PyCOMPS_CtoPy_CItem));
    ret->ref_count = 0;
    ret->data = data;
    ret->data_destructor = data_destructor;
    return ret;
}

void ctopy_citem_destroy(PyCOMPS_CtoPy_CItem *citem) {
    if (citem->ref_count == 0) {
        if (citem->data) {
            citem->data_destructor(citem->data);
        }
        free(citem);
    } else {
        citem->ref_count --;
    }
}
inline void ctopy_citem_destroy_v(void *citem) {
    ctopy_citem_destroy((PyCOMPS_CtoPy_CItem*)citem);
}
inline void ctopy_citem_incref(void *citem) {
    ((PyCOMPS_CtoPy_CItem*)citem)->ref_count++;
}
inline void ctopy_citem_decref(void *citem) {
    ctopy_citem_destroy_v(citem);
}
inline void ctopy_pitem_decref(void *pitem) {
    ctopy_citem_decref(((PyCOMPS_CtoPy_PItem*)pitem)->citem);
}

