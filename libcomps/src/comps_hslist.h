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


#ifndef COMPS_HSLIST_H
#define COMPS_HSLIST_H


struct _COMPS_HSListItem {
    struct _COMPS_HSListItem * next;
    void * data;
};

typedef struct _COMPS_HSListItem COMPS_HSListItem;

typedef struct {
    COMPS_HSListItem * first;
    COMPS_HSListItem * last;
    void(*data_destructor)(void*);
    void*(*data_cloner)(void*);
    void*(*data_constructor)(void*);
} COMPS_HSList;

COMPS_HSList * comps_hslist_create();
void comps_hslist_destroy(COMPS_HSList ** hlist);
void comps_hslist_destroy_v(void ** hlist);

void comps_hslist_init(COMPS_HSList * hlist,
                       void*(*data_constructor)(void* data),
                       void*(*data_cloner)(void* data),
                       void(*data_destructor)(void* data));
void comps_hslist_append(COMPS_HSList * hlist, void * data,
                                                        unsigned construct);
void comps_hslist_remove(COMPS_HSList * hlist, COMPS_HSListItem * it);
void* comps_hslist_data_at(COMPS_HSList * hlist, unsigned int index);
void comps_hslist_insert_after(COMPS_HSList * hslist, COMPS_HSListItem *item,
                               void *data, unsigned construct);
int comps_hslist_insert_at(COMPS_HSList * hslist, int pos,
                               void *data, unsigned construct);
void comps_hslist_prepend(COMPS_HSList * hslist, void *data, unsigned construct);
void* comps_hslist_shift(COMPS_HSList * hslist);
void* comps_hslist_pop(COMPS_HSList * hslist);

COMPS_HSList* comps_hslist_clone(COMPS_HSList * hslist);
void comps_hslist_clear(COMPS_HSList * hslist);
unsigned comps_hslist_values_equal(COMPS_HSList *hlist1, COMPS_HSList *hlist2,
                                   char (*cmpf)(void*, void*));
void comps_hslist_unique(COMPS_HSList *hslist1, char (*cmpf)(void*, void*));

#endif
