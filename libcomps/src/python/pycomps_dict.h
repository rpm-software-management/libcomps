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

COMPS_Dict * get_dict(PyObject *self);

void PyCOMPSDict_dealloc(PyCOMPS_Dict *self);
PyObject* PyCOMPSDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSDict_init(PyCOMPS_Dict *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSDict_str(PyObject *self);
PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op);

extern PyTypeObject PyCOMPS_DictType;

#endif
