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

#include "comps_objradix.h"
#include <stdio.h>

void comps_objrtree_data_destroy(COMPS_ObjRTreeData * rtd) {
    free(rtd->key);
    comps_object_destroy(rtd->data);
    comps_hslist_destroy(&rtd->subnodes);
    free(rtd);
}

inline void comps_objrtree_data_destroy_v(void * rtd) {
    comps_objrtree_data_destroy((COMPS_ObjRTreeData*)rtd);
}

inline COMPS_ObjRTreeData * __comps_objrtree_data_create(char *key,
                                                   unsigned int keylen,
                                                   COMPS_Object *data){
    COMPS_ObjRTreeData * rtd;
    if ((rtd = malloc(sizeof(*rtd))) == NULL)
        return NULL;
    if ((rtd->key = malloc(sizeof(char) * (keylen+1))) == NULL) {
        free(rtd);
        return NULL;
    }
    memcpy(rtd->key, key, sizeof(char)*keylen);
    rtd->key[keylen] = 0;
    rtd->data = data;
    if (data != NULL) {
        rtd->is_leaf = 1;
    }
    rtd->subnodes = comps_hslist_create();
    comps_hslist_init(rtd->subnodes, NULL, NULL, &comps_objrtree_data_destroy_v);
    return rtd;
}

COMPS_ObjRTreeData * comps_objrtree_data_create(char *key, COMPS_Object *data) {
    COMPS_ObjRTreeData * rtd;
    rtd = __comps_objrtree_data_create(key, strlen(key), data);
    return rtd;
}

COMPS_ObjRTreeData * comps_objrtree_data_create_n(char *key, unsigned keylen,
                                                  COMPS_Object *data) {
    COMPS_ObjRTreeData * rtd;
    rtd = __comps_objrtree_data_create(key, keylen, data);
    return rtd;
}

void comps_objrtree_create(COMPS_ObjRTree *rtree, COMPS_Object **args) {
    (void)args;
    rtree->subnodes = comps_hslist_create();
    comps_hslist_init(rtree->subnodes, NULL, NULL, &comps_objrtree_data_destroy_v);
    if (rtree->subnodes == NULL) {
        COMPS_OBJECT_DESTROY(rtree);
        return;
    }
    rtree->len = 0;
}
COMPS_CREATE_u(objrtree, COMPS_ObjRTree)

void comps_objrtree_destroy(COMPS_ObjRTree * rt) {
    comps_hslist_destroy(&(rt->subnodes));
}
COMPS_DESTROY_u(objrtree, COMPS_ObjRTree)

COMPS_ObjList * comps_objrtree_values(COMPS_ObjRTree * rt) {
    COMPS_ObjList *ret;
    COMPS_HSList *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it, *firstit;
    ret = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo, NULL);
    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, NULL);
    comps_hslist_append(tmplist, rt->subnodes, 0);
    while (tmplist->first != NULL) {
        it = tmplist->first;
        firstit = it;
        comps_hslist_remove(tmplist, firstit);
        tmp_subnodes = (COMPS_HSList*)it->data;
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_ObjRTreeData*)it->data)->subnodes->first) {
                comps_hslist_append(tmplist,
                                    ((COMPS_ObjRTreeData*)it->data)->subnodes, 0);
            }
            if (((COMPS_ObjRTreeData*)it->data)->data != NULL) {
                comps_objlist_append(ret,
                                    ((COMPS_ObjRTreeData*)it->data)->data);
            }
        }
        free(firstit);
    }
    comps_hslist_destroy(&tmplist);
    return ret;
}

