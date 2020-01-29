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

/*! \file comps_objlist.h
 * \brief A Documented file.
 * Details.
 **/
#ifndef COMPS_OBJLIST_H
#define COMPS_OBJLIST_H

#include "comps_obj.h"

#include <string.h>
#include <stdlib.h>

typedef struct COMPS_ObjListIt COMPS_ObjListIt;

struct COMPS_ObjListIt {
    COMPS_Object *comps_obj;
    COMPS_ObjListIt *next;
};


/** COMPS_Object derivate representing category element in comps.xml structure*/
typedef struct COMPS_ObjList {
    COMPS_Object_HEAD;
    COMPS_ObjListIt *first; /**< first list item iterator */
    COMPS_ObjListIt *last; /**< last list item iterator */
    size_t len; /**< list lenght*/
} COMPS_ObjList;
COMPS_Object_TAIL(COMPS_ObjList);

//void comps_objlist_create(COMPS_ObjList *objlist, COMPS_Object **args);
//void comps_objlist_create_u(COMPS_Object *uobj, COMPS_Object **args);
//void comps_objlist_destroy(COMPS_ObjList *objlist);
//void comps_objlist_destroy_u(COMPS_Object *objlist);


/** Clear the list
 * Remove all items from list and call destructor on each one
 * @param objlist COMPS_ObjList object
 */
void comps_objlist_clear(COMPS_ObjList *objlist);


const COMPS_ObjListIt *comps_objlist_it_next(const COMPS_ObjListIt *it);

/** Traverse the list
 *
 * Start traversing list from walker position to end of list. Each
 * call store actual item object to result parameter and move walker iterator
 * forward
 *
 * @param walker Iterator position in list
 * @param result object of actual item
 * @param return non-zero if walker hasn't reached end, otherwise zero
 */
int comps_objlist_walk(COMPS_ObjListIt **walker, COMPS_Object **result);

/** Traverse the list with sentinel iterator
 *
 * Same as comps_objlist_walk with additional sentinel iterator supplying
 * end of list.
 *
 * @param walker Iterator position in list
 * @param result object of actual item
 * @param return non-zero if walker hasn't reached end, otherwise zero
 *
 */
int comps_objlist_walk_r(COMPS_ObjListIt *walker_start,
                            COMPS_ObjListIt *mantinel,
                            COMPS_Object **result);

/** Append new object to list
 *
 * Does not incremented object's reference counter
 * @param objlist COMPS_ObjList instance
 * @param obj appended object
 */
int comps_objlist_append_x(COMPS_ObjList *objlist, COMPS_Object *obj);

/** Append new object to list
 *
 * This function increment object's reference counter
 * @param objlist COMPS_ObjList instance
 * @param obj appended object
 */
int comps_objlist_append(COMPS_ObjList *objlist, COMPS_Object *obj);

/** Return item's object at specified position
 *
 * Returned object has incremented reference counter
 * @param objlist COMPS_ObjList object
 * @param atpos item's position
 * @return if list has enough items, return item's object, otherwise NULL
 */
COMPS_Object* comps_objlist_get(COMPS_ObjList *objlist, unsigned int atpos);

/** Set item's object at specified positoin
 *
 * set new item to specified position, increment new item's reference counter
 * and destroy old item (decrement reference counter). If list hasn't enough
 * items, returns 0.
 *
 * @param objlist COMPS_ObjList object
 * @param atpos item's position
 * @parma obj new object
 * @return non-zero on success, otherwise returns zero
 */
int comps_objlist_set(COMPS_ObjList *objlist, unsigned int atpos,
                      COMPS_Object *obj);

/** Return item's object at specified position
 *
 * Returned object HASN'T incremented reference count
 * @param objlist COMPS_ObjList object
 * @param atpos item's position
 * @return if list has enough items, return item's object, otherwise NULL
 */
COMPS_Object* comps_objlist_get_x(COMPS_ObjList *objlist, unsigned int atpos);

/** Insert item at specified position
 *
 * If list doesn't have enough items (specified position is greater
 * then items count, even if position is greater only by 1) fails
 * and returns 0
 *
 * @param objlist COMPS_ObjList object
 * @param pos item's position
 * @param obj inserted object
 * @return non-zero if success, zero otherwise 
 */
int comps_objlist_insert_at(COMPS_ObjList *objlist,
                           unsigned int pos,
                           COMPS_Object *obj);

int comps_objlist_insert_at_x(COMPS_ObjList *objlist,
                           unsigned int pos,
                           COMPS_Object *obj);

/** Remove item on specified position from list
 *
 * If list doesn't have enough items fails. On succes decrements item object's
 * reference counter and remove item from list
 *
 * @param objlist COMPS_ObjList object
 * @param atpos item's position
 * @return non-zero if success, zero otherwise 
 */
int comps_objlist_remove_at(COMPS_ObjList *objlist, unsigned int atpos);

/** Remove item with specified object from list
 *
 * Remove first matching items with object pointer eqaul specifed
 * object pointer.
 *
 * @param objlist COMPS_ObjList object
 * @param obj removed object
 * @return non-zero if success, zero otherwise 
 */
int comps_objlist_remove(COMPS_ObjList *objlist, COMPS_Object *obj);


int comps_objlist_index(COMPS_ObjList *objlist, COMPS_Object *obj);

/** Returns new sublist from original list
 *
 * Returns new sublist from original list, starting item startit and ending
 * item end
 *
 * @param startit start item iterator
 * @param end end item iterator
 * @param list COMPS_ObjList instance
 * @return new sublist
 */
COMPS_ObjList* comps_objlist_sublist_it(COMPS_ObjListIt *startit,
                                      COMPS_ObjListIt *end);

/** Returns new sublist from original list
 *
 * Returns new sublist from original list, with items passed through filter
 *
 * @param list COMPS_ObjList instance
 * @param filter_func filter callback
 * @return new sublist
 */
COMPS_ObjList* comps_objlist_filter(COMPS_ObjList *list,
                                  char (*filter_func)(COMPS_Object*));

void comps_objlist_concat_in(COMPS_ObjList *list1, COMPS_ObjList *list2);

//extern COMPS_ObjectInfo COMPS_ObjList_ObjInfo;

#endif
