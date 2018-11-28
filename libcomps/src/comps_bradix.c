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

#include "comps_bradix.h"
#include <stdio.h>

void comps_brtree_data_destroy(COMPS_BRTreeData * rtd) {
    rtd->key_destroy(rtd->key);
        if (rtd->data_destructor && rtd->data)
        rtd->data_destructor(rtd->data);
    comps_hslist_destroy(&rtd->subnodes);
    free(rtd);
}

inline void comps_brtree_data_destroy_v(void * rtd) {
    comps_brtree_data_destroy((COMPS_BRTreeData*)rtd);
}

inline COMPS_BRTreeData * __comps_brtree_data_create(COMPS_BRTree *brt,
                                                     void *key,
                                                     unsigned int keylen,
                                                     void *data){
    COMPS_BRTreeData * brtd;
    if ((brtd = malloc(sizeof(COMPS_BRTreeData))) == NULL)
        return NULL;
    brtd->key = brt->key_clone(key, keylen);
    if (!brtd->key) {
        free(brtd);
        return NULL;
    }

    brtd->data = data;
    if (data != NULL) {
            brtd->is_leaf = 1;
    }
    brtd->data_destructor = brt->data_destructor;
    brtd->subnodes = comps_hslist_create();
    brtd->key_destroy = brt->key_destroy;
    comps_hslist_init(brtd->subnodes, NULL, NULL, &comps_brtree_data_destroy_v);
    return brtd;
}

COMPS_BRTreeData * comps_brtree_data_create(COMPS_BRTree *brt,void * key,
                                          void * data) {
    COMPS_BRTreeData * brtd;
    brtd = __comps_brtree_data_create(brt, key, brt->key_len(key), data);
    return brtd;
}

COMPS_BRTreeData * comps_brtree_data_create_n(COMPS_BRTree *brt, void * key,
                                            unsigned keylen, void * data) {
    COMPS_BRTreeData * brtd;
    brtd = __comps_brtree_data_create(brt, key, keylen, data);
    return brtd;
}

COMPS_BRTree * comps_brtree_create(void* (*data_constructor)(void*),
                                   void* (*data_cloner)(void*),
                                   void (*data_destructor)(void*),
                                   void* (*key_clone)(void*, unsigned int),
                                   void (*key_destroy)(void*),
                                   unsigned int (*key_cmp)(void*, void*,
                                                           unsigned int,
                                                           unsigned int,
                                                           unsigned int, char*),
                                   unsigned int (*key_len)(void*),
                                   void* (*subkey)(void*,
                                                   unsigned int,
                                                   unsigned int),
                                   void* (*key_concat)(void*, void*)) {

    COMPS_BRTree *ret;
    if ((ret = malloc(sizeof(COMPS_BRTree))) == NULL)
        return NULL;
    ret->subnodes = comps_hslist_create();
    comps_hslist_init(ret->subnodes, NULL, NULL, &comps_brtree_data_destroy_v);
    if (ret->subnodes == NULL) {
        free(ret);
        return NULL;
    }
    ret->data_constructor = data_constructor;
    ret->data_cloner = data_cloner;
    ret->data_destructor = data_destructor;
    ret->key_clone = key_clone;
    ret->key_destroy = key_destroy;
    ret->key_cmp = key_cmp;
    ret->key_len = key_len;
    ret->subkey = subkey;
    ret->key_concat = key_concat;
    return ret;
}

void comps_brtree_destroy(COMPS_BRTree * brt) {
    if (!brt) return;
    comps_hslist_destroy(&(brt->subnodes));
    free(brt);
}