COMPS_ObjRTree * comps_objrtree_clone(COMPS_ObjRTree *rt) {

    COMPS_HSList *to_clone, *tmplist, *new_subnodes;
    COMPS_ObjRTree *ret;
    COMPS_HSListItem *it, *it2;
    COMPS_ObjRTreeData *rtdata;
    COMPS_Object *new_data;

    if (!rt) return NULL;

    to_clone = comps_hslist_create();
    comps_hslist_init(to_clone, NULL, NULL, NULL);
    ret = (COMPS_ObjRTree*)comps_object_create(&COMPS_ObjRTree_ObjInfo, NULL);

    for (it = rt->subnodes->first; it != NULL; it = it->next) {
        rtdata = comps_objrtree_data_create(
                                    ((COMPS_ObjRTreeData*)it->data)->key, NULL);
        if (((COMPS_ObjRTreeData*)it->data)->data != NULL)
            new_data = comps_object_copy(((COMPS_ObjRTreeData*)it->data)->data);
        else
            new_data = NULL;
        comps_hslist_destroy(&rtdata->subnodes);
        rtdata->subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;
        rtdata->data = new_data;
        comps_hslist_append(ret->subnodes, rtdata, 0);
        comps_hslist_append(to_clone, rtdata, 0);
    }

    while (to_clone->first) {
        it2 = to_clone->first;
        tmplist = ((COMPS_ObjRTreeData*)it2->data)->subnodes;
        comps_hslist_remove(to_clone, to_clone->first);

        new_subnodes = comps_hslist_create();
        comps_hslist_init(new_subnodes, NULL, NULL, &comps_objrtree_data_destroy_v);
        for (it = tmplist->first; it != NULL; it = it->next) {
            rtdata = comps_objrtree_data_create(
                                      ((COMPS_ObjRTreeData*)it->data)->key, NULL);
            if (((COMPS_ObjRTreeData*)it->data)->data != NULL)
                new_data = comps_object_copy(((COMPS_ObjRTreeData*)it->data)->data);
            else
                new_data = NULL;
            comps_hslist_destroy(&rtdata->subnodes);
            rtdata->subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;
            rtdata->data = new_data;
            comps_hslist_append(new_subnodes, rtdata, 0);
            comps_hslist_append(to_clone, rtdata, 0);
        }
        ((COMPS_ObjRTreeData*)it2->data)->subnodes = new_subnodes;
        free(it2);
    }
    comps_hslist_destroy(&to_clone);
    return ret;
}
void comps_objrtree_copy(COMPS_ObjRTree *rt1, COMPS_ObjRTree *rt2){
    COMPS_HSList *to_clone, *tmplist, *new_subnodes;
    COMPS_HSListItem *it, *it2;
    COMPS_ObjRTreeData *rtdata;
    COMPS_Object *new_data;

    rt1->subnodes = comps_hslist_create();
    comps_hslist_init(rt1->subnodes, NULL, NULL, &comps_objrtree_data_destroy_v);
    if (rt1->subnodes == NULL) {
        COMPS_OBJECT_DESTROY(rt1);
        return;
    }
    rt1->len = 0;

    to_clone = comps_hslist_create();
    comps_hslist_init(to_clone, NULL, NULL, NULL);

    for (it = rt2->subnodes->first; it != NULL; it = it->next) {
        rtdata = comps_objrtree_data_create(
                                    ((COMPS_ObjRTreeData*)it->data)->key, NULL);
        if (((COMPS_ObjRTreeData*)it->data)->data != NULL)
            new_data = comps_object_copy(((COMPS_ObjRTreeData*)it->data)->data);
        else
            new_data = NULL;
        comps_hslist_destroy(&rtdata->subnodes);
        rtdata->subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;
        rtdata->data = new_data;
        comps_hslist_append(rt1->subnodes, rtdata, 0);
        comps_hslist_append(to_clone, rtdata, 0);
    }

    while (to_clone->first) {
        it2 = to_clone->first;
        tmplist = ((COMPS_ObjRTreeData*)it2->data)->subnodes;
        comps_hslist_remove(to_clone, to_clone->first);

        new_subnodes = comps_hslist_create();
        comps_hslist_init(new_subnodes, NULL, NULL, &comps_objrtree_data_destroy_v);
        for (it = tmplist->first; it != NULL; it = it->next) {
            rtdata = comps_objrtree_data_create(
                                      ((COMPS_ObjRTreeData*)it->data)->key, NULL);
            if (((COMPS_ObjRTreeData*)it->data)->data != NULL)
                new_data = comps_object_copy(((COMPS_ObjRTreeData*)it->data)->data);
            else
                new_data = NULL;
            comps_hslist_destroy(&rtdata->subnodes);
            rtdata->subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;
            rtdata->data = new_data;
            comps_hslist_append(new_subnodes, rtdata, 0);
            comps_hslist_append(to_clone, rtdata, 0);
        }
        ((COMPS_ObjRTreeData*)it2->data)->subnodes = new_subnodes;
        free(it2);
    }
    comps_hslist_destroy(&to_clone);

}
COMPS_COPY_u(objrtree, COMPS_ObjRTree) /*comps_utils.h macro*/

