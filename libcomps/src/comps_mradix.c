#include "comps_mradix.h"
#include <stdio.h>

void comps_mrtree_data_destroy(COMPS_MRTreeData * rtd) {
////    printf("Destroying data '%s'\n", rtd->key);
    free(rtd->key);
    comps_hslist_destroy(&rtd->data);
    comps_hslist_destroy(&rtd->subnodes);
    free(rtd);
}

inline void comps_mrtree_data_destroy_v(void * rtd) {
    comps_mrtree_data_destroy((COMPS_MRTreeData*)rtd);
}

COMPS_MRTreeData * comps_mrtree_data_create(COMPS_MRTree * tree,
                                            char * key, void * data) {

    COMPS_MRTreeData * rtd;
    if ((rtd = malloc(sizeof(*rtd))) == NULL)
        return NULL;
    if ((rtd->key = malloc(sizeof(char) * (strlen(key)+1))) == NULL) {
        free(rtd);
        return NULL;
    }
    memcpy(rtd->key, key, sizeof(char)*((strlen(key)+1)));
    rtd->is_leaf = 1;
    rtd->data = comps_hslist_create();
    comps_hslist_init(rtd->data, NULL, tree->data_cloner,
                                       tree->data_destructor);
    if (data)
        comps_hslist_append(rtd->data, data, 0);
    rtd->subnodes = comps_hslist_create();
    comps_hslist_init(rtd->subnodes, NULL,
                                     NULL,
                                     &comps_mrtree_data_destroy_v);
    return rtd;
}

COMPS_MRTreeData * comps_mrtree_data_create_n(COMPS_MRTree * tree, char * key,
                                            unsigned keylen, void * data) {
    COMPS_MRTreeData * rtd;
    if ((rtd = malloc(sizeof(*rtd))) == NULL)
        return NULL;
    if ((rtd->key = malloc(sizeof(char) * (keylen+1))) == NULL) {
        free(rtd);
        return NULL;
    }
    memcpy(rtd->key, key, sizeof(char)*keylen);
    rtd->key[keylen] = 0;
    rtd->is_leaf = 1;
    rtd->data = comps_hslist_create();
    comps_hslist_init(rtd->data, NULL, tree->data_cloner,
                                       tree->data_destructor);
    if (data)
        comps_hslist_append(rtd->data, data, 0);
    rtd->subnodes = comps_hslist_create();
    comps_hslist_init(rtd->subnodes, NULL,
                                     NULL,
                                     &comps_mrtree_data_destroy_v);
    return rtd;
}

COMPS_MRTree * comps_mrtree_create(void* (*data_constructor)(void*),
                                   void* (*data_cloner)(void*),
                                   void (*data_destructor)(void*)) {
    COMPS_MRTree *ret;
    if ((ret = malloc(sizeof(COMPS_MRTree))) == NULL)
        return NULL;
    ret->subnodes = comps_hslist_create();
    comps_hslist_init(ret->subnodes, NULL, NULL, &comps_mrtree_data_destroy_v);
    if (ret->subnodes == NULL) {
        free(ret);
        return NULL;
    }
    ret->data_constructor = data_constructor;
    ret->data_cloner = data_cloner;
    ret->data_destructor = data_destructor;
    return ret;
}

void comps_mrtree_destroy(COMPS_MRTree * rt) {
    comps_hslist_destroy(&(rt->subnodes));
    free(rt);
}

void comps_mrtree_print(COMPS_HSList * hl, unsigned  deep) {
    COMPS_HSListItem * it;
    for (it = hl->first; it != NULL; it=it->next) {
        printf("%d %s\n",deep, (((COMPS_MRTreeData*)it->data)->key));
        comps_mrtree_print(((COMPS_MRTreeData*)it->data)->subnodes, deep+1);
    }
}

