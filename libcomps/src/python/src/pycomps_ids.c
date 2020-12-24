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

#include "pycomps_ids.h"

void* str_from_PyObj_convert(PyObject *obj) {
    char *str;
    if (__pycomps_arg_to_char(obj, &str)) {
        return NULL;
    }
    return str;
}

char pycomps_ids_cmp(void *id1, void *id2) {
    return !strcmp((char*)id1,(char*)id2);
}

int PyCOMPSIDs_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    comps_list_init(get_list((PyObject*)self));

    self->itemtypes = malloc(sizeof(PyTypeObject*)*2);
    self->in_convert_funcs = malloc(sizeof(PyCOMPSSeq_in_itemconvert)*2);
    self->itemtypes[0] = &PyUnicode_Type;
    self->in_convert_funcs[0] = str_from_PyObj_convert;
    self->itemtypes[1] = &PyBytes_Type;
    self->in_convert_funcs[1] = str_from_PyObj_convert;
    self->item_types_len = 2;

    self->out_convert_func = &str_to_unicode;
    self->item_cmp_func = &pycomps_ids_cmp;
  return 0;
}

PyMemberDef PyCOMPSIDs_members[] = {
    {NULL}};

PyMethodDef PyCOMPSIDs_methods[] = {
     //{"append", (PyCFunction)PyCOMPSIDs_append, METH_O,
     // "Append item to new of the list"},
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_IDsType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.IdList",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    sizeof(PyObject),         /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,//&PyCOMPSID_sequence,       /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSSeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Id list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSSeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSIDs_methods,        /* tp_methods */
    PyCOMPSIDs_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_SeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSIDs_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSSeq_new,                 /* tp_new */};
