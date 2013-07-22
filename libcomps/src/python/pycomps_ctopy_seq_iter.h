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
