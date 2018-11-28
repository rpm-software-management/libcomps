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

#include "comps_hslist.h"
#include "comps_set.h"

#include <stdlib.h>
COMPS_HSList * comps_hslist_create() {
    COMPS_HSList *ret;
    ret =  malloc(sizeof(COMPS_HSList));
    if (!ret) {
        return NULL;
    }
    return ret;
}

void comps_hslist_init(COMPS_HSList * hslist,
                            void*(*data_constructor)(void* data),
                            void*(*data_cloner)(void* data),
                            void(*data_destructor)(void* data)) {
    if (hslist == NULL) {
        return;
    }
    hslist->data_constructor = data_constructor;
    hslist->data_destructor = data_destructor;
    hslist->data_cloner = data_cloner;
    hslist->first = NULL;
    hslist->last = NULL;
}

void comps_hslist_append(COMPS_HSList * hslist, void * data, unsigned construct) {
    COMPS_HSListItem * it;

    if (hslist == NULL)
        return;
    if ((it = malloc(sizeof(*it))) == NULL)
        return;
    if (construct && hslist->data_constructor) {
        it->data = hslist->data_constructor(data);
    } else {
        it->data = data;
    }
    it->next = NULL;
    if (hslist->last == NULL) {
        hslist->last = it;
        hslist->first = it;
    } else {
        hslist->last->next = it;
        hslist->last = hslist->last->next;
    }
}

void comps_hslist_insert_after(COMPS_HSList * hslist, COMPS_HSListItem *item,
                               void *data, unsigned construct) {
    COMPS_HSListItem * it;
    void * ndata;

    if (hslist == NULL || item == NULL)
        return;
    if ((it = malloc(sizeof(*it))) == NULL)
        return;
    if (construct && hslist->data_constructor) {
        ndata = hslist->data_constructor(data);
    } else {
        ndata = data;
    }
    it->data = ndata;
    it->next = item->next;
    item->next = it;
    if (item == hslist->last) {
        //printf("inserting after last\n");
        hslist->last = it;
    }
}
int comps_hslist_insert_at(COMPS_HSList * hslist, int pos,
                               void *data, unsigned construct) {
    COMPS_HSListItem *it, *oldit, *newit;
    int n;
    
    if (hslist == NULL)
        return 0;
    if ((newit = malloc(sizeof(*newit))) == NULL)
        return 0;
    if (construct && hslist->data_constructor) {
        newit->data = hslist->data_constructor(data);
    } else {
        newit->data = data;
    }
    oldit=NULL;
    for (n=0, it = hslist->first; n != pos && it != NULL; n++, it = it->next) {
        oldit = it;
    }
    if (n == 0 && pos == 0) {
        newit->next = hslist->first;
        hslist->first = newit;
        if (hslist->last == NULL)
            hslist->last = newit;
    } else if (n == pos) {
        newit->next = oldit->next;
        oldit->next = newit;
    } else {
        if (hslist->data_destructor)
            hslist->data_destructor(newit->data);
        free(newit);
    }
    return 1;
}

void comps_hslist_prepend(COMPS_HSList * hslist, void *data, unsigned construct) {
    COMPS_HSListItem * it;
    void * ndata;

    if (hslist == NULL)
        return;
    if ((it = malloc(sizeof(*it))) == NULL)
        return;
    if (construct && hslist->data_constructor) {
        ndata = hslist->data_constructor(data);
    } else {
        ndata = data;
    }
    it->data = ndata;
    it->next = hslist->first;
    hslist->first = it;
}

void* comps_hslist_shift(COMPS_HSList * hslist) {
    void * data;
    COMPS_HSListItem *it;

    if (hslist == NULL || hslist->first == NULL) {
        //printf("ret null %p\n", hslist->first);
        return NULL;
    }
    it = hslist->first;
    data = hslist->first->data;
    hslist->first = hslist->first->next;
    if (hslist->first == NULL)
        hslist->last=NULL;
    free(it);
    return data;
}

void* comps_hslist_pop(COMPS_HSList * hslist) {
    void * data;
    COMPS_HSListItem *it, *it2;

    if (hslist == NULL || hslist->first == NULL)
        return NULL;
    it2 = NULL;
    for (it = hslist->first; it != hslist->last; it2=it, it = it->next);

    if (!it2) {
        hslist->last = NULL;
        hslist->first = NULL;
    } else {
        hslist->last = it2;
        it2->next = NULL;
    }
    data = it->data;
    free(it);
    return data;
}