COMPS_HSList * comps_brtree_values(COMPS_BRTree * brt) {
    COMPS_HSList *ret, *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it, *firstit;
    ret = comps_hslist_create();
    comps_hslist_init(ret, NULL, NULL, NULL);
    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, NULL);
    comps_hslist_append(tmplist, brt->subnodes, 0);
    while (tmplist->first != NULL) {
        it = tmplist->first;
        firstit = it;
        comps_hslist_remove(tmplist, firstit);
        tmp_subnodes = (COMPS_HSList*)it->data;
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_BRTreeData*)it->data)->subnodes->first) {
                comps_hslist_append(tmplist,
                                    ((COMPS_BRTreeData*)it->data)->subnodes, 0);
            }
            if (((COMPS_BRTreeData*)it->data)->data != NULL) {
                comps_hslist_append(ret,
                                    ((COMPS_BRTreeData*)it->data)->data, 0);
            }
        }
        free(firstit);
    }
    comps_hslist_destroy(&tmplist);
    return ret;
}

COMPS_BRTree * comps_brtree_clone(COMPS_BRTree *brt) {

    COMPS_HSList *to_clone, *tmplist, *new_subnodes;
    COMPS_BRTree *ret;
    COMPS_HSListItem *it, *it2;
    COMPS_BRTreeData *brtdata;
    void *new_data;

    if (!brt) return NULL;

    to_clone = comps_hslist_create();
    comps_hslist_init(to_clone, NULL, NULL, NULL);
    ret = comps_brtree_create(brt->data_constructor,
                              brt->data_cloner,
                              brt->data_destructor,
                              brt->key_clone,
                              brt->key_destroy,
                              brt->key_cmp,
                              brt->key_len,
                              brt->subkey,
                              brt->key_concat);


    for (it = brt->subnodes->first; it != NULL; it = it->next) {
        brtdata = comps_brtree_data_create(brt,
                                      ((COMPS_BRTreeData*)it->data)->key, NULL);
        new_data = brt->data_cloner(((COMPS_BRTreeData*)it->data)->data);

        comps_hslist_destroy(&brtdata->subnodes);
        brtdata->subnodes = ((COMPS_BRTreeData*)it->data)->subnodes;
        brtdata->data = new_data;
        comps_hslist_append(ret->subnodes, brtdata, 0);

        comps_hslist_append(to_clone, brtdata, 0);
    }

    while (to_clone->first) {
        it2 = to_clone->first;
        tmplist = ((COMPS_BRTreeData*)it2->data)->subnodes;
        comps_hslist_remove(to_clone, to_clone->first);

        new_subnodes = comps_hslist_create();
        comps_hslist_init(new_subnodes, NULL, NULL, &comps_brtree_data_destroy_v);
        for (it = tmplist->first; it != NULL; it = it->next) {
            brtdata = comps_brtree_data_create(brt,
                                      ((COMPS_BRTreeData*)it->data)->key, NULL);
            if (((COMPS_BRTreeData*)it->data)->data != NULL)
                new_data = brt->data_cloner(((COMPS_BRTreeData*)it->data)->data);
            else
                new_data = NULL;
            comps_hslist_destroy(&brtdata->subnodes);
            brtdata->subnodes = ((COMPS_BRTreeData*)it->data)->subnodes;
            brtdata->data = new_data;
            comps_hslist_append(new_subnodes, brtdata, 0);

            comps_hslist_append(to_clone, brtdata, 0);
        }
        ((COMPS_BRTreeData*)it2->data)->subnodes = new_subnodes;
        free(it2);
    }
    comps_hslist_destroy(&to_clone);
    return ret;
}

void comps_brtree_values_walk(COMPS_BRTree * brt, void* udata,
                              void (*walk_f)(void*, void*)) {
    COMPS_HSList *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it;
    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, NULL);
    comps_hslist_append(tmplist, brt->subnodes, 0);
    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = (COMPS_HSList*)it->data;
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_BRTreeData*)it->data)->subnodes->first) {
                comps_hslist_append(tmplist,
                                    ((COMPS_BRTreeData*)it->data)->subnodes, 0);
            }
            if (((COMPS_BRTreeData*)it->data)->data != NULL) {
               walk_f(udata, ((COMPS_BRTreeData*)it->data)->data);
            }
        }
    }
    comps_hslist_destroy(&tmplist);
}

