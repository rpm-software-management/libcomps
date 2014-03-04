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

/*! \file comps_objdict.h
 * \brief Libcomps dictionary and multi-dictionary. COMPS_ObjDict and
 * COMPS_ObjMDict are both derivates of COMPS_Object read more
 * \link comps_obj here
 * \endlink
 * @see comps_dict
 * @see comps_multi_dict
 * 
 **/
#ifndef COMPS_OBJDICT_H
#define COMPS_OBJDICT_H

#include "comps_objradix.h"
#include "comps_objmradix.h"

typedef COMPS_ObjRTree COMPS_ObjDict;
COMPS_Object_TAIL(COMPS_ObjDict);

typedef COMPS_ObjMRTree COMPS_ObjMDict;
COMPS_Object_TAIL(COMPS_ObjMDict);

COMPS_ObjDict* comps_objdict_create();
COMPS_ObjMDict* comps_objmdict_create();


void comps_objdict_destroy(COMPS_ObjDict *rt);
void comps_objdict_destroy_v(void *rt);
void comps_objmdict_destroy(COMPS_ObjMDict *rt);
void comps_objmdict_destroy_v(void *rt);
/** \defgroup comps_dict comps dictionary functions
 *@{ @} */
/** \defgroup comps_multi_dict comps multi-dictionary functions
 *@{ @} */

 /** \addtogroup comps_dict
 *@{*/
/** set new item to dictionary
 *
 * if there's already item for specified key in dictionary, old item will
 * be replaced with new item and its reference counter will be decremented.
 * Reference counter of new item will be incremented
 *
 * @param rt COMPS_ObjDict object
 * @param key key for new item
 * @param data new item
 */
void comps_objdict_set(COMPS_ObjDict *rt, char *key, COMPS_Object *data);

/** set new item to dictionary
 *
 * if there's already item for specified key in dictionary, old item will
 * be replaced with new item and its reference counter will be decremented.
 * Reference counter of new items won't be incremented
 *
 * @param rt COMPS_ObjDict object
 * @param key key for new item
 * @param data new item
 */
void comps_objdict_set_x(COMPS_ObjDict *rt, char *key, COMPS_Object *data);

/** same as comps_objdict_set but with key length limited by argument
 *
 * @param rt COMPS_ObjDict object
 * @param key key for new item
 * @param len key length limiter
 * @param data new item
 */
void comps_objdict_set_n(COMPS_ObjDict *rt, char *key, unsigned int len,
                         COMPS_Object *data);
/** @}*/

 /** \addtogroup comps_multi_dict
 *@{*/
/** set new item to multi-dictionary
 *
 * if there's already item for specified key, new item will be added into list
 * for specified key. Reference counter of new item is incremented
 *
 * @param rt COMPS_ObjMDict object
 * @param key key for new item
 * @param data new item
 */
void comps_objmdict_set(COMPS_ObjMDict *rt, char *key, COMPS_Object *data);

/** set new item to multi-dictionary
 *
 * if there's already item for specified key, new item will be added into list
 * for specified key. Reference counter of new item isn't incremented
 *
 * @param rt COMPS_ObjMDict object
 * @param key key for new item
 * @param data new item
 */
void comps_objmdict_set_x(COMPS_ObjMDict *rt, char *key, COMPS_Object *data);

/** same as comps_objmdict_set but with key length limited by argument
 *
 * @param rt COMPS_ObjMDict object
 * @param key key for new item
 * @param len key length limiter
 * @param data new item
 */
void comps_objmdict_set_n(COMPS_ObjMDict *rt, char *key, unsigned int len,
                            COMPS_Object *data);
/** @}*/

 /** \addtogroup comps_dict
 *@{*/

/** get item from dictionary for specified key
 *
 * if there's no such item, return NULL. Item's reference counter will
 * be incremented.
 *
 * @param rt COMPS_ObjDict object
 * @param specified key
 * @return item for key
 */
COMPS_Object* comps_objdict_get(COMPS_ObjDict *rt, const char *key);

/** get item from dictionary for specified key
 *
 * if there's no such item, return NULL. Item's reference counter WON'T
 * be incremented.
 *
 * @param rt COMPS_ObjDict object
 * @param specified key
 * @return item for key
 */
COMPS_Object* comps_objdict_get_x(COMPS_ObjRTree * rt, const char * key);
/** @}*/

 /** \addtogroup comps_multi_dict
 *@{*/
/** get items from multi-dictionary for specified key
 *
 * if there's no such items, return NULL. Item's reference counter WON'T
 * be incremented.
 *
 * @param rt COMPS_ObjDict object
 * @param specified key
 * @return COMPS_ObjList of specified items
 */
COMPS_ObjList * comps_objmdict_get(COMPS_ObjMDict *rt, const char *key);
/** @}*/

/** \addtogroup comps_dict
 *@{*/
/** remove item from dictionary for specified key
 *
 * item's reference counter will be decremented
 *
 * @param rt COMPS_ObjDict object
 * @param key item's key
 */
