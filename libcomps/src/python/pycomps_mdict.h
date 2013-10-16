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

#ifndef PYCOMPS_MDICT_H
#define PYCOMPS_MDICT_H

#include "libcomps/comps_doc.h"
#include "libcomps/comps_objdict.h"

#include "pycomps_sequence.h"
#include "pycomps_utils.h"

#include <Python.h>
#include "structmember.h"


void PyCOMPSMDict_dealloc(PyCOMPS_MDict *self);
PyObject* PyCOMPSMDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSMDict_init(PyCOMPS_MDict *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSMDict_str(PyObject *self);
PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op);
PyObject* PyCOMPSMDict_getiteritems(PyObject *self);

PyObject* PyCOMPSMDictIter_new(PyTypeObject *type, PyObject *args,
                              PyObject *kwds);
int PyCOMPSMDictIter_init(PyCOMPS_MDictIter *self, PyObject *args, PyObject *kwds);
void PyCOMPSMDictIter_dealloc(PyCOMPS_MDictIter *self);
PyObject* PyCOMPSMDict_iternext(PyObject *iter_o);

COMPS_Object* __pycomps_unicode_in(PyObject *obj);
COMPS_Object* __pycomps_bytes_in(PyObject *pobj);
PyObject* __pycomps_str_out(COMPS_Object *obj);

int PyCOMPSStrSeq_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);

extern PyTypeObject PyCOMPS_StrSeqType;

#endif