void comps_brtree_set(COMPS_BRTree * brt, void * key, void * data)
{
    static COMPS_HSListItem *it;
    COMPS_HSList *subnodes;
    COMPS_BRTreeData *brtd;
    static COMPS_BRTreeData *brtdata;

    unsigned int len, offset=0;
    unsigned x, found = 0;
    void *ndata, *newkey;
    char ended;

    len = brt->key_len(key);

    if (brt->subnodes == NULL)
        return;
    if (brt->data_constructor) {
        ndata = brt->data_constructor(data);
    } else {
        ndata = data;
    }

    subnodes = brt->subnodes;
    while (offset < len)
    {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (brt->key_cmp(((COMPS_BRTreeData*)it->data)->key, key,
                                                 0, offset, 1, &ended) == 2) {
                found = 1;
                break;
            }
        }
        if (!found) { // not found in subnodes; create new subnode
            newkey = brt->subkey(key, offset, brt->key_len(key));
            brtd = comps_brtree_data_create(brt, newkey, ndata);
            brt->key_destroy(newkey);
            comps_hslist_append(subnodes, brtd, 0);
            return;
        } else {
            brtdata = (COMPS_BRTreeData*)it->data;
            x = brt->key_cmp(brtdata->key, key, 1, offset+1, len, &ended);
            if (ended == 3) { //keys equals; data replacement
                brt->data_destructor(brtdata->data);
                brtdata->data = ndata;
                return;
            } else if (ended == 2) { //global key ends first; make global leaf
                comps_hslist_remove(subnodes, it);
                it->next = NULL;
                brtd = comps_brtree_data_create(brt, key, ndata);
                comps_hslist_append(subnodes, brtd, 0);
                ((COMPS_BRTreeData*)subnodes->last->data)->subnodes->last = it;
                ((COMPS_BRTreeData*)subnodes->last->data)->subnodes->first = it;
                len = brt->key_len(key) - offset;

                newkey = brt->subkey(brtdata->key, brt->key_len(key)+offset,
                                     brt->key_len(brtdata->key)-len);
                brt->key_destroy(brtdata->key);
                brtdata->key = newkey;

                return;
            } else if (ended == 1) { //local key ends first; go deeper
                subnodes = brtdata->subnodes;
                offset += x;
            } else {
                comps_hslist_remove(subnodes, it);  //remove old node
                brtd = comps_brtree_data_create_n(brt, brtdata->key, x, NULL);
                comps_hslist_append(subnodes, brtd, 0);

                newkey = brt->subkey(key, offset+x, brt->key_len(key));
                brtd = comps_brtree_data_create(brt, newkey, ndata);
                brt->key_destroy(newkey);
                comps_hslist_append(
                            ((COMPS_BRTreeData*)subnodes->last->data)->subnodes,
                            brtd, 0);

                it->next = NULL;
                ((COMPS_BRTreeData*)subnodes->last->data)->subnodes->last->next = it;
                ((COMPS_BRTreeData*)subnodes->last->data)->subnodes->last = it;
                newkey = brt->subkey(brtdata->key, x, brt->key_len(brtdata->key));
                brt->key_destroy(brtdata->key);
                brtdata->key = newkey;
                return;
            }
        }
    }
}


void* comps_brtree_get(COMPS_BRTree *brt, void *key) {
    COMPS_HSList *subnodes;
    COMPS_HSListItem *it = NULL;
    COMPS_BRTreeData *brtdata;
    unsigned int offset, len, x;
    char found, ended;

    len = brt->key_len(key);
    offset = 0;
    subnodes = brt->subnodes;
    while (offset < len) {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (brt->key_cmp(((COMPS_BRTreeData*)it->data)->key, key,
                                                 0, offset, 1, &ended) == 2) {
                found = 1;
                break;
            }
        }
        if (!found)
            return NULL;
        brtdata = (COMPS_BRTreeData*)it->data;

        x = brt->key_cmp(brtdata->key, key, 1, offset+1, len, &ended);
        if (ended == 3) return brtdata->data;
        else if (ended == 1) offset+=x;
        else return NULL;
        subnodes = ((COMPS_BRTreeData*)it->data)->subnodes;
    }
    if (it != NULL)
        return ((COMPS_BRTreeData*)it->data)->data;
    else return NULL;
}

