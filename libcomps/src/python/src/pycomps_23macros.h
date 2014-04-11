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

#ifndef PYCOMPS_23MACROS_H
#define PYCOMPS_23MACROS_H
#include <sys/types.h>

#include "pyport.h"
#include <Python.h>
#include "structmember.h"


#if PY_MAJOR_VERSION >= 3
    #define PYINIT_FUNC PyInit__libpycomps
#else
    #define PYINIT_FUNC init_libpycomps
#endif

#if PY_MAJOR_VERSION >= 3
    #define MODINIT_RET_NONE return NULL
    #define PY_OBJ_HEAD_INIT PyVarObject_HEAD_INIT(NULL, 0)
    #define IS_PY3K
    #define PyINT_CHECK(OB) PyLong_Check(OB)
    #define PyINT_ASLONG(OB) PyLong_AsLong(OB)
    #define PyINT_FROM_LONG(OB) PyLong_FromLong(OB)
    #define SLICE_CAST PyObject*
    #define PYCOMPS_DICT_ITERITEMS "items"
    #define PYCOMPS_DICT_ITERVALUES "values"
    #define PYCOMPS_DICT_ITERKEYS "keys"
#else
    #define MODINIT_RET_NONE return
    #define PY_OBJ_HEAD_INIT PyObject_HEAD_INIT(NULL)\
                             0,
    #define PyINT_CHECK(OB) PyInt_Check(OB)
    #define PyINT_ASLONG(OB) PyInt_AsLong(OB)
    #define PyINT_FROM_LONG(OB) PyInt_FromLong(OB)

    #define SLICE_CAST PySliceObject*
    #define PYCOMPS_DICT_ITERITEMS "iteritems"
    #define PYCOMPS_DICT_ITERVALUES "itervalues"
    #define PYCOMPS_DICT_ITERKEYS "iterkeys"
#endif




#define CMP_OP_EQ_NE_CHECK(OP)                                  \
    if ((OP) != Py_EQ && (OP) != Py_NE) {                       \
        PyErr_Format(PyExc_NotImplementedError, "Unsuported operator");   \
        return NULL; \
    }

#define CMP_NONE_CHECK(OP, SELF, OTHER)                         \
    if ((OTHER) == Py_None && (SELF) == Py_None)                \
        return ((OP) == Py_EQ)?(Py_INCREF(Py_True),Py_True):    \
                             (Py_INCREF(Py_False),Py_False);    \
    else if (((OTHER) == Py_None && (SELF) != Py_None) ||       \
             ((OTHER) != Py_None && (SELF) == Py_None))         \
        return ((OP) == Py_EQ)?(Py_INCREF(Py_False),Py_False):  \
                             (Py_INCREF(Py_True),Py_True);      \

#define PYUNICODE_ASSTRING(U) PyBytes_AsString(PyUnicode_AsUTF8String(U))

#define SET_TO(STRUCT, OFFSET, P) *((size_t*)(((char*)STRUCT)+OFFSET)) = (size_t)P;
#define GET_FROM(STRUCT, OFFSET) *((char**)(((char*)STRUCT)+OFFSET))

#endif /*PYCOMPS_23MACROS_H*/
