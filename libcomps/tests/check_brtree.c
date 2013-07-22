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

#include "../src/comps_bradix.h"
#include <stdio.h>
#include <string.h>

struct _key {
    char *key;
    unsigned int keylen;
};

struct _teststruct {
    char *testdata;
};

struct _teststruct* make_test_struct(char *testdata) {
    struct _teststruct *ret;
    ret = malloc(sizeof(struct _teststruct));
    ret->testdata = malloc(sizeof(char)*(strlen(testdata)+1));
    memcpy(ret->testdata, testdata, sizeof(char)*(strlen(testdata)+1));
    return ret;
}

void destroy_test_struct(void *ts) {
    if (!ts) return;
    free(((struct _teststruct*)ts)->testdata);
    free(ts);
}


void print_key(struct _key *key){
    unsigned char x;
    for (x = 0; x<key->keylen-1; x++) {
        printf("%hhu|",key->key[x]);
        if (x>10) break;
    }
    printf("%hhu\n",key->key[x]);
}

void print_key_(struct _key *key){
    unsigned char x;
    for (x = 0; x<key->keylen-1; x++) {
        printf("%hhu|",key->key[x]);
        if (x>10) break;
    }
    printf("%hhu ",key->key[x]);
}

void key_destroy(void *key) {
    free(((struct _key*)key)->key);
    free(key);
}

void* key_clone(void *key, unsigned int len) {
    struct _key * ret;
    //unsigned int x;

    ret = malloc(sizeof(struct _key));
    ret->key = malloc(sizeof(char)*len);
    ret->key = memcpy(ret->key, ((struct _key*)key)->key, sizeof(char)*len);
    ret->keylen = len;
    /*printf("clonned key\n");
    print_key(ret);
    printf("--\n");*/
    return ret;
}

void* make_key(void *x) {
    struct _key *ret;
    unsigned char i;
    ret = malloc(sizeof(struct _key));
    ret->key = malloc(sizeof(char)*sizeof(x));
    for (i = 0; i<sizeof(x); i++) {
        ret->key[i] = ((char*)&x)[sizeof(x)-i-1];
    }
    /*printf("make_key\n");
    for (i = 0; i < sizeof(x); i++) {
        printf("keypart%d=%hhu\n",i,
            ret->key[i]);
    }*/
    //printf("---\n");
    ret->keylen = sizeof(x);
    return ret;
}

unsigned int key_cmp(void *key1, void *key2,
                     unsigned int offset1, unsigned int offset2,
                     unsigned int len, char *ended) {
    unsigned int x;//, min_offset;
    char locended = 0;
    /*print_key((struct _key*)key1);
    print_key((struct _key*)key2);
    printf("----\n");*/
    //printf("final %u\n", (sizeof(int*)-1-len));

    //min_offset = (offset1 > offset2)?offset1:offset2;
    for (x = 0; x < len; x++) {
        if (((struct _key*)key1)->keylen == x+offset1) locended += 1;
        if (((struct _key*)key2)->keylen == x+offset2) locended += 2;
        if (locended != 0) break;
        /*printf("part%d-key1=%hhu part%d-key2=%hhu\n",x,
                ((char*)((struct _key*)key1)->key)[x+offset1],x,
                ((char*)((struct _key*)key2)->key)[x+offset2]);*/
        if (((char*)((struct _key*)key1)->key)[x+offset1]
         != ((char*)((struct _key*)key2)->key)[x+offset2]) break;
    }

    //printf("x: %d ended:%d\n", x, locended);
    *ended = locended;
    return x+1;//sizeof(int*)-1-x+offset1;
}

void* subkey(void *key, unsigned int offset, unsigned int len) {
    struct _key * ret;

    ret = malloc(sizeof(struct _key));
    //ret->key = ((struct _key*)key)->key;
    ret->key = malloc(sizeof(char)*len);
    //memset(((char*)&ret->key), 0, offset);

    memcpy(ret->key, ((struct _key*)key)->key+offset, len-offset);
    //printf("subkey offset:%d len:%d\n", offset, len);
    ret->keylen = len-offset;
    /*printf("subkey: ");
    print_key_(ret);
    printf("\n");*/

    return ret;
}

unsigned int key_len(void *key) {
    return ((struct _key*)key)->keylen;
}

void* key_concat(void *key1, void *key2) {
    struct _key * ret;
    unsigned int i, len;
    ret = malloc(sizeof(struct _key));
    ret->key = malloc(sizeof(char)*(((struct _key*)key1)->keylen +\
                                    ((struct _key*)key2)->keylen));

    //printf("key len1:%d key len2:%d\n", ((struct _key*)key1)->keylen,
    //                       ((struct _key*)key2)->keylen);

    for (i = 0; i < ((struct _key*)key1)->keylen; i++)
        ret->key[i] = ((struct _key*)key1)->key[i];
    ret->keylen = i; 
    len = i;
    //printf("len: %d\n", len);
    for (i = 0; i < ((struct _key*)key2)->keylen; i++)
        ret->key[i+len] = ((struct _key*)key2)->key[i];
    //printf("len2: %d\n", ((struct _key*)key2)->keylen);
    ret->keylen += i;
    return ret;
}