void** comps_brtree_getp(COMPS_BRTree *brt, void *key) {
    COMPS_HSList *subnodes;
    COMPS_HSListItem *it = NULL;
    COMPS_BRTreeData *brtdata;
    unsigned int offset, len, x;
    char found, ended;

    len = brt->key_len(key);
    offset = 0;
    subnodes = brt->subnodes;
    while (offset < len) {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (brt->key_cmp(((COMPS_BRTreeData*)it->data)->key, key,
                                                 0, offset, 1, &ended) == 2) {
                found = 1;
                break;
            }
        }
        if (!found)
            return NULL;
        brtdata = (COMPS_BRTreeData*)it->data;

        x = brt->key_cmp(brtdata->key, key, 1, offset+1, len, &ended);
        if (ended == 3) return &brtdata->data;
        else if (ended == 1) offset+=x;
        else return NULL;
        subnodes = ((COMPS_BRTreeData*)it->data)->subnodes;
    }
    if (it != NULL)
        return &((COMPS_BRTreeData*)it->data)->data;
    else return NULL;
}

void comps_brtree_unset(COMPS_BRTree * brt, void * key) {
    COMPS_HSList * subnodes;
    COMPS_HSListItem * it;
    COMPS_BRTreeData * brtdata;
    unsigned int offset, len, x;
    char found, ended;
    COMPS_HSList * path;

    struct Relation {
        COMPS_HSList * parent_nodes;
        COMPS_HSListItem * child_it;
    } *relation;

    path = comps_hslist_create();
    if (path == NULL) {
       return;
    }
    comps_hslist_init(path, NULL, NULL, &free);

    len = strlen(key);
    offset = 0;
    subnodes = brt->subnodes;
    while (offset != len) {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (brt->key_cmp(((COMPS_BRTreeData*)it->data)->key, key,
                                                 0, offset, 1, &ended)) {
                found = 1;
                break;
            }
        }
        if (!found) {
            comps_hslist_destroy(&path);
            return;
        }
        brtdata = (COMPS_BRTreeData*)it->data;
        x = brt->key_cmp(brtdata->key, key, 1, offset+1, len, &ended);
        if (ended == 3) {
            /* remove node from tree only if there's no descendant*/
            if (brtdata->subnodes->last == NULL) {
                printf("removing all\n");
                comps_hslist_remove(subnodes, it);
                comps_brtree_data_destroy(brtdata);
                free(it);
            }
            else if (brtdata->data_destructor != NULL) {
                printf("removing data only\n");
                brtdata->data_destructor(brtdata->data);
                brtdata->is_leaf = 0;
                brtdata->data = NULL;
            }

            if (path->last == NULL) {
                comps_hslist_destroy(&path);
                return;
            }
            brtdata = (COMPS_BRTreeData*)
                     ((struct Relation*)path->last->data)->child_it->data;

            /*remove all predecessor of deleted node (recursive) with no childs*/
            while (brtdata->subnodes->last == NULL) {
                //printf("removing '%s'\n", brtdata->key);
                comps_brtree_data_destroy(brtdata);
                comps_hslist_remove(
                            ((struct Relation*)path->last->data)->parent_nodes,
                            ((struct Relation*)path->last->data)->child_it);
                free(((struct Relation*)path->last->data)->child_it);
                it = path->last;
                comps_hslist_remove(path, path->last);
                free(it);
                brtdata = (COMPS_BRTreeData*)
                         ((struct Relation*)path->last->data)->child_it->data;
            }
            comps_hslist_destroy(&path);
            return;
        }
        else if (ended == 1) offset+=x;
        else {
            comps_hslist_destroy(&path);
            return;
        }
        if ((relation = malloc(sizeof(struct Relation))) == NULL) {
            comps_hslist_destroy(&path);
            return;
        }
        subnodes = ((COMPS_BRTreeData*)it->data)->subnodes;
        relation->parent_nodes = subnodes;
        relation->child_it = it;
        comps_hslist_append(path, (void*)relation, 0);
    }
    comps_hslist_destroy(&path);
    return;
}

