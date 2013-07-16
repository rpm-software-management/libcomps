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
 * along with this program; if not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 */

#include "pycomps_ctopy_seq_iter.h"

PyObject* PyCOMPSCtoPySeqIter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    PyCOMPS_CtoPySeqIter *self;

    self = (PyCOMPS_CtoPySeqIter*) type->tp_alloc(type, 0);
    return (PyObject*) self;
}
void PyCOMPSCtoPySeqIter_dealloc(PyObject *self) {
    Py_TYPE(self)->tp_free((PyObject*)self);
}



int PyCOMPSCtoPySeqIter_init(PyCOMPS_CtoPySeqIter *self,
                             PyObject *args,
                             PyObject *kwds)
{
    (void)args;
    (void)kwds;

    self->it = NULL;
    self->pos = 0;
    return 0;
}

PyObject* PyCOMPSCtoPySeqIter_iternext(PyObject *self) {
    PyObject *ret;
    if (((PyCOMPS_CtoPySeqIter*)self)->it) {
        ret = ctopy_list_getitem(((PyCOMPS_CtoPySeqIter*)self)->seq,
                                 ((PyCOMPS_CtoPySeqIter*)self)->pos);
    }
    if (((PyCOMPS_CtoPySeqIter*)self)->it) {
        ((PyCOMPS_CtoPySeqIter*)self)->it = ((PyCOMPS_CtoPySeqIter*)self)->it->next;
        ((PyCOMPS_CtoPySeqIter*)self)->pos += 1;
    } else {
        PyErr_SetNone(PyExc_StopIteration);
        return NULL;
    }
    return ret;
}

PyObject * PyCOMPSCtoPySeqIter_getattr(PyObject * self, char * attr) {
    return PyObject_GetAttrString(self, attr);
}

PyMemberDef PyCOMPSCtoPySeqIter_members[] = {
    {NULL}};

PyMethodDef PyCOMPSCtoPySeqIter_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_CtoPySeqIterType = {
    PY_OBJ_HEAD_INIT
    "libcomps.SeqIter",   /*tp_name*/
    sizeof(PyCOMPS_CtoPySeqIter), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    PyCOMPSCtoPySeqIter_dealloc,    /*tp_dealloc*/
    0,                         /*tp_print*/
    &PyCOMPSCtoPySeqIter_getattr,   /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Comps CtoPySeq Iterator",  /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,//PyCOMPSCtoPySeq_getiter,    /* tp_iter */
    PyCOMPSCtoPySeqIter_iternext,   /* tp_iternext */
    PyCOMPSCtoPySeqIter_methods,/* tp_methods */
    PyCOMPSCtoPySeqIter_members,/* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSCtoPySeqIter_init,  /* tp_init */
    0,                              /* tp_alloc */
    PyCOMPSCtoPySeqIter_new,             /* tp_new */};