inline void comps_hslist_destroy_v(void ** hslist) {
    comps_hslist_destroy((COMPS_HSList**) hslist);
}

void comps_hslist_destroy(COMPS_HSList ** hslist) {
    COMPS_HSListItem *it,*oldit;
    unsigned int x;
    if (*hslist == NULL) return;
    oldit = (*hslist)->first;
    it = (oldit)?oldit->next:NULL;
    for (x=0 ;it != NULL; it=it->next, x++) {
        if ((*hslist)->data_destructor != NULL)
            (*hslist)->data_destructor(oldit->data);
        free(oldit);
        oldit = it;
    }
    if (oldit) {
        if ((*hslist)->data_destructor != NULL)
            (*hslist)->data_destructor(oldit->data);
        free(oldit);
    }
    free(*hslist);
    *hslist = NULL;
}

void comps_hslist_remove(COMPS_HSList * hslist,
                              COMPS_HSListItem * it) {
    COMPS_HSListItem *itx, *itprev=NULL;
    if (it == NULL) return;
    for (itx = hslist->first; itx != NULL && itx != it; itx = itx->next) {
        itprev = itx;
    }
    if (itx != it)
        return;
    if (itprev == NULL) {
        if (hslist->first == hslist->last)
            hslist->last = hslist->first->next;
        hslist->first = hslist->first->next;
    } else {
        itprev->next = it->next;
        if (it == hslist->last) {
            if (hslist->first == hslist->last)
                hslist->first = itprev;
            hslist->last = itprev;
        }
    }
}

void* comps_hslist_data_at(COMPS_HSList * hlist, unsigned int index)
{
    unsigned int i=0;
    COMPS_HSListItem * itx;
    for (itx = hlist->first; itx != NULL && index != i; itx = itx->next, i++);
    if (itx == NULL)
        return itx;
    else
        return itx->data;
}

COMPS_HSList* comps_hslist_clone(COMPS_HSList * hslist) {
    COMPS_HSList *ret;
    COMPS_HSListItem *it;

    ret = comps_hslist_create();
    comps_hslist_init(ret, hslist->data_constructor,
                           hslist->data_cloner,
                           hslist->data_destructor);
    for (it = hslist->first; it != NULL; it = it->next) {
        comps_hslist_append(ret, hslist->data_cloner(it->data), 0);
    }
    return ret;
}

void comps_hslist_clear(COMPS_HSList * hslist) {
    COMPS_HSListItem *it,*oldit;
    if (hslist == NULL) return;
    oldit = hslist->first;
    it = (oldit)?oldit->next:NULL;
    for (;it != NULL; it=it->next) {
        if (hslist->data_destructor != NULL)
            hslist->data_destructor(oldit->data);
        free(oldit);
        oldit = it;
    }
    if (oldit) {
        if (hslist->data_destructor != NULL)
            hslist->data_destructor(oldit->data);
        free(oldit);
    }
    hslist->first = NULL;
    hslist->last = NULL;
}

unsigned comps_hslist_values_equal(COMPS_HSList *hlist1, COMPS_HSList *hlist2,
                                   char (*cmpf)(void*, void*)) {
    COMPS_HSListItem *it, *it2;
    for (it = hlist1->first, it2 = hlist2->first; it != NULL && it2 != NULL;
        it = it->next, it2 = it2->next) {
        if (!cmpf(it->data, it2->data))
            return 0;
    }
    if (it != NULL || it2 != NULL)
        return 0;
    return 1;
}

void comps_hslist_unique(COMPS_HSList *hlist, char (*cmpf)(void*, void*)) {
    COMPS_Set *set;
    COMPS_HSListItem *it, *it2;

    if (hlist == NULL) return;

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, cmpf);
    it2 = NULL;
    for (it = hlist->first; it != NULL; it2 = it, it = it->next) {
        if (!comps_set_in(set, it->data)) {
            comps_set_add(set, it->data);
        } else {
            hlist->data_destructor(it->data);
        }
        free(it2);
    }
    free(it2);
    hlist->first = set->data->first;
    hlist->last = set->data->last;
    free(set->data);
    free(set);
}
