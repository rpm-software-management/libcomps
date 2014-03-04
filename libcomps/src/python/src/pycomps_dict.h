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

#include "libcomps/comps_doc.h"
#include "libcomps/comps_objdict.h"

//#include "pycomps_types.h"
#include "pycomps_utils.h"

#include "pyport.h"
#include <Python.h>
#include "structmember.h"


void PyCOMPSDict_dealloc(PyCOMPS_Dict *self);
PyObject* PyCOMPSDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSDict_init(PyCOMPS_Dict *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSDict_str(PyObject *self);
PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op);
PyObject* PyCOMPSDict_getiteritems(PyObject *self);

PyObject* PyCOMPSDictIter_new(PyTypeObject *type, PyObject *args,
                              PyObject *kwds);
int PyCOMPSDictIter_init(PyCOMPS_DictIter *self, PyObject *args, PyObject *kwds);
void PyCOMPSDictIter_dealloc(PyCOMPS_DictIter *self);
PyObject* PyCOMPSDict_iternext(PyObject *iter_o);

COMPS_Object* __pycomps_unicode_in(PyObject *obj);
COMPS_Object* __pycomps_bytes_in(PyObject *pobj);
PyObject* __pycomps_str_out(COMPS_Object *obj);

extern PyTypeObject PyCOMPS_StrDictType;
extern PyCOMPS_ItemInfo PyCOMPS_StrDictInfo;

#endif