void print_intp_data(void * data, const char * key) {
    if (data == NULL) printf("data for '%s': NULL\n", key);
    else printf("data for '%s': %d\n", key, *(int*)data);
}

void print_intp_hslist(COMPS_HSList * list) {
    COMPS_HSListItem * it;
    if (list == NULL)
        return;
    for (it = list->first; it != NULL; it = it->next) {
        printf("%d ", *(int*)it->data);
    }
    printf("\n");
}

void* int_cloner(void * int_v) {
    void * ret = malloc(sizeof(int));
    memcpy(ret, int_v, sizeof(int));
    return ret;
}

void* str_cloner(void *str) {
    void *ret = malloc(sizeof(char)* (strlen((char*)str)+1));
    memcpy(ret, str, (strlen((char*)str)+1));
    return ret;
}

void print_all(COMPS_BRTree *rt) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    int *x;

    printf("-------- print pairs --------------\n");
    pairlist = comps_brtree_pairs(rt);

    for (it = pairlist->first; it != NULL; it = it->next) {
        x = ((COMPS_BRTreePair*)it->data)->data;
        print_key_(((COMPS_BRTreePair*)it->data)->key);
        printf("= %d\n", *x);

    }
    comps_hslist_destroy(&pairlist);
    printf("-----------------------------------\n");
}

void print_all_str(COMPS_BRTree *rt) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    char *x;

    printf("-------- print pairs --------------\n");
    pairlist = comps_brtree_pairs(rt);

    for (it = pairlist->first; it != NULL; it = it->next) {
        x = ((COMPS_BRTreePair*)it->data)->data;
        printf("%ld = %ld \n",
               (long int)((struct _key*)((COMPS_BRTreePair*)it->data)->key)->key,
                              (long int)x);

    }
    comps_hslist_destroy(&pairlist);
    printf("-----------------------------------\n");
}



int main(int argc, char * argv[]) {

    COMPS_BRTree * tree;//, *clonned;

    int a,b,c,d,e,aa,bb;
    int *ret;
    struct _key *key;

    struct _teststruct *test1, *test2, *test3, **test;

    (void)argc;
    (void)argv;

    tree = comps_brtree_create(NULL, &int_cloner, &free, &key_clone,
                               &key_destroy, &key_cmp, &key_len, &subkey,
                               &key_concat);

    a = 10;
    b = 20;
    c = 30;
    d = 40;
    e = 50;
    aa  = 11;
    bb = 22;

    key = make_key(&a);
    comps_brtree_set(tree, key, int_cloner(&a));
    key_destroy(key);
    key = make_key(&b);
    comps_brtree_set(tree, key, int_cloner(&b));
    key_destroy(key);
    key = make_key(&c);
    comps_brtree_set(tree, key, int_cloner(&c));
    key_destroy(key);
    key = make_key(&d);
    comps_brtree_set(tree, key, int_cloner(&d));
    key_destroy(key);
    key = make_key(&e);
    comps_brtree_set(tree, key, int_cloner(&e));
    key_destroy(key);

    //return 1;
    print_all(tree);
    //comps_brtree_destroy(tree);
    //return 1;
    //print_all(tree);

    key = make_key(&a);
    comps_brtree_set(tree, key, int_cloner(&aa));
    key_destroy(key);
    key = make_key(&b);
    comps_brtree_set(tree, key, int_cloner(&bb));
    key_destroy(key);

    print_all(tree);

    key = make_key(&c);
    ret =  comps_brtree_get(tree, key);
    key_destroy(key);

    if (ret)
        printf("%d\n", *ret);
    else 
        printf("NULL\n");

    comps_brtree_destroy(tree);
    tree = comps_brtree_create(NULL, NULL, &destroy_test_struct, &key_clone,
                               &key_destroy, &key_cmp, &key_len, &subkey,
                               &key_concat);
    
    test1 = make_test_struct("test1");
    test2 = make_test_struct("test2");
    test3 = make_test_struct("test3");

    key = make_key(&test1);
    comps_brtree_set(tree, key, test1);
    key_destroy(key);
    key = make_key(&test2);
    comps_brtree_set(tree, key, test2);
    key_destroy(key);
    key = make_key(&test3);
    comps_brtree_set(tree, key, test3);
    key_destroy(key);

    key = make_key(&test1);
    test = (struct _teststruct **)comps_brtree_getp(tree, key);
    key_destroy(key);
    destroy_test_struct(*test);
    *test = NULL;
    key = make_key(&test1);
    test = comps_brtree_get(tree, key);
    key_destroy(key);
    comps_brtree_destroy(tree);

    //comps_brtree_destroy(tree);


    return 0;
}
