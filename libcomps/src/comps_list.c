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
 * along with this program; if not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 */

#include "comps_list.h"
#include <stdio.h>

/** COMPS_List constructor
 * @return COMPS_List object
 */
COMPS_List * comps_list_create(){
    return malloc(sizeof(COMPS_List));
}

void comps_list_destroy_v(void *dl)
{
    comps_list_destroy((COMPS_List**)&dl);
}

/** COMPS_List destructor
 * destroy the list and set it's reference to NULL
 * @param dl reference to COMPS_List object
 */
void comps_list_destroy(COMPS_List ** dl)
{
    COMPS_ListItem *it,*oldit;
    if (*dl == NULL) return;

    oldit = (*dl)->first;
    it = (oldit != NULL)?oldit->next:NULL;

    for (;it != NULL; it=it->next) {
        comps_list_item_destroy(oldit);
        oldit = it;
    }
    comps_list_item_destroy(oldit);
    free(*dl);
    *dl = NULL;
}
/**
 * Clear the list (destroy items) and reinit list
 * @param dl COMPS_List object
 */
void comps_list_clear(COMPS_List * dl)
{
    COMPS_ListItem *it, *oldit;
    if (dl == NULL) return;

    oldit = dl->first;
    it = (oldit != NULL)?oldit->next:NULL;

    for (;it != NULL; it=it->next) {
        comps_list_item_destroy(oldit);
        oldit = it;
    }
    comps_list_item_destroy(oldit);
    dl->first = NULL;
    dl->last = NULL;
    dl->len = 0;
}

/**
 * COMPS_ListItem constructor
 * @param data useful data in item
 * @param data_constructor pointer to function creating data from existing
 * data object
 * @param data_destructor pointer to function called when destroying/clearing
 * list item/list
 * @return COMPS_ListItem object
 */
COMPS_ListItem* comps_list_item_create(void * data,
                                     void* (*data_constructor)(void * data),
                                     void (*data_destructor)(void * data))
{
    COMPS_ListItem * item;
    if ((item = malloc(sizeof(COMPS_ListItem)))==NULL) {
        return NULL;
    }
    item->next = NULL;
    item->prev = NULL;
    if (data_constructor != NULL)
        item->data = data_constructor(data);
    else
        item->data = data;

    item->data_destructor = data_destructor;
    return item;
}

/**
 * COMPS_ListItem destructor (destroy item and its data)
 * @param item COMPS_ListItem object
 */
void comps_list_item_destroy(COMPS_ListItem * item)
{
    if (item == NULL)
        return;
    if (item->data_destructor != NULL) {
        item->data_destructor(item->data);
    }
    free(item);
}

/**
 * Init the list. Set first and last item to NULL and zero list len
 * @param @dl COMPS_List object
 * @return 1 if initialization was successful 0 otherwise
 */
unsigned comps_list_init(COMPS_List * dl)
{
    if (dl == NULL)
        return 1;
    dl->first = NULL;
    dl->last = NULL;
    dl->len = 0;
    return 0;
}

/**
 * Insert list item to specified position to list. If position doesn't exists
 * (list has'nt enough items) do nothing
 * @param dl COMPS_List object
 * @param di COMPS_ListItem object
 * @param pos position to insert in
 */
void comps_list_insert(COMPS_List * dl, COMPS_ListItem * di, unsigned int pos)
{
    COMPS_ListItem * it;
    unsigned int index=0;
    if ((dl == NULL) || (di == NULL))
        return;

    it = dl->first;
    if (it == NULL) {
        if (pos == 0) {
            dl->first = di;
            dl->last = di;
            dl->len++;
            return;
        }
        return;
    }

    if (pos != 0) {
        for (it = dl->first;
             index != pos-1 && it != NULL;
             it=it->next, index++);
        if (it == NULL)
            return;
        di->prev = it;
        di->next = it->next;
        it->next = di;
        if (di->next)
            di->next->prev = di;
        dl->len++;
    } else
    {
        di->prev = NULL;
        di->next = dl->first;
        dl->first->prev = di;
        dl->first = di;
        dl->len++;
    }
}

/**
 * Append item to the end of list
 * @param dl COMPS_List object
 * @param di COMPS_ListItem object
 * @return 1 if append was successful 0 otherwise
 */
char comps_list_append(COMPS_List * dl, COMPS_ListItem * di)
{
    if ((dl == NULL) || (di == NULL))
        return 0;

    if (dl->last == NULL)
    {
        dl->last = di;
        dl->first = di;
    } else {
        dl->last->next = di;
        di->prev = dl->last;
        dl->last = di;
    }
    dl->len++;
    return 1;
}

/** Remove and destroy item at specified pos
 * @param dl COMPS_List object
 * @param pos item position in list
 */
void comps_list_remove_pos(COMPS_List * dl, unsigned int pos)
{
    COMPS_ListItem *it, *tmpit;
    unsigned int index=0;

    if (dl == NULL) return;

    it = dl->first;
    if (it == NULL) return;

    if (pos != 0) {
        for (it = dl->first;
             index != pos-1 && it != NULL;
             it=it->next, index++);

        if (it == NULL) return;

        tmpit = it->next;
        if (tmpit != NULL)
            it->next = tmpit->next;
        else
            it->next = NULL;
        if (tmpit != NULL && tmpit->next != NULL)
            tmpit->next->prev = it;
        if ((void*)tmpit == dl->last)
            dl->last = (void*)it;
        comps_list_item_destroy(tmpit);
        dl->len--;
    } else {
        tmpit = dl->first;
        if ((void*)tmpit == dl->last) {
            dl->len--;
            comps_list_item_destroy(tmpit);
            dl->first = NULL;
            dl->last = NULL;
            return;
        }
        tmpit->next->prev = NULL;
        dl->first = tmpit->next;
        comps_list_item_destroy(tmpit);
        dl->len--;
    }
}

