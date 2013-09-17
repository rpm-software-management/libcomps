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

/*! \file comps_list.h
 * \brief A Documented file.
 * Details.
 * */
#ifndef COMPS_OBJLIST_H
#define COMPS_OBJLIST_H

#include "comps_obj.h"
#include "comps_utils.h"

#include <string.h>
#include <stdlib.h>

typedef struct COMPS_ObjListIt COMPS_ObjListIt;

struct COMPS_ObjListIt {
    COMPS_Object *comps_obj;
    COMPS_ObjListIt *next;
} UnfoObjListIt;

typedef struct COMPS_ObjList {
    COMPS_Object_HEAD
    COMPS_ObjListIt *first;
    COMPS_ObjListIt *last;
    size_t len;
} COMPS_ObjList;


void comps_objlist_create(COMPS_ObjList *objlist, COMPS_Object **args);
void comps_objlist_create_u(COMPS_Object *uobj, COMPS_Object **args);
void comps_objlist_destroy(COMPS_ObjList *objlist);
void comps_objlist_destroy_u(COMPS_Object *objlist);
void comps_objlist_clear(COMPS_ObjList *objlist);


const COMPS_ObjListIt *comps_objlist_it_next(const COMPS_ObjListIt *it);

int comps_objlist_walk(COMPS_ObjListIt **walker, COMPS_Object **result);
int comps_objlist_walk_r(COMPS_ObjListIt *walker_start,
                            COMPS_ObjListIt *mantinel,
                            COMPS_Object **result);


int comps_objlist_append_x(COMPS_ObjList *objlist, COMPS_Object *obj);
int comps_objlist_append(COMPS_ObjList *objlist, COMPS_Object *obj);
COMPS_Object* comps_objlist_get(COMPS_ObjList *objlist, unsigned int atpos);
int comps_objlist_set(COMPS_ObjList *objlist, unsigned int atpos,
                      COMPS_Object *obj);
COMPS_Object* comps_objlist_get_x(COMPS_ObjList *objlist, unsigned int atpos);
int comps_objlist_insert_at(COMPS_ObjList *objlist,
                           unsigned int pos,
                           COMPS_Object *obj);
int comps_objlist_remove_at(COMPS_ObjList *objlist, unsigned int atpos);
COMPS_ObjList* comps_objlist_sublist_it(COMPS_ObjListIt *startit,
                                      COMPS_ObjListIt *end);
COMPS_ObjList* comps_objlist_filter(COMPS_ObjList *list,
                                  char (*filter_func)(COMPS_Object*));

extern COMPS_ObjectInfo COMPS_ObjList_ObjInfo;

#endif
