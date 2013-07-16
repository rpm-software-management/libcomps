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

/*! \file comps_list.h
 * \brief A Documented file.
 * Details.
 * */
#ifndef COMPS_LIST_H
#define COMPS_LIST_H

#include <string.h>
#include <stdlib.h>

/**
 * COMPS_ListItem
 * @see comps_list_item_create
 * @see comps_list_item_destroy
*/
struct COMPS_ListItem {
/** pointer to next item in list*/
struct COMPS_ListItem * next;
/** pointer to previous item in list*/
struct COMPS_ListItem * prev;
/** usefull userdata */
void * data;
/** data destructor */
void (*data_destructor)(void * data);
};
typedef struct COMPS_ListItem COMPS_ListItem;

/** comps_list_walk callback function
@param COMPS_ListItem curently processed list item
@param "unsigned int" curently processed item position in list
@param user_data passed user data from comps_list_walk
*/
typedef void (*COMPS_ListWalkFunc)(
                        COMPS_ListItem*,
                        unsigned int,
                        void * user_data);
/**
* COMPS List
* \par COMPS_List create/destroy
* @see comps_list_create
* @see comps_list_init
* @see comps_list_destroy
* @see comps_list_clear
* \par COMPS_List item adding
* @see comps_list_insert
* @see comps_list_append
* \par COMPS_List item removing
* @see comps_list_remove_pos
* @see comps_list_remove_item
* @see comps_list_remove_data
* @see comps_list_pop
* @see comps_list_shift
* \par COMPS_List item manipulation
* @see comps_list_at
* @see comps_list_walk
*/
typedef struct {
COMPS_ListItem * first;
COMPS_ListItem * last;
int len;
} COMPS_List;

COMPS_ListItem * comps_list_item_create(void * data,
                                 void* (*data_constructor)(void * data),
                                 void (*data_destructor)(void * data));
void comps_list_item_destroy(COMPS_ListItem * item);

COMPS_List * comps_list_create();
unsigned comps_list_init(COMPS_List * dl);
void comps_list_destroy(COMPS_List ** dl);
void comps_list_destroy_v(void *dl);
void comps_list_clear(COMPS_List * dl);

void comps_list_insert(COMPS_List * dl, COMPS_ListItem * di, unsigned int pos);
char comps_list_append(COMPS_List * dl, COMPS_ListItem * di);

void comps_list_remove_pos(COMPS_List * dl, unsigned int pos);
COMPS_ListItem* comps_list_remove_pos_r(COMPS_List * dl, unsigned int pos);
void comps_list_remove_item(COMPS_List * dl, COMPS_ListItem *it);
char comps_list_remove_data(COMPS_List *dl, void * data);
COMPS_ListItem * comps_list_pop(COMPS_List * dl);
COMPS_ListItem * comps_list_shift(COMPS_List * dl);

COMPS_ListItem * comps_list_at(COMPS_List* dl, unsigned int pos);
void comps_list_walk(COMPS_List * dl, COMPS_ListWalkFunc func,
                     void * user_data);

#endif