void comps_objdict_unset(COMPS_ObjDict * rt, const char * key);
/** @}*/

/** \addtogroup comps_multi_dict
 *@{*/
/** remove item from multi-dictionary for specified key
 *
 * item's reference counter will be decremented
 *
 * @param rt COMPS_ObjDict object
 * @param key item's key
 */
void comps_objmdict_unset(COMPS_ObjMDict * rt, const char * key);
/** @}*/

/** \addtogroup comps_dict
 *@{*/
/** remove all items from dictionary
 * @param rt COMPS_ObjDict object
 */
void comps_objdict_clear(COMPS_ObjDict * rt);
/** @}*/

/** \addtogroup comps_multi_dict
 *@{*/
/** remove all items from multi-dictionary
 * @param rt COMPS_ObjDict object
 */
void comps_objmdict_clear(COMPS_ObjMDict * rt);
/** @}*/

/** \addtogroup comps_dict
 *@{*/
/** Return list of all values(items) in dictionary
 * @param rt COMPS_ObjDict objecti
 * @return COMPS_HSList of values
 */
COMPS_HSList * comps_objdict_values(COMPS_ObjDict * rt);
/** @}*/

/** \addtogroup comps_multi_dict
 *@{*/
/** Return list of all values(items) in multi-dictionary
 * @param rt COMPS_ObjDict object
 * @return COMPS_HSList of values
 */
COMPS_HSList * comps_objmdict_values(COMPS_ObjMDict * rt);
/** @}*/

/** \addtogroup comps_dict
 *@{*/
/** Apply function for each item in dictionary
 *
 * Applied function takes user data (udata param) and item as arguments
 *
 * @param rt COMPS_ObjDict object
 * @param udata user data
 * @param walk_f applied function
 *
 */
void comps_objdict_values_walk(COMPS_ObjRTree * rt, void* udata,
                              void (*walk_f)(void*, COMPS_Object*));
/** @}*/

/** \addtogroup comps_multi_dict
 *@{*/
/** Apply function for each item in multi-dictionary
 *
 * Applied function takes user data (udata param) and item as arguments
 *
 * @param rt COMPS_ObjDict object
 * @param udata user data
 * @param walk_f applied function
 *
 */
void comps_objmdict_values_walk(COMPS_ObjMDict *rt, void *udata,
                              void (*walk_f)(void*, void*));
/** @}*/

/** \addtogroup comps_dict
 *@{*/
/** Makes copy of dictionary
 *
 * Items in new dictionary is same items with incremented reference
 * counter only
 *
 * @param rt COMPS_ObjDict object
 * @return new COMPS_ObjDict object
 */
COMPS_ObjDict* comps_objdict_clone(COMPS_ObjDict *rt);
/** @}*/
void * comps_objdict_clone_v(void * rt);

/** \addtogroup comps_multi_dict
 *@{*/
/** Makes copy of multi-dictionary
 *
 * Items in new dictionary is same items with incremented reference
 * counter only
 *
 * @param rt COMPS_ObjDict object
 * @return new COMPS_ObjDict object
 */
COMPS_ObjMDict* comps_objmdict_clone(COMPS_ObjMDict *rt);
/** @}*/
void* comps_objmdict_clone_v(void *rt);

/** \addtogroup comps_multi_dict
 *@{*/
/** Return list of keys in multi-dictionary
 *
 * @param rt COMPS_ObjMDict object
 * @return COMPS_HSList of key in multi-dictionary
 */
COMPS_HSList* comps_objmdict_keys(COMPS_ObjMDict *rt);
/** @}*/

/** \addtogroup comps_dict
 *@{*/
/** Return list of keys in dictionary
 *
 * @param rt COMPS_ObjDict object
 * @return COMPS_HSList of key in dictionary
 */
COMPS_HSList* comps_objdict_keys(COMPS_ObjDict *rt);

/** Return list of pairs(key-value) in dictionary
 *
 * @param rt COMPS_ObjDict object
 * @return COMPS_HSList of pairs in dictionary
 */
COMPS_HSList* comps_objdict_pairs(COMPS_ObjDict *rt);
/** @}*/

/** \addtogroup comps_multi_dict
 *@{*/
/** Return list of pairs(key-value) in multi-dictionary
 *
 * @param rt COMPS_ObjMDict object
 * @return COMPS_HSList of pairs in dictionary
 */
COMPS_HSList* comps_objmdict_pairs(COMPS_ObjMDict *rt);
/** @}*/

/** \addtogroup comps_dict
 *@{*/
//void comps_mdict_unite(COMPS_MDict *d1, COMPS_MDict *d2);

/** Join two dictionries into one
 *
 * New dictionary is filled with pairs of first dictionary and then
 * with pairs of second dictionary with skipped items whose keys are already
 * in dictionary
 *
 * @param d1 COMPS_ObjDict object
 * @param d2 COMPS_ObjDict object
 * @return new COMPS_ObjDict object
 */
COMPS_ObjDict* comps_objdict_union(COMPS_ObjDict *d1, COMPS_ObjDict *d2);
/** @}*/
#endif
