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

#ifndef COMPS_Set_H
#define COMPS_Set_H

#include "comps_hslist.h"

typedef struct {
    char (*eqf)(void*, void*);
    void (*data_destructor)(void*);
    void* (*data_cloner)(void*);
    void* (*data_constructor)(void*);
    COMPS_HSList *data;
} COMPS_Set;

void* comps_set_index_clone(void *item);
COMPS_Set * comps_set_create();
void comps_set_destroy(COMPS_Set **set);
void comps_set_destroy_v(void *set);
void comps_set_init(COMPS_Set *set,  void* (*data_constructor)(void*),
                                     void* (*data_cloner)(void*),
                                     void (*data_destructor)(void*),
                                     char (*eqf)(void*, void*));

char comps_set_in(COMPS_Set *set, void *item);
char comps_set_add(COMPS_Set *set, void *item);
void* comps_set_remove(COMPS_Set *set, void *item);
char comps_set_is_empty(COMPS_Set *set);
char comps_set_cmp(COMPS_Set *set1, COMPS_Set *set2);
int comps_set_at(COMPS_Set *set, void *item);
void* comps_set_data_at(COMPS_Set * set, void * item);
void comps_set_clear(COMPS_Set *set);

#endif
