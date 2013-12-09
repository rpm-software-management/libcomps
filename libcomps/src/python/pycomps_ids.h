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

#ifndef PYCOMPS_IDS_H
#define PYCOMPS_IDS_H

#include <Python.h>
#include "libcomps/comps_doc.h"
//#include "libcomps/comps_list.h"
#include "structmember.h"
#include "pycomps_sequence.h"
#include "pycomps_utils.h"

int PyCOMPSIDs_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);

//PyObject* ids_getitem(PyObject *self, Py_ssize_t index);
//int ids_setitem(PyObject *self, Py_ssize_t index, PyObject *value);
//int ids_setslice(PyObject *self, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *value);
//PyObject* ids_getslice(PyObject *self, Py_ssize_t ilow, Py_ssize_t ihigh);
//PyObject* ids_concat(PyObject *self, PyObject *other);
//PyObject* ids_repeat(PyObject *self, Py_ssize_t count);
//PyObject* PyCOMPSIDs_append(PyObject * self, PyObject *item);

extern PyTypeObject PyCOMPS_IDsType;

#endif
