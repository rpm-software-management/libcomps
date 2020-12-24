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

#ifndef PYCOMPS_SEQ_H
#define PYCOMPS_SEQ_H

#include "pycomps_macros.h"

#include "pycomps_hash.h"
#include "pycomps_types.h"

#include "libcomps/comps_objlist.h"
#include "libcomps/comps_utils.h"


Py_ssize_t list_len(PyObject *self);
PyObject *list_getitem(PyObject *self, Py_ssize_t index);
PyObject* list_getitem_byid(PyObject *self, PyObject *id);
PyObject* list_get_slice(PyObject *self, PyObject *key);

int list_setitem_id_unique(PyObject *self, Py_ssize_t index, PyObject *item);
int list_setitem(PyObject *self, Py_ssize_t index, PyObject *value);
int PyCOMPSSeq_set_unique(PyObject *self, PyObject *key, PyObject *val);
PyObject* list_concat(PyObject *self, PyObject *other);
PyObject *PyCOMPSSeq_get(PyObject *self, PyObject *key);
PyObject* PyCOMPSSeq_id_get(PyObject *self, PyObject *key);
int PyCOMPSSeq_set(PyObject *self, PyObject *key, PyObject *val);

PyObject* list_repeat(PyObject *self, Py_ssize_t count);
PyObject* PyCOMPSSeq_clear(PyObject *self);

void PyCOMPSSeq_dealloc(PyCOMPS_Sequence *self);
PyObject* PyCOMPSSeq_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSSeq_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSSeq_append(PyObject * self, PyObject *item);
PyObject* PyCOMPSSeq_append_unique(PyObject * self, PyObject *item);
PyObject* PyCOMPSSeq_str(PyObject *self);
PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op);

void py_xdecref(void* obj);

PyObject *list_getitem_X(PyObject *self, PyObject *key);

PyObject* PyCOMPSSeqIter_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSSeqIter_init(PyCOMPS_SeqIter *self, PyObject *args, PyObject *kwds);

extern PyMappingMethods PyCOMPSSeq_mapping_extra;

#endif