void comps_mrtree_values_walk(COMPS_MRTree * rt, void* udata,
                              void (*walk_f)(void*, void*)) {
    COMPS_HSList *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it, *it2;
    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, NULL);
    comps_hslist_append(tmplist, rt->subnodes, 0);
    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = (COMPS_HSList*)it->data;
        free(it);
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_MRTreeData*)it->data)->subnodes->first) {
                comps_hslist_append(tmplist,
                                    ((COMPS_MRTreeData*)it->data)->subnodes, 0);
            }
            for (it2 = (COMPS_HSListItem*)((COMPS_MRTreeData*)it->data)->data->first;
                 it2 != NULL; it2 = it2->next) {
                walk_f(udata, it2->data);
            }
        }
    }
    comps_hslist_destroy(&tmplist);
}

COMPS_MRTree * comps_mrtree_clone(COMPS_MRTree * rt) {

    COMPS_HSList * to_clone, *tmplist, *new_subnodes;
    COMPS_MRTree * ret;
    COMPS_HSListItem *it, *it2;
    COMPS_MRTreeData *rtdata;//, *rtdata2;
    COMPS_HSList *new_data_list;

    to_clone = comps_hslist_create();
    comps_hslist_init(to_clone, NULL, NULL, NULL);
    ret = comps_mrtree_create(rt->data_constructor, rt->data_cloner,
                              rt->data_destructor);

    for (it = rt->subnodes->first; it != NULL; it = it->next) {
        rtdata = comps_mrtree_data_create(rt,
                                      ((COMPS_MRTreeData*)it->data)->key, NULL);
        new_data_list = comps_hslist_clone(((COMPS_MRTreeData*)it->data)->data);
        comps_hslist_destroy(&rtdata->data);
        comps_hslist_destroy(&rtdata->subnodes);
        rtdata->subnodes = ((COMPS_MRTreeData*)it->data)->subnodes;
        rtdata->data = new_data_list;
        comps_hslist_append(ret->subnodes, rtdata, 0);

        comps_hslist_append(to_clone, rtdata, 0);
    }


    while (to_clone->first) {
        it2 = to_clone->first;
        tmplist = ((COMPS_MRTreeData*)it2->data)->subnodes;
        comps_hslist_remove(to_clone, to_clone->first);

        new_subnodes = comps_hslist_create();
        comps_hslist_init(new_subnodes, NULL, NULL, &comps_mrtree_data_destroy_v);
        for (it = tmplist->first; it != NULL; it = it->next) {
            rtdata = comps_mrtree_data_create(rt,
                                      ((COMPS_MRTreeData*)it->data)->key, NULL);
            new_data_list = comps_hslist_clone(((COMPS_MRTreeData*)it->data)->data);
            comps_hslist_destroy(&rtdata->subnodes);
            comps_hslist_destroy(&rtdata->data);
            rtdata->subnodes = ((COMPS_MRTreeData*)it->data)->subnodes;
            rtdata->data = new_data_list;
            comps_hslist_append(new_subnodes, rtdata, 0);

            comps_hslist_append(to_clone, rtdata, 0);
        }
        ((COMPS_MRTreeData*)it2->data)->subnodes = new_subnodes;
        free(it2);
    }
    comps_hslist_destroy(&to_clone);
    return ret;
}

void comps_mrtree_unite(COMPS_MRTree *rt1, COMPS_MRTree *rt2) {
    COMPS_HSList *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it, *it2;
    struct Pair {
        COMPS_HSList * subnodes;
        char * key;
        char added;
    } *pair, *parent_pair;

    pair = malloc(sizeof(struct Pair));
    pair->subnodes = rt2->subnodes;
    pair->key = NULL;

    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, &free);
    comps_hslist_append(tmplist, pair, 0);

    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = ((struct Pair*)it->data)->subnodes;
        parent_pair = (struct Pair*) it->data;
        free(it);

        pair->added = 0;
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            pair = malloc(sizeof(struct Pair));
            pair->subnodes = ((COMPS_MRTreeData*)it->data)->subnodes;

            if (parent_pair->key != NULL) {
                pair->key =
                    malloc(sizeof(char)
                           * (strlen(((COMPS_MRTreeData*)it->data)->key)
                           + strlen(parent_pair->key) + 1));
                memcpy(pair->key, parent_pair->key,
                       sizeof(char) * strlen(parent_pair->key));
                memcpy(pair->key+strlen(parent_pair->key),
                       ((COMPS_MRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_MRTreeData*)it->data)->key)+1));
            } else {
                pair->key = malloc(sizeof(char)*
                                (strlen(((COMPS_MRTreeData*)it->data)->key) +
                                1));
                memcpy(pair->key, ((COMPS_MRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_MRTreeData*)it->data)->key)+1));
            }
            /* current node has data */
            if (((COMPS_MRTreeData*)it->data)->data->first != NULL) {
            //    printf("data not null for |%s|\n", pair->key);
                for (it2 = ((COMPS_MRTreeData*)it->data)->data->first;
                     it2 != NULL; it2 = it2->next) {
                    comps_mrtree_set(rt1, pair->key, it2->data);
                }

                if (((COMPS_MRTreeData*)it->data)->subnodes->first) {
                    comps_hslist_append(tmplist, pair, 0);
                } else {
                    free(pair->key);
                    free(pair);
                }
            /* current node hasn't data */
            } else {
                if (((COMPS_MRTreeData*)it->data)->subnodes->first) {
                    comps_hslist_append(tmplist, pair, 0);
                } else {
                    free(pair->key);
                    free(pair);
                }
            }
        }
        free(parent_pair->key);
        free(parent_pair);
    }
    comps_hslist_destroy(&tmplist);
}

