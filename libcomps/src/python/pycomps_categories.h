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

#include <Python.h>
#include "libcomps/comps.h"
#include "libcomps/comps_list.h"
#include "structmember.h"
#include "pycomps_sequence.h"
#include "pycomps_dict.h"
#include "pycomps_ids.h"
#include "pycomps_utils.h"
#include "pycomps_23macros.h"
#include "pycomps_ctopy_seq.h"
#include "pycomps_ctopy.h"
#include "pycomps_lang.h"

typedef struct {
    PyObject_HEAD
    PyCOMPS_CtoPy_PItem_HEAD
    PyObject *group_ids_pobj;
    PyObject *name_by_lang_pobj;
    PyObject *desc_by_lang_pobj;
} PyCOMPS_Category;

typedef struct COMPS_DocCategoryExtra {
    PyCOMPS_CtoPy_CItem *citem;
    PyCOMPS_CtoPy_CItem *group_ids_citem;
    PyCOMPS_CtoPy_CItem *name_by_lang_citem;
    PyCOMPS_CtoPy_CItem *desc_by_lang_citem;
} COMPS_DocCategoryExtra;

COMPS_DocCategoryExtra* comps_doccategory_extra_create();
void comps_doccategory_extra_destroy(void *cdce);
char __pycomps_cat_idcmp(void *c1, void *c2);

COMPS_List* comps_cats_union(COMPS_List *cats1, COMPS_List *cats2);

void pycomps_cat_destroy(void *cat);
void pycomps_cat_decref(void *cat);
void pycomps_cat_incref(PyObject *pycat);
void comps_cat_incref(void *cat);
COMPS_DocCategory* pycomps_cat_oget(PyObject *pycat);
COMPS_DocCategory* pycomps_cat_gget(PyCOMPS_Category *pycat);
COMPS_DocCategoryExtra* pycomps_cat_get_extra(PyObject *pycat);
COMPS_DocCategoryExtra* comps_cat_get_extra(void *doccat);
PyObject* comps_cat_str(void * cat);

PyObject* PyCOMPSCat_convert(void *c);

COMPS_DocCategory* doccat_from_pycompscat(PyObject *self);
PyObject* pycompscat_from_doccat_v(void *c);

void PyCOMPSCat_dealloc(PyObject *self);
PyObject * PyCOMPSCat_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSCat_init(PyCOMPS_Category *self, PyObject *args, PyObject *kwds);

PyObject *cats_getitem(PyObject *self, Py_ssize_t index);
int cats_setitem(PyObject *self, Py_ssize_t index, PyObject *value);
int cats_setslice(PyObject *self, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *value);
PyObject* cats_getslice(PyObject *self, Py_ssize_t ilow, Py_ssize_t ihigh);
PyObject* cats_concat(PyObject *self, PyObject *other);
PyObject* cats_repeat(PyObject *self, Py_ssize_t count);
PyObject* PyCOMPSCats_append(PyObject * self, PyObject *item);

PyObject* PyCOMPSCat_get_id(PyCOMPS_Category *self, void *closure);
PyObject* PyCOMPSCat_get_name(PyCOMPS_Category *self, void *closure);
PyObject* PyCOMPSCat_get_desc(PyCOMPS_Category *self, void *closure);
int PyCOMPSCat_set_desc(PyCOMPS_Category *self, PyObject *value, void *closure);

void PyCOMPSCats_dealloc(PyCOMPS_Sequence* self);
int PyCOMPSCats_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds);

PyObject* PyCOMPSCat_get_name_by_lang(PyObject *self, void *closure);
int PyCOMPSCat_set_name_by_lang(PyObject *self, PyObject *value, void *closure);
PyObject* PyCOMPSCat_get_desc_by_lang(PyObject *self, void *closure);
int PyCOMPSCat_set_desc_by_lang(PyObject *self, PyObject *value, void *closure);


extern PyTypeObject PyCOMPS_CatsType;
extern PyTypeObject PyCOMPS_CatType;

#endif
