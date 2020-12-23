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

#ifndef PYCOMPS_GIDS_H
#define PYCOMPS_GIDS_H

#include "pycomps_macros.h"

#include "libcomps/comps_doc.h"
//#include "libcomps/comps_list.h"
#include "libcomps/comps_docgroupid.h"

#include "pycomps_types.h"
#include "pycomps_utils.h"
#include "pycomps_macros.h"
#include "pycomps_sequence.h"

typedef struct {
    PyObject_HEAD
    COMPS_DocGroupId *c_obj;
} PyCOMPS_GID;

__H_COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroupId) /*pycomps_utils.h macro*/

COMPS_DocGroupId* pycomps_gid_get(PyObject *pygid);
PyObject* PyCOMPSGID_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSGID_init(PyCOMPS_GID *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSGID_convert(void *gid);

int PyCOMPSGIDs_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);


#endif
