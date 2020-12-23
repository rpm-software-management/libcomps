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

#ifndef PYCOMPS_CATEGORIES_H
#define PYCOMPS_CATEGORIES_H

#include "pycomps_macros.h"

#include "libcomps/comps_doc.h"
//#include "libcomps/comps_list.h"

#include "pycomps_sequence.h"
#include "pycomps_dict.h"
#include "pycomps_gids.h"
#include "pycomps_utils.h"

typedef struct {
    PyObject_HEAD
    COMPS_DocCategory *c_obj;
    PyObject *p_group_ids;
    PyObject *p_name_by_lang;
    PyObject *p_desc_by_lang;
} PyCOMPS_Category;

__H_COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocCategory) /*pycomps_utils.h macro*/
__H_COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocCategory) /*pycomps_utils.h macro*/
__H_COMPS_LIST_GETSET_CLOSURE(COMPS_DocCategory) /*pycomps_utils.h macro*/
__H_COMPS_DICT_GETSET_CLOSURE(COMPS_DocCategory) /*pycomps_utils.h macro*/

COMPS_ObjList* comps_cats_union(COMPS_ObjList *cats1, COMPS_ObjList *cats2);

PyObject* PyCOMPSCat_convert(void *c);

void PyCOMPSCat_dealloc(PyObject *self);
PyObject * PyCOMPSCat_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSCat_init(PyCOMPS_Category *self, PyObject *args, PyObject *kwds);

PyObject* PyCOMPSCats_append(PyObject * self, PyObject *item);

void PyCOMPSCats_dealloc(PyCOMPS_Sequence* self);
int PyCOMPSCats_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);

PyObject* PyCOMPSCat_get_name_by_lang(PyObject *self, void *closure);
int PyCOMPSCat_set_name_by_lang(PyObject *self, PyObject *value, void *closure);
PyObject* PyCOMPSCat_get_desc_by_lang(PyObject *self, void *closure);
int PyCOMPSCat_set_desc_by_lang(PyObject *self, PyObject *value, void *closure);


extern PyTypeObject PyCOMPS_CatsType;
extern PyTypeObject PyCOMPS_CatType;

#endif
