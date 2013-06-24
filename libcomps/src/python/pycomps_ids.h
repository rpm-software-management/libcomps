#ifndef PYCOMPS_IDS_H
#define PYCOMPS_IDS_H

#include <Python.h>
#include "libcomps/comps.h"
#include "libcomps/comps_list.h"
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