COMPS_HSList* comps_mrtree_keys(COMPS_MRTree * rt) {
    COMPS_HSList *tmplist, *tmp_subnodes, *ret;
    COMPS_HSListItem *it;
    struct Pair {
        COMPS_HSList * subnodes;
        char * key;
        char added;
    } *pair, *parent_pair;

    pair = malloc(sizeof(struct Pair));
    pair->subnodes = rt->subnodes;
    pair->key = NULL;
    pair->added = 0;

    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, &free);
    ret = comps_hslist_create();
    comps_hslist_init(ret, NULL, NULL, &free);
    comps_hslist_append(tmplist, pair, 0);

    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = ((struct Pair*)it->data)->subnodes;
        parent_pair = (struct Pair*) it->data;
        free(it);

        //pair->added = 0;
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            pair = malloc(sizeof(struct Pair));
            pair->subnodes = ((COMPS_MRTreeData*)it->data)->subnodes;
            pair->added = 0;

            if (parent_pair->key != NULL) {
                pair->key =
                    malloc(sizeof(char)
                           * (strlen(((COMPS_MRTreeData*)it->data)->key)
                           + strlen(parent_pair->key) + 1));
                memcpy(pair->key, parent_pair->key,
                       sizeof(char) * strlen(parent_pair->key));
                memcpy(pair->key+strlen(parent_pair->key),
                       ((COMPS_MRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_MRTreeData*)it->data)->key)+1));
            } else {
                pair->key = malloc(sizeof(char)*
                                (strlen(((COMPS_MRTreeData*)it->data)->key) +
                                1));
                memcpy(pair->key, ((COMPS_MRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_MRTreeData*)it->data)->key)+1));
            }
            //printf("%s|\n", pair->key);
            /* current node has data */
            if (((COMPS_MRTreeData*)it->data)->data->first != NULL) {
                //printf("data not null for |%s|\n", pair->key);
                comps_hslist_append(ret, pair->key, 0);
                pair->added = 1;
                if (((COMPS_MRTreeData*)it->data)->subnodes->first != NULL) {
//                    printf("subnodes found\b");
                    comps_hslist_append(tmplist, pair, 0);
                } else {
                    free(pair);
                }
            /* current node hasn't data */
            } else {
                //printf("no data\n");
                if (((COMPS_MRTreeData*)it->data)->subnodes->first) {
                    comps_hslist_append(tmplist, pair, 0);
                    //added = 1;
                } else {
                    free(pair->key);
                    free(pair);
                }
            }
        }
        if (parent_pair->added == 0)
            free(parent_pair->key);
        free(parent_pair);
    }
    comps_hslist_destroy(&tmplist);
    return ret;
}