COMPS_ListItem* comps_list_remove_pos_r(COMPS_List * dl, unsigned int pos)
{
    COMPS_ListItem *it, *tmpit;
    unsigned int index=0;

    if (dl == NULL) return NULL;

    it = dl->first;
    if (it == NULL) return NULL;

    if (pos != 0) {
        for (it = dl->first;
             index != pos-1 && it != NULL;
             it=it->next, index++);

        if (it == NULL) return NULL;

        tmpit = it->next;
        if (tmpit != NULL)
            it->next = tmpit->next;
        else
            it->next = NULL;
        if (tmpit != NULL && tmpit->next != NULL)
            tmpit->next->prev = it;
        if ((void*)tmpit == dl->last)
            dl->last = (void*)it;
        dl->len--;
        return tmpit;
    } else {
        tmpit = dl->first;
        if ((void*)tmpit == dl->last) {
            dl->len--;
            dl->first = NULL;
            dl->last = NULL;
            return tmpit;
        }
        tmpit->next->prev = NULL;
        dl->first = tmpit->next;
        dl->len--;
        return tmpit;
    }
    return NULL;
}

/**
 * Remove first occurence of data in list
 * @param dl COMPS_List object
 * @param data removed data
 * @return 1 if removing was successfull 0 otherwise
 */
char comps_list_remove_data(COMPS_List *dl, void * data) {
    COMPS_ListItem * it;
    for (it = dl->first; it != NULL; it = it->next) {
        if (it->data == data) break;
    }
    if (it != NULL) {
        if (it == dl->first) {
            dl->first = it->next;
            if (it->next)
                it->next->prev = it;
        }
        if (it == dl->last) {
            dl->last = it->prev;
            if (it->prev)
                it->prev->next = NULL;
        }
        free(it);
        dl->len--;
        return 1;
    }
    return 0;
}

/**
 * Remove specified list item form list(Do not destroying it)
 * @param dl COMPS_List object
 * @param it COMPS_ListItem object
 */
void comps_list_remove_item(COMPS_List * dl, COMPS_ListItem *it) {
    COMPS_ListItem * tmpit;
    int pos;
    if (dl == NULL) return;

    if (dl->first == dl->last && dl->first == it) {
        dl->first = NULL;
        dl->last = NULL;
        dl->len = 0;
        return;
    }
    pos = 0;
    for (tmpit = dl->first; tmpit != it && tmpit != NULL; tmpit = tmpit->next) pos++;
    if (!tmpit) return;

    if (tmpit->prev != NULL) {
        if (tmpit->next != NULL) {
            tmpit->prev->next = tmpit->next;
        } else {
            tmpit->prev->next = NULL;
        }
    } else dl->first = tmpit->next;

    if (tmpit->next) {
        tmpit->next->prev = tmpit->prev;
    } else dl->last = tmpit->prev;
    dl->len--;
}

/**
 * Remove and return last item of list(do not destroying it)
 * @param dl COMPS_List object
 * @return COMPS_ListItem object or NULL if list is empty
 */
COMPS_ListItem * comps_list_pop(COMPS_List * dl)
{
    COMPS_ListItem * it;
    if (dl == NULL || dl->last == NULL)
        return NULL;
    it =  dl->last;
    if (dl->last->prev != NULL) {
        dl->last->prev->next = NULL;
        dl->last = dl->last->prev;
    } else {
        dl->first = NULL;
        dl->last = NULL;
    }
    dl->len--;
    return it;
}

/**
 * Remove and return first item of list(do not destroing it)
 * @param dl COMPS_List object
 * @return COMPS_ListItem object or NULL if list is empty
 */
COMPS_ListItem * comps_list_shift(COMPS_List * dl)
{
    COMPS_ListItem * it;
    if (dl == NULL || dl->first == NULL)
        return NULL;
    it = dl->first;
    if (dl->first->next != NULL) {
        dl->first->next->prev = NULL;
        dl->first = dl->first->next;
    } else {
        dl->first = NULL;
        dl->last = NULL;
    }
    dl->len--;
    return it;
}

/**
 * Apply func to all items in list
 * @param dl COMPS_List object
 * @param func pointer to user specified function
 * @param user_data user data passed to func function
 */
void comps_list_walk(COMPS_List * dl, COMPS_ListWalkFunc func, void * user_data)
{
    COMPS_ListItem *it;
    unsigned int index=0;
    if (dl == NULL)
        return;
    for (it = dl->first; it != NULL; it=it->next, index++)
        func(it, index, user_data);
}

/**
 * Return list item at specified position
 * @param dl COMPS_List object
 * @param pos specified position
 * @return COMPS_ListItem object or NULL if position doesn't exist in list
 */
COMPS_ListItem * comps_list_at(COMPS_List* dl, unsigned int pos) {
    COMPS_ListItem *it = NULL;
    unsigned int index=0;
    for (it = dl->first;
         it != NULL && index<pos;
         it = it->next, index++);
    return it;
}
