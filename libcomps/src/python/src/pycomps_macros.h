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

#ifndef PYCOMPS_MACROS_H
#define PYCOMPS_MACROS_H
#include <sys/types.h>

#include <Python.h>
#include "structmember.h"


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

#define SET_TO(STRUCT, OFFSET, P) *((size_t*)(((char*)STRUCT)+OFFSET)) = (size_t)P;
#define GET_FROM(STRUCT, OFFSET) *((char**)(((char*)STRUCT)+OFFSET))

#endif /*PYCOMPS_MACROS_H*/