void comps_mrtree_set(COMPS_MRTree * rt, char * key, void * data)
{
    static COMPS_HSListItem *it;
    COMPS_HSList *subnodes;
    COMPS_MRTreeData *rtd;
    static COMPS_MRTreeData *rtdata;

    unsigned int len, offset=0;
    unsigned x, found = 0;
    void *ndata;
    char ended, tmpch;

    len = strlen(key);

    if (rt->subnodes == NULL)
        return;
    if (rt->data_constructor)
        ndata = rt->data_constructor(data);
    else
        ndata = data;

    subnodes = rt->subnodes;
    while (offset != len)
    {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_MRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
//        printf("%s found %d\n", key+offset, found);
        if (!found) { // not found in subnodes; create new subnode
            rtd = comps_mrtree_data_create(rt, key+offset, ndata);
            comps_hslist_append(subnodes, rtd, 0);
            return;
        } else {
            rtdata = (COMPS_MRTreeData*)it->data;
            ended = 0;
////            printf("gk:%s| lk:%s|\n", key+offset, rtdata->key);
            for (x=1; ;x++) {
                if (rtdata->key[x] == 0) ended += 1;
                if (x == len - offset) ended += 2;
                if (ended != 0) break;
                if (key[offset+x] != rtdata->key[x]) break;
            }
//            printf("ended:%d\n", ended);
            if (ended == 3) { //keys equals; append new data
                comps_hslist_append(rtdata->data, ndata, 0);
                return;
            } else if (ended == 2) { //global key ends first; make global leaf
                comps_hslist_remove(subnodes, it);
                it->next = NULL;
                rtd = comps_mrtree_data_create(rt, key,  ndata);
                comps_hslist_append(subnodes, rtd, 0);
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->last = it;
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->first = it;
                len = strlen(key + offset);
////                printf("key+offset:|%s|\n", rtdata->key + len);
                memmove(rtdata->key,rtdata->key+strlen(key+offset),
                                    strlen(rtdata->key) - len);
                rtdata->key[strlen(rtdata->key) - len] = 0;
                rtdata->key = realloc(rtdata->key, sizeof(char)* (strlen(rtdata->key)+1));
////                printf("new sub-key: |%s|\n", rtdata->key);
                return;
            } else if (ended == 1) { //local key ends first; go deeper
                subnodes = rtdata->subnodes;
                offset += x;
            } else { /* keys differ */
////                comps_mrtree_print(subnodes,0);
                comps_hslist_remove(subnodes, it);      /*remove existing item*/
                tmpch = rtdata->key[x];
                rtdata->key[x] = 0;     /*cut existing item key to common key*/
////                printf("new part-key: |%s|\n", rtdata->key);
                /* create new parent node with common key with no data*/
                rtd = comps_mrtree_data_create(rt, rtdata->key, NULL);
                /* append to the end of subnodes in current tree level*/
                comps_hslist_append(subnodes, rtd, 0);

                /* create new item with differ part inserting key*/
                rtd = comps_mrtree_data_create(rt, key+offset+x, ndata);
////                printf("new sub-key1: |%s|\n", rtd->key);
                comps_hslist_append(
                            ((COMPS_MRTreeData*)subnodes->last->data)->subnodes,
                            rtd, 0);
                /* create new item with differ part existing key*/
                it->next = NULL;
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->last->next = it;
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->last = it;
                rtdata->key[x] = tmpch;
////                printf("len: %d\n",(int)strlen(rtdata->key+x));
                len = strlen(rtdata->key+x);
                memmove(rtdata->key,rtdata->key+x, sizeof(char)*len);
                rtdata->key = realloc(rtdata->key, sizeof(char)*(len+1));
                rtdata->key[len] = 0;
////                printf("new sub-key2: |%s|\n", rtdata->key);
                return;
            }
        }
    }
}