void comps_objrtree_values_walk(COMPS_ObjRTree * rt, void* udata,
                              void (*walk_f)(void*, COMPS_Object*)) {
    COMPS_HSList *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it;
    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, NULL);
    comps_hslist_append(tmplist, rt->subnodes, 0);
    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = (COMPS_HSList*)it->data;
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_ObjRTreeData*)it->data)->subnodes->first) {
                comps_hslist_append(tmplist,
                                    ((COMPS_ObjRTreeData*)it->data)->subnodes, 0);
            }
            if (((COMPS_ObjRTreeData*)it->data)->data != NULL) {
               walk_f(udata, ((COMPS_ObjRTreeData*)it->data)->data);
            }
        }
    }
    comps_hslist_destroy(&tmplist);
}

char comps_objrtree_paircmp(void *obj1, void *obj2) {
    if (strcmp(((COMPS_ObjRTreePair*)obj1)->key,
               ((COMPS_ObjRTreePair*)obj2)->key) != 0)
        return 0;
    return comps_object_cmp(((COMPS_ObjRTreePair*)obj1)->data,
                            ((COMPS_ObjRTreePair*)obj1)->data);
}


signed char comps_objrtree_cmp(COMPS_ObjRTree *ort1, COMPS_ObjRTree *ort2) {
    COMPS_HSList *values1, *values2;
    signed char ret;
    values1 = comps_objrtree_pairs(ort1);
    values2 = comps_objrtree_pairs(ort2);
    
    ret = comps_hslist_values_equal(values1, values2, &comps_objrtree_paircmp);
    comps_hslist_destroy(&values1);
    comps_hslist_destroy(&values2);
    return ret;
}
COMPS_CMP_u(objrtree, COMPS_ObjRTree)

void comps_objrtree_set_x(COMPS_ObjRTree *rt, char *key, COMPS_Object *data) {
    __comps_objrtree_set(rt, key, data);
}
void comps_objrtree_set(COMPS_ObjRTree *rt, char *key, COMPS_Object *data) {
    COMPS_Object *ndata = ndata = comps_object_copy(data);
    __comps_objrtree_set(rt, key, ndata);
}


void __comps_objrtree_set(COMPS_ObjRTree *rt, char *key, COMPS_Object *ndata) {

    static COMPS_HSListItem *it;
    COMPS_HSList *subnodes;
    COMPS_ObjRTreeData *rtd;
    static COMPS_ObjRTreeData *rtdata;

    unsigned int len, offset=0;
    unsigned x, found = 0;
    char ended, tmpch;

    len = strlen(key);

    if (rt->subnodes == NULL)
        return;

    subnodes = rt->subnodes;
    while (offset != len)
    {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_ObjRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
        if (!found) { // not found in subnodes; create new subnode
            rtd = comps_objrtree_data_create(key+offset, ndata);
            comps_hslist_append(subnodes, rtd, 0);
            return;
        } else {
            rtdata = (COMPS_ObjRTreeData*)it->data;
            ended = 0;
            for (x=1; ;x++) {
                if (rtdata->key[x] == 0) ended += 1;
                if (x == len - offset) ended += 2;
                if (ended != 0) break;
                if (key[offset+x] != rtdata->key[x]) break;
            }
            if (ended == 3) { //keys equals; data replacement
                comps_object_destroy(rtdata->data);
                rtdata->data = ndata;
                return;
            } else if (ended == 2) { //global key ends first; make global leaf
                comps_hslist_remove(subnodes, it);
                it->next = NULL;
                rtd = comps_objrtree_data_create(key, ndata);
                comps_hslist_append(subnodes, rtd, 0);
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->last = it;
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->first = it;
                len = strlen(key + offset);

                memmove(rtdata->key,rtdata->key+strlen(key+offset),
                                    strlen(rtdata->key) - len);
                rtdata->key[strlen(rtdata->key) - len] = 0;
                rtdata->key = realloc(rtdata->key, sizeof(char)* (strlen(rtdata->key)+1));
                return;
            } else if (ended == 1) { //local key ends first; go deeper
                subnodes = rtdata->subnodes;
                offset += x;
            } else {
                comps_hslist_remove(subnodes, it);  //remove old node
                tmpch = rtdata->key[x];             // split mutual key
                rtdata->key[x] = 0;
                /* insert new parent node with mutual key part */
                rtd = comps_objrtree_data_create(rtdata->key, NULL);

                comps_hslist_append(subnodes, rtd, 0);


                rtd = comps_objrtree_data_create(key+offset+x, ndata);
                comps_hslist_append(
                            ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes,
                            rtd, 0);

                it->next = NULL;
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->last->next = it;
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->last = it;
                rtdata->key[x] = tmpch;
                len = strlen(rtdata->key+x);
                memmove(rtdata->key,rtdata->key+x, sizeof(char)*len);
                rtdata->key = realloc(rtdata->key, sizeof(char)*(len+1));
                rtdata->key[len] = 0;
                return;
            }
        }
    }
}

