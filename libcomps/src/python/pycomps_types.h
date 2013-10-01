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

#ifndef PYCOMPS_TYPES_H
#define PYCOMPS_TYPES_H

#include "libcomps/comps_objlist.h"
#include "libcomps/comps_objdict.h"

typedef COMPS_Object* (*PyCOMPSSeq_in_itemconvert)(PyObject*);
typedef PyObject* (*PyCOMPSSeq_out_itemconvert)(COMPS_Object*);

typedef struct PyCOMPS_SeqInfo {
    PyTypeObject ** itemtypes;
    PyCOMPSSeq_in_itemconvert *in_convert_funcs;
    PyCOMPSSeq_out_itemconvert out_convert_func;
    unsigned item_types_len;
} PyCOMPS_SeqInfo;

typedef struct PyCOMPS_Sequence {
    PyObject_HEAD
    COMPS_ObjList *list;
    PyCOMPS_SeqInfo *it_info;

} PyCOMPS_Sequence;

typedef struct PyCOMPS_Dict {
    PyObject_HEAD
    COMPS_ObjDict *dict;
} PyCOMPS_Dict;

typedef struct PyCOMPS_DictIter{
    PyObject_HEAD
    COMPS_ObjListIt *it;
    COMPS_HSListItem *hsit;
    COMPS_ObjList *objlist;
    COMPS_HSList *hslist;
    PyObject* (*out_func)(COMPS_HSListItem *hsit);
} PyCOMPS_DictIter;

extern PyTypeObject PyCOMPS_GIDsType;
extern PyTypeObject PyCOMPS_GIDType;

extern PyTypeObject PyCOMPS_SeqType;
extern PyTypeObject PyCOMPS_SeqIterType;

extern PyTypeObject PyCOMPS_DictType;
extern PyTypeObject PyCOMPS_DictIterType;
#endif