void comps_mrtree_set_n(COMPS_MRTree *rt, char *key,
                        unsigned int len, void *data)
{
    static COMPS_HSListItem *it;
    COMPS_HSList *subnodes;
    COMPS_MRTreeData *rtd;
    static COMPS_MRTreeData *rtdata;

    unsigned int klen, offset=0;
    unsigned x, found = 0;
    void *ndata;
    char ended, tmpch;

    if (rt->subnodes == NULL)
        return;
    if (rt->data_constructor)
        ndata = rt->data_constructor(data);
    else
        ndata = data;

    subnodes = rt->subnodes;
    while (offset != len)
    {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_MRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
////        printf("%s found %d\n", key+offset, found);
        if (!found) { /* not found in subnodes; create new subnode */
            rtd = comps_mrtree_data_create_n(rt, key+offset, len, ndata);
            comps_hslist_append(subnodes, rtd, 0);
            return;
        } else {
            rtdata = (COMPS_MRTreeData*)it->data;
            ended = 0;
////            printf("gk:%s| lk:%s|\n", key+offset, rtdata->key);
            for (x=1; ;x++) {
                if (rtdata->key[x] == 0) ended += 1;
                if (x == len-offset) ended += 2;
                if (ended != 0) break;
                if (key[offset+x] != rtdata->key[x]) break;
            }
////            printf("ended:%d\n", ended);
            if (ended == 3) { //keys equals; append new data
                comps_hslist_append(rtdata->data, ndata, 0);
                return;
            } else if (ended == 2) { //global key ends first; make global leaf
                comps_hslist_remove(subnodes, it);
                it->next = NULL;
                rtd = comps_mrtree_data_create_n(rt, key+offset, len, ndata);
                comps_hslist_append(subnodes, rtd, 0);
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->last = it;
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->first = it;
                klen = len-offset;
                memmove(rtdata->key,rtdata->key+(len-offset), sizeof(char)*klen);
                rtdata->key[len] = 0;
////                printf("new sub-key: |%s|\n", rtdata->key);
                return;
            } else if (ended == 1) { //local key ends first; go deeper
                subnodes = rtdata->subnodes;
                offset += x;
            } else {
////                comps_mrtree_print(subnodes,0);
                comps_hslist_remove(subnodes, it);
                tmpch = rtdata->key[x];
                rtdata->key[x] = 0;
////                printf("new part-key: %s\n", rtdata->key);
                rtd = comps_mrtree_data_create(rt, rtdata->key, NULL);
                comps_hslist_append(subnodes, rtd, 0);

                rtd = comps_mrtree_data_create_n(rt, key+offset+x,
                                                 len-offset-x, ndata);
////                printf("new sub-key1: %s\n", rtd->key);
                comps_hslist_append(
                            ((COMPS_MRTreeData*)subnodes->last->data)->subnodes,
                            rtd, 0);

                rtdata->key[x] = tmpch;
                it->next = NULL;
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->last->next = it;
                ((COMPS_MRTreeData*)subnodes->last->data)->subnodes->last = it;
////                printf("len: %d\n",(int)strlen(rtdata->key+x));
                len = strlen(rtdata->key+x);
                memmove(rtdata->key,rtdata->key+x, sizeof(char)*len);
                rtdata->key = realloc(rtdata->key, sizeof(char)*(len+1));
                rtdata->key[len] = 0;
////                printf("new sub-key2: %s\n", rtdata->key);
                return;
            }
        }
    }
}

COMPS_HSList * comps_mrtree_get(COMPS_MRTree * rt, const char * key) {
    COMPS_HSList * subnodes;
    COMPS_HSListItem * it;
    COMPS_MRTreeData * rtdata;
    unsigned int offset, len, x;
    char found, ended;

    len = strlen(key);
    offset = 0;
    subnodes = rt->subnodes;
    while (offset != len) {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_MRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
        if (!found)
            return NULL;
        rtdata = (COMPS_MRTreeData*)it->data;

        for (x=1; ;x++) {
            ended=0;
            if (rtdata->key[x] == 0) ended += 1;
            if (x == len - offset) ended += 2;
            if (ended != 0) break;
            if (key[offset+x] != rtdata->key[x]) break;
        }
        if (ended == 3) return rtdata->data;
        else if (ended == 1) offset+=x;
        else return NULL;
        subnodes = ((COMPS_MRTreeData*)it->data)->subnodes;
    }
    return ((COMPS_MRTreeData*)it->data)->data;
}