void comps_objrtree_set_n(COMPS_ObjRTree * rt, char * key,
                       unsigned int len, COMPS_Object * data)
{
    static COMPS_HSListItem *it;
    COMPS_HSList *subnodes;
    COMPS_ObjRTreeData *rtd;
    static COMPS_ObjRTreeData *rtdata;

    unsigned int klen, offset=0;
    unsigned x, found = 0;
    COMPS_Object *ndata;
    char ended, tmpch;

    if (rt->subnodes == NULL)
        return;
    ndata = comps_object_copy(data);

    subnodes = rt->subnodes;
    while (offset != len)
    {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_ObjRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
        if (!found) { // not found in subnodes; create new subnode
            rtd = comps_objrtree_data_create_n(key+offset, len, ndata);
            comps_hslist_append(subnodes, rtd, 0);
            return;
        } else {
            rtdata = (COMPS_ObjRTreeData*)it->data;
            ended = 0;
            for (x=1; ;x++) {
                if (rtdata->key[x] == 0) ended += 1;
                if (x == len-offset) ended += 2;
                if (ended != 0) break;
                if (key[offset+x] != rtdata->key[x]) break;
            }
            if (ended == 3) { //keys equals; data replacement
                comps_object_destroy(rtdata->data);
                return;
            } else if (ended == 2) { //global key ends first; make global leaf
                comps_hslist_remove(subnodes, it);
                it->next = NULL;
                rtd = comps_objrtree_data_create_n(key, len, ndata);
                comps_hslist_append(subnodes, rtd, 0);
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->last = it;
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->first = it;
                klen = len-offset;
                memmove(rtdata->key,rtdata->key+(klen-offset), sizeof(char)*klen);
                rtdata->key[len] = 0;
                return;
            } else if (ended == 1) { //local key ends first; go deeper
                subnodes = rtdata->subnodes;
                offset += x;
            } else {
                comps_hslist_remove(subnodes, it);
                tmpch = rtdata->key[x];
                rtdata->key[x] = 0;
                rtd = comps_objrtree_data_create(rtdata->key, NULL);
                comps_hslist_append(subnodes, rtd, 0);
                rtd = comps_objrtree_data_create_n(key+offset+x,
                                                 len-offset-x, ndata);
                comps_hslist_append(
                            ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes,
                            rtd, 0);
                rtdata->key[x] = tmpch;
                it->next = NULL;
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->last->next = it;
                ((COMPS_ObjRTreeData*)subnodes->last->data)->subnodes->last = it;
                klen = strlen(rtdata->key+x);
                memmove(rtdata->key,rtdata->key+x, sizeof(char)*len);
                rtdata->key = realloc(rtdata->key, sizeof(char)*(len+1));
                rtdata->key[len] = 0;
                return;
            }
        }
    }
}