void comps_brtree_clear(COMPS_BRTree * rt) {
    COMPS_HSListItem *it, *oldit;
    if (rt==NULL) return;
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


inline COMPS_HSList* __comps_brtree_all(COMPS_BRTree * brt, char pairorkey) {
    (void)brt;
    (void)pairorkey;

    COMPS_HSList *tmplist, *tmp_subnodes, *ret;
    COMPS_HSListItem *it;
    struct Pair {
        COMPS_HSList * subnodes;
        void * key;
        char added;
    } *pair, *parent_pair;
    COMPS_BRTreePair *rtpair;

    pair = malloc(sizeof(struct Pair));
    pair->subnodes = brt->subnodes;
    pair->key = NULL;
    pair->added = 0;

    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, &free);
    ret = comps_hslist_create();
    if (pairorkey == 1)
        comps_hslist_init(ret, NULL, NULL, &free);
    else
        comps_hslist_init(ret, NULL, NULL, &comps_brtree_pair_destroy_v);

    comps_hslist_append(tmplist, pair, 0);

    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = ((struct Pair*)it->data)->subnodes;
        parent_pair = (struct Pair*) it->data;
        free(it);

        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            pair = malloc(sizeof(struct Pair));
            pair->subnodes = ((COMPS_BRTreeData*)it->data)->subnodes;
            pair->added = 0;

            if (parent_pair->key != NULL) {
                pair->key = brt->key_concat(parent_pair->key,
                                            ((COMPS_BRTreeData*)it->data)->key);
            } else {
                pair->key = brt->key_clone(((COMPS_BRTreeData*)it->data)->key,
                                           brt->key_len(((COMPS_BRTreeData*)
                                                        it->data)->key));
            }
            if (((COMPS_BRTreeData*)it->data)->data != NULL) {
                if (pairorkey == 1) {
                    comps_hslist_append(ret, pair->key, 0);
                } else {
                    rtpair = malloc(sizeof(COMPS_BRTreePair));
                    rtpair->key = pair->key;
                    rtpair->data = ((COMPS_BRTreeData*)it->data)->data;
                    rtpair->key_destroy = brt->key_destroy;
                    comps_hslist_append(ret, rtpair, 0);
                }
                pair->added = 1;
                if (((COMPS_BRTreeData*)it->data)->subnodes->first != NULL) {
                    comps_hslist_append(tmplist, pair, 0);
                } else {
                    free(pair);
                }
            /// current node hasn't data /
            } else {
                if (((COMPS_BRTreeData*)it->data)->subnodes->first) {
                    comps_hslist_append(tmplist, pair, 0);
                } else {
                    brt->key_destroy(pair->key);
                    free(pair);
                }
            }
        }
        if (parent_pair->added == 0) {
            if (parent_pair->key)
                brt->key_destroy(parent_pair->key);
        }
        free(parent_pair);
    }
    comps_hslist_destroy(&tmplist);
    return ret;

}

COMPS_HSList* comps_brtree_keys(COMPS_BRTree * rt) {
    return __comps_brtree_all(rt, 1);
}

COMPS_HSList* comps_brtree_pairs(COMPS_BRTree * rt) {
    return __comps_brtree_all(rt, 0);
}

inline void comps_brtree_pair_destroy(COMPS_BRTreePair * pair) {
    pair->key_destroy(pair->key);
    free(pair);
}

inline void comps_brtree_pair_destroy_v(void * pair) {
    ((COMPS_BRTreePair*)pair)->key_destroy(((COMPS_BRTreePair*)pair)->key);
    free(pair);
}

