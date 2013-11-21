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

#ifndef COMPS_MM_H
#define COMPS_MM_H

#include <stdlib.h>
#include <string.h>
#include <signal.h>

/*! \file comps_mm.h
 * \brief COMPS memory management(reference counter) file
 *
 * Details.
 * */

/**
    Reference counter structure
*/
typedef struct {
    size_t ref_count; /**< actual reference count for object */
    void (*destructor)(void*); /**< callback destructor, called when reference
                                    count fall bellow 1*/
    void *obj; /**< pointer to counted object itself */
} COMPS_RefC;

/** reference counter constructor */
COMPS_RefC* comps_refc_create(void *obj, void (*destructor)(void*));

/** if ref counter equals zero destroy holded object
 *  and ref counter object itself, otherwise decrement counter
 *  @see comps_refc_decref
 * */
void comps_refc_destroy(COMPS_RefC *refc);

/** alias with void argument
 *  @see comps_refc_destroy
 * */
void comps_refc_destroy_v(void *refc);

/** alias for comps_refc_destroy
 *  @see comps_refc_destroy
 * */
void comps_refc_decref(COMPS_RefC *refc);

/** increment reference counter by 1
 */
void comps_refc_incref(COMPS_RefC *refc);

#endif //COMPS_MM_H
