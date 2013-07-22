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

#ifndef PYCOMPS_DICT_H
#define PYCOMPS_DICT_H

#include "libcomps/comps.h"
#include "libcomps/comps_dict.h"

#include "pycomps_utils.h"
#include "pycomps_ctopy.h"

#include "Python.h"
#include "structmember.h"


typedef struct PyCOMPS_Dict {
    PyObject_HEAD
    PyCOMPS_CtoPy_CItem *citem;
} PyCOMPS_Dict;

typedef struct PyCOMPS_DictIter{
    PyObject_HEAD
    COMPS_HSListItem *hsit;
    COMPS_HSList *hslist;
    PyObject* (*out_func)(COMPS_HSListItem *hsit);
} PyCOMPS_DictIter;

COMPS_Dict * get_dict(PyObject *self);

void PyCOMPSDict_dealloc(PyCOMPS_Dict *self);
PyObject* PyCOMPSDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSDict_init(PyCOMPS_Dict *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSDict_str(PyObject *self);
PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op);
PyObject* PyCOMPSDict_getiteritems(PyObject *self);

PyObject* PyCOMPSDictIter_new(PyTypeObject *type, PyObject *args,
                              PyObject *kwds);
void PyCOMPSDictIter_dealloc(PyCOMPS_DictIter *self);
PyObject* PyCOMPSDict_iternext(PyObject *iter_o);


extern PyTypeObject PyCOMPS_DictType;
extern PyTypeObject PyCOMPS_DictIterType;

#endif