COMPS_HSList ** comps_mrtree_getp(COMPS_MRTree * rt, const char * key) {
    COMPS_HSList * subnodes;
    COMPS_HSListItem * it;
    COMPS_MRTreeData * rtdata;
    unsigned int offset, len, x;
    char found, ended;

    len = strlen(key);
    offset = 0;
    subnodes = rt->subnodes;
    while (offset != len) {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_MRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
        if (!found)
            return NULL;
        rtdata = (COMPS_MRTreeData*)it->data;

        for (x=1; ;x++) {
            ended=0;
            if (rtdata->key[x] == 0) ended += 1;
            if (x == len - offset) ended += 2;
            if (ended != 0) break;
            if (key[offset+x] != rtdata->key[x]) break;
        }
        if (ended == 3) return &rtdata->data;
        else if (ended == 1) offset+=x;
        else return NULL;
        subnodes = ((COMPS_MRTreeData*)it->data)->subnodes;
    }
    return &((COMPS_MRTreeData*)it->data)->data;
}

void comps_mrtree_unset(COMPS_MRTree * rt, const char * key) {
    COMPS_HSList * subnodes;
    COMPS_HSListItem * it;
    COMPS_MRTreeData * rtdata;
    unsigned int offset, len, x;
    char found, ended;
    COMPS_HSList * path;

    struct Relation {
        COMPS_HSList * parent_nodes;
        COMPS_HSListItem * child_it;
    } *relation;

    path = comps_hslist_create();
    comps_hslist_init(path, NULL, NULL, &free);

    len = strlen(key);
    offset = 0;
    subnodes = rt->subnodes;
    while (offset != len) {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_MRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
        if (!found)
            return;
        rtdata = (COMPS_MRTreeData*)it->data;

        for (x=1; ;x++) {
            ended=0;
            if (rtdata->key[x] == 0) ended += 1;
            if (x == len - offset) ended += 2;
            if (ended != 0) break;
            if (key[offset+x] != rtdata->key[x]) break;
        }
        if (ended == 3) {
            /* remove node from tree only if there's no descendant*/
            if (rtdata->subnodes->last == NULL) {
                printf("removing all\n");
                comps_hslist_remove(subnodes, it);
                comps_mrtree_data_destroy(rtdata);
                free(it);
            }
            else {
                printf("removing data only\n");
                comps_hslist_clear(rtdata->data);
                rtdata->is_leaf = 0;
            }

            if (path->last == NULL) {
                comps_hslist_destroy(&path);
                return;
            }
            rtdata = (COMPS_MRTreeData*)
                     ((struct Relation*)path->last->data)->child_it->data;

            /*remove all predecessor of deleted node (recursive) with no childs*/
            while (rtdata->subnodes->last == NULL) {
                printf("removing '%s'\n", rtdata->key);
                comps_mrtree_data_destroy(rtdata);
                comps_hslist_remove(
                            ((struct Relation*)path->last->data)->parent_nodes,
                            ((struct Relation*)path->last->data)->child_it);
                free(((struct Relation*)path->last->data)->child_it);
                it = path->last;
                comps_hslist_remove(path, path->last);
                free(it);
                rtdata = (COMPS_MRTreeData*)
                         ((struct Relation*)path->last->data)->child_it->data;
            }
            comps_hslist_destroy(&path);
            return;
        }
        else if (ended == 1) offset+=x;
        else return;
        if ((relation = malloc(sizeof(struct Relation))) == NULL) {
            comps_hslist_destroy(&path);
            return;
        }
        subnodes = ((COMPS_MRTreeData*)it->data)->subnodes;
        relation->parent_nodes = subnodes;
        relation->child_it = it;
        comps_hslist_append(path, (void*)relation, 0);
    }
    comps_hslist_destroy(&path);
    return;
}

void comps_mrtree_clear(COMPS_MRTree * rt) {
    COMPS_HSListItem *it, *oldit;
    if (rt == NULL) return;
    if (rt->subnodes == NULL) return;
    oldit = rt->subnodes->first;
    it = (oldit)?oldit->next:NULL;
    for (;it != NULL; it=it->next) {
        if (rt->subnodes->data_destructor != NULL)
            rt->subnodes->data_destructor(oldit->data);
        free(oldit);
        oldit = it;
    }
    if (oldit) {
        if (rt->subnodes->data_destructor != NULL)
            rt->subnodes->data_destructor(oldit->data);
        free(oldit);
    }
}
