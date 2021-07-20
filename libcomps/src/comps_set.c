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

#include "comps_set.h"
#include <stdlib.h>
#include <string.h>

char comps_set_index_cmp(void *item1, void *item2) {
    return *(unsigned int*)item1 == *(unsigned int*)item2;
}
void* comps_set_index_clone(void *item) {
    void * ret;
    if ((ret = malloc(sizeof(unsigned int))) == NULL)
        return NULL;
    memcpy(ret, item, sizeof(unsigned int));
    return ret;
}

COMPS_Set * comps_set_create()
{
    COMPS_Set *ret;
    if ((ret = malloc(sizeof(COMPS_Set))) == NULL) {
        return NULL;
    }
    ret->data = comps_hslist_create();
    if (ret->data == NULL) {
        free(ret);
        return NULL;
    }
    return ret;
}

void comps_set_destroy(COMPS_Set **set) {
    comps_hslist_destroy(&(*set)->data);
    free(*set);
    *set = NULL;
}

inline void comps_set_destroy_v(void *set) {
    comps_hslist_destroy(&((COMPS_Set*)set)->data);
    free((COMPS_Set*)set);
}

void comps_set_init(COMPS_Set * set, void* (*data_constructor)(void*),
                                     void* (*data_cloner)(void*),
                                     void (*data_destructor)(void*),
                                     char (*eqf)(void*, void*)) {
    if (set == NULL)
        return;
    set->data_constructor = data_constructor;
    set->data_destructor = data_destructor;
    set->data_cloner = data_cloner;
    set->eqf = eqf;
    comps_hslist_init(set->data, data_constructor, data_cloner, data_destructor);
}

char comps_set_in(COMPS_Set * set, void * item) {
    COMPS_HSListItem * it;
    for (it = set->data->first; it != NULL; it = it->next) {
        if (set->eqf(it->data, item))
            return 1;
    }
    return 0;
}

int comps_set_at(COMPS_Set * set, void * item) {
    COMPS_HSListItem * it;
    int x;
    for (x=0, it = set->data->first; it != NULL; it = it->next, x++) {
        if (set->eqf(it->data, item)) {
            return x;
        }
    }
    return -1;
}

void* comps_set_data_at(COMPS_Set * set, void * item) {
    COMPS_HSListItem * it;
    for (it = set->data->first; it != NULL; it = it->next) {
        if (set->eqf(it->data, item)) {
            return it->data;
        }
    }
    return NULL;
}

char comps_set_add(COMPS_Set * set, void *item) {
    COMPS_HSListItem * it;

    for (it = set->data->first; it != NULL; it = it->next) {
        if (set->eqf(it->data, item)) {
            return 0;
        }
    }
    comps_hslist_append(set->data, item, 1);
    return 1;
}

void* comps_set_remove(COMPS_Set *set, void *item) {
    if (set && set->data) {
        void * ret;
        COMPS_HSListItem * it;
        for (it = set->data->first; it != NULL; it = it->next) {
            if (set->eqf(it->data, item)) {
                comps_hslist_remove(set->data, it);
                ret = it->data;
                free(it);
                return ret;
            }
        }
    }
    return NULL;
}

inline char comps_set_is_empty(COMPS_Set *set) {
    if (set->data->first == NULL) {
        return 1;
    } else return 0;
}

char comps_set_cmp(COMPS_Set *set1, COMPS_Set *set2) {
    unsigned int x, index;
    COMPS_Set *not_processed1, *not_processed2;
    COMPS_HSListItem *it;
    void *item;//, *tmpdata;
    char ret;
    int at;

    not_processed1 = comps_set_create();
    not_processed2 = comps_set_create();
    comps_set_init(not_processed1, &comps_set_index_clone,
                                   &comps_set_index_clone,
                                   &free, &comps_set_index_cmp);
    comps_set_init(not_processed2, &comps_set_index_clone,
                                   &comps_set_index_clone,
                                   &free, &comps_set_index_cmp);
    for (x = 0, it = set1->data->first; it!= NULL; it = it->next, x++) {
        comps_hslist_append(not_processed1->data, &x, 1);
    }
    for (x = 0, it = set2->data->first; it!= NULL; it = it->next, x++) {
        comps_hslist_append(not_processed2->data, &x, 1);
    }
    it = set1->data->first;

    index = 0;
    ret = 0;
    while (!comps_set_is_empty(not_processed1) &&
           !comps_set_is_empty(not_processed2)) {
        if ((at = comps_set_at(set2, it->data)) != -1) {
            item = comps_set_remove(not_processed1, (void*)&index);
            free(item);
            item = comps_set_remove(not_processed2 , (void*)&at);
            free(item);
            //tmpdata = comps_hslist_data_at(set2->data, at);
        } else {
            ret = 2;
            break;
        }
        it = it->next;
        index++;
        if (it == NULL) break;
    }
    if (ret == 0) {
        if (comps_set_is_empty(not_processed1)) {
            /*if (comps_set_is_empty(not_processed2))
                ret = 0;*/
            if (!comps_set_is_empty(not_processed2))
                ret = -1;
        } else {
            if (comps_set_is_empty(not_processed2))
                ret = 1;
            else ret = 2;
        }
    }
    comps_set_destroy(&not_processed1);
    comps_set_destroy(&not_processed2);
    return ret;
}

inline void comps_set_clear(COMPS_Set *set) {
    comps_hslist_clear(set->data);
}

