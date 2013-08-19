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

#ifndef COMPS_UTILS_H
#define COMPS_UTILS_H

#include <stdlib.h>
#include <string.h>

#define COMPS_DOCLIST_PREP(OBJ, LIST)\
    if (OBJ->LIST == NULL) {\
        OBJ->LIST = comps_list_create();\
        comps_list_init(OBJ->LIST);\
    }

char __comps_strcmp(void *s1, void *s2);
void* __comps_str_clone(void *str);

#endif
