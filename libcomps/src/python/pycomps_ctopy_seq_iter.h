#ifndef PYCOMPS_CTOPY_SEQITER_H
#define PYCOMPS_CTOPY_SEQITER_H

#include "libcomps/comps_list.h"

#include <Python.h>
#include "structmember.h"
#include "pycomps_23macros.h"
#include "pycomps_ctopy_seq.h"

typedef struct PyCOMPS_CtoPy_SeqIter{
    PyObject_HEAD
    PyObject *seq;
    COMPS_ListItem *it;
    long pos;
} PyCOMPS_CtoPySeqIter;

PyObject* PyCOMPSCtoPySeqIter_new(PyTypeObject *type,
                                  PyObject *args,
                                  PyObject *kwds);
int PyCOMPSCtoPySeqIter_init(PyCOMPS_CtoPySeqIter *self,
                             PyObject *args,
                             PyObject *kwds);

PyObject * PyCOMPSCtoPySeqIter_getattr(PyObject * self, char * attr);

void PyCOMPSCat_dealloc(PyObject *self);

extern PyTypeObject PyCOMPS_CtoPySeqIterType;
#endif