COMPS_Object* comps_objrtree_get(COMPS_ObjRTree * rt, const char * key) {
    COMPS_HSList * subnodes;
    COMPS_HSListItem * it = NULL;
    COMPS_ObjRTreeData * rtdata;
    unsigned int offset, len, x;
    char found, ended;

    len = strlen(key);
    offset = 0;
    subnodes = rt->subnodes;
    while (offset != len) {
        found = 0;
        for (it = subnodes->first; it != NULL; it=it->next) {
            if (((COMPS_ObjRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
        if (!found)
            return NULL;
        rtdata = (COMPS_ObjRTreeData*)it->data;

        for (x=1; ;x++) {
            ended=0;
            if (x == strlen(rtdata->key)) ended += 1;
            if (x == len-offset) ended += 2;
            if (ended != 0) break;
            if (key[offset+x] != rtdata->key[x]) break;
        }
        if (ended == 3) return rtdata->data;
        else if (ended == 1) offset+=x;
        else return NULL;
        subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;
    }
    if (it != NULL)
        return comps_object_copy(((COMPS_ObjRTreeData*)it->data)->data);
    else return NULL;
}

void comps_objrtree_unset(COMPS_ObjRTree * rt, const char * key) {
    COMPS_HSList * subnodes;
    COMPS_HSListItem * it;
    COMPS_ObjRTreeData * rtdata;
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
            if (((COMPS_ObjRTreeData*)it->data)->key[0] == key[offset]) {
                found = 1;
                break;
            }
        }
        if (!found)
            return;
        rtdata = (COMPS_ObjRTreeData*)it->data;

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
                //printf("removing all\n");
                comps_hslist_remove(subnodes, it);
                comps_objrtree_data_destroy(rtdata);
                free(it);
            }
            else {
                //printf("removing data only\n");
                comps_object_destroy(rtdata->data);
                rtdata->is_leaf = 0;
                rtdata->data = NULL;
            }

            if (path->last == NULL) {
                comps_hslist_destroy(&path);
                return;
            }
            rtdata = (COMPS_ObjRTreeData*)
                     ((struct Relation*)path->last->data)->child_it->data;

            /*remove all predecessor of deleted node (recursive) with no childs*/
            while (rtdata->subnodes->last == NULL) {
                //printf("removing '%s'\n", rtdata->key);
                comps_objrtree_data_destroy(rtdata);
                comps_hslist_remove(
                            ((struct Relation*)path->last->data)->parent_nodes,
                            ((struct Relation*)path->last->data)->child_it);
                free(((struct Relation*)path->last->data)->child_it);
                it = path->last;
                comps_hslist_remove(path, path->last);
                free(it);
                rtdata = (COMPS_ObjRTreeData*)
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
        subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;
        relation->parent_nodes = subnodes;
        relation->child_it = it;
        comps_hslist_append(path, (void*)relation, 0);
    }
    comps_hslist_destroy(&path);
    return;
}

void comps_objrtree_clear(COMPS_ObjRTree * rt) {
    COMPS_HSListItem *it, *oldit;
    if (rt==NULL) return;
    if (rt->subnodes == NULL) return;
    oldit = rt->subnodes->first;
    it = (oldit)?oldit->next:NULL;
    for (;it != NULL; it=it->next) {
        comps_object_destroy(oldit->data);
        free(oldit);
        oldit = it;
    }
    if (oldit) {
        comps_object_destroy(oldit->data);
        free(oldit);
    }
}

inline COMPS_HSList* __comps_objrtree_all(COMPS_ObjRTree * rt, char pairorkey) {
    COMPS_HSList *tmplist, *tmp_subnodes, *ret;
    COMPS_HSListItem *it;
    struct Pair {
        COMPS_HSList * subnodes;
        char * key;
        char added;
    } *pair, *parent_pair;
    COMPS_ObjRTreePair *rtpair;

    pair = malloc(sizeof(struct Pair));
    pair->subnodes = rt->subnodes;
    pair->key = NULL;
    pair->added = 0;

    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, &free);
    ret = comps_hslist_create();
    if (pairorkey == 1)
        comps_hslist_init(ret, NULL, NULL, &free);
    else
        comps_hslist_init(ret, NULL, NULL, &comps_objrtree_pair_destroy_v);

    comps_hslist_append(tmplist, pair, 0);

    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = ((struct Pair*)it->data)->subnodes;
        parent_pair = (struct Pair*) it->data;
        free(it);

        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            pair = malloc(sizeof(struct Pair));
            pair->subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;
            pair->added = 0;

            if (parent_pair->key != NULL) {
                pair->key =
                    malloc(sizeof(char)
                           * (strlen(((COMPS_ObjRTreeData*)it->data)->key)
                           + strlen(parent_pair->key) + 1));
                memcpy(pair->key, parent_pair->key,
                       sizeof(char) * strlen(parent_pair->key));
                memcpy(pair->key+strlen(parent_pair->key),
                       ((COMPS_ObjRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_ObjRTreeData*)it->data)->key)+1));
            } else {
                pair->key = malloc(sizeof(char)*
                                (strlen(((COMPS_ObjRTreeData*)it->data)->key) +
                                1));
                memcpy(pair->key, ((COMPS_ObjRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_ObjRTreeData*)it->data)->key)+1));
            }
            /* current node has data */
            if (((COMPS_ObjRTreeData*)it->data)->data != NULL) {
                if (pairorkey == 1) {
                    comps_hslist_append(ret, pair->key, 0);
                } else {
                    rtpair = malloc(sizeof(COMPS_ObjRTreePair));
                    rtpair->key = pair->key;
                    rtpair->data = ((COMPS_ObjRTreeData*)it->data)->data;
                    comps_hslist_append(ret, rtpair, 0);
                }
                pair->added = 1;
                if (((COMPS_ObjRTreeData*)it->data)->subnodes->first != NULL) {
                    comps_hslist_append(tmplist, pair, 0);
                } else {
                    free(pair);
                }
            /* current node hasn't data */
            } else {
                if (((COMPS_ObjRTreeData*)it->data)->subnodes->first) {
                    comps_hslist_append(tmplist, pair, 0);
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

void comps_objrtree_unite(COMPS_ObjRTree *rt1, COMPS_ObjRTree *rt2) {
    COMPS_HSList *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it;
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
        //printf("key-part:%s\n", parent_pair->key);
        free(it);

        //pair->added = 0;
        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            pair = malloc(sizeof(struct Pair));
            pair->subnodes = ((COMPS_ObjRTreeData*)it->data)->subnodes;

            if (parent_pair->key != NULL) {
                pair->key = malloc(sizeof(char)
                               * (strlen(((COMPS_ObjRTreeData*)it->data)->key)
                               + strlen(parent_pair->key) + 1));
                memcpy(pair->key, parent_pair->key,
                       sizeof(char) * strlen(parent_pair->key));
                memcpy(pair->key + strlen(parent_pair->key),
                       ((COMPS_ObjRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_ObjRTreeData*)it->data)->key)+1));
            } else {
                pair->key = malloc(sizeof(char)*
                                (strlen(((COMPS_ObjRTreeData*)it->data)->key) +1));
                memcpy(pair->key, ((COMPS_ObjRTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_ObjRTreeData*)it->data)->key)+1));
            }
            /* current node has data */
            if (((COMPS_ObjRTreeData*)it->data)->data != NULL) {
                    comps_objrtree_set(rt1, pair->key,
                                       comps_object_copy(
                                      (((COMPS_ObjRTreeData*)it->data)->data)));
            }
            if (((COMPS_ObjRTreeData*)it->data)->subnodes->first) {
                comps_hslist_append(tmplist, pair, 0);
            } else {
                free(pair->key);
                free(pair);
            }
        }
        free(parent_pair->key);
        free(parent_pair);
    }
    comps_hslist_destroy(&tmplist);
}

COMPS_ObjRTree* comps_objrtree_union(COMPS_ObjRTree *rt1, COMPS_ObjRTree *rt2){
    COMPS_ObjRTree *ret;
    ret = comps_objrtree_clone(rt2);
    comps_objrtree_unite(ret, rt1);
    return ret;
}


COMPS_HSList* comps_objrtree_keys(COMPS_ObjRTree * rt) {
    return __comps_objrtree_all(rt, 1);
}

COMPS_HSList* comps_objrtree_pairs(COMPS_ObjRTree * rt) {
    return __comps_objrtree_all(rt, 0);
}

inline void comps_objrtree_pair_destroy(COMPS_ObjRTreePair * pair) {
    free(pair->key);
    free(pair);
}

inline void comps_objrtree_pair_destroy_v(void * pair) {
    free(((COMPS_ObjRTreePair *)pair)->key);
    free(pair);
}

COMPS_ObjectInfo COMPS_ObjRTree_ObjInfo = {
    .obj_size = sizeof(COMPS_ObjRTree),
    .constructor = &comps_objrtree_create_u,
    .destructor = &comps_objrtree_destroy_u,
    .copy = &comps_objrtree_copy_u,
    .obj_cmp = &comps_objrtree_cmp_u
};
