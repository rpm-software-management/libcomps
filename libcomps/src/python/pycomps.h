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

#ifndef PYCOMPS_H
#define PYCOMPS_H

#include <stddef.h>

#include "libcomps/comps_doc.h"
#include "libcomps/comps_parse.h"
#include "libcomps/comps_dict.h"

#include "pycomps_categories.h"
#include "pycomps_groups.h"
#include "pycomps_envs.h"
#include "pycomps_utils.h"
#include "pycomps_dict.h"
#include "pycomps_ctopy.h"
#include "pycomps_ctopy_seq.h"
#include "pycomps_ctopy_seq_iter.h"
#include "pycomps_gids.h"

#include <Python.h>
#include "structmember.h"

typedef struct {
    PyTypeObject * type;
    size_t pobj_offset;
    COMPS_ObjList *(*get_f)(COMPS_Doc*);
    void (*set_f)(COMPS_Doc*, COMPS_ObjList*);
} PyCOMPS_GetSetClosure;

typedef struct {
    PyObject_HEAD
    COMPS_Doc * comps_doc;
    PyObject *p_groups;
    PyObject *p_categories;
    PyObject *p_environments;
} PyCOMPS;

PyCOMPS_GetSetClosure * get_closure(void * closure);

extern PyTypeObject PyCOMPS_Type;

const char PYCOMPS_DOCU[] = "Comps class is representating comps.xml file"
" represented in structure form.";
#endif
