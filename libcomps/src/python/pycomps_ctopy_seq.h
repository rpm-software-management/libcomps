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

#ifndef PYCOMPS_CTOPY_SEQ_H
#define PYCOMPS_CTOPY_SEQ_H
#include "pycomps_ctopy.h"
#include "pycomps_ctopy_seq_iter.h"
#include "libcomps/comps_bradix.h"


typedef struct PyCOMPS_CtoPySeqItemMan {
    PyTypeObject * item_type;
    PyObject* (*ctopy_convert)(void*);
    void (*data_decref)(void*);
    void (*data_incref)(void*);
    char (*data_cmp)(void *, void*);
    void (*fprint_f)(FILE*, void*);
    PyObject* (*str_f)(void*);

} PyCOMPS_CtoPySeqItemMan;

typedef struct PyCOMPS_CtoPySeq {
    PyObject_HEAD
    PyCOMPS_CtoPy_CItem *citem;
    COMPS_List *citem_list;
    COMPS_BRTree * ctopy_map;
    PyCOMPS_CtoPySeqItemMan *item_man;
} PyCOMPS_CtoPySeq;


COMPS_List * ctopy_get_list(PyObject *self);
Py_ssize_t list_len(PyObject *self);
PyObject *ctopy_list_getitem(PyObject *self, Py_ssize_t index);
int ctopy_list_setitem(PyObject *self, Py_ssize_t index, PyObject *item);
PyObject* ctopy_list_concat(PyObject *self, PyObject *other);
PyObject *PyCOMPSCtoPySeq_get(PyObject *self, PyObject *key);
int PyCOMPSCtoPySeq_set(PyObject *self, PyObject *key, PyObject *val);

PyObject* list_repeat(PyObject *self, Py_ssize_t count);
PyObject* PyCOMPSCtoPySeq_clear(PyObject *self);

void PyCOMPSCtoPySeq_dealloc(PyCOMPS_CtoPySeq *self);
PyObject* PyCOMPSCtoPySeq_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSCtoPySeq_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSCtoPySeq_append(PyObject * self, PyObject *item);
PyObject* PyCOMPSCtoPySeq_str(PyObject *self);
int PyCOMPSCtoPySeq_print(PyObject *self, FILE *f, int flags);
PyObject* PyCOMPSCtoPySeq_cmp(PyObject *self, PyObject *other, int op);

void py_xdecref(void* obj);

PyObject *list_getitem_X(PyObject *self, PyObject *key);

extern PyTypeObject PyCOMPS_CtoPySeqType;

#endif
