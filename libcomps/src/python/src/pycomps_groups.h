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

#ifndef PYCOMPS_GROUPS_H
#define PYCOMPS_GROUPS_H

#include "pycomps_macros.h"

#include "libcomps/comps_doc.h"
//#include "libcomps/comps_list.h"

#include "pycomps_sequence.h"
#include "pycomps_utils.h"
#include "pycomps_ids.h"
#include "pycomps_dict.h"
#include "pycomps_macros.h"

typedef struct {
    PyObject_HEAD
    COMPS_DocGroup *c_obj;
    PyObject *p_packages;
    PyObject *p_name_by_lang;
    PyObject *p_desc_by_lang;
} PyCOMPS_Group;

typedef struct {
    PyObject_HEAD
    COMPS_DocGroupPackage * c_obj;
} PyCOMPS_Package;

__H_COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) /*pycomps_utils.h macro*/
__H_COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocGroup) /*pycomps_utils.h macro*/
__H_COMPS_LIST_GETSET_CLOSURE(COMPS_DocGroup) /*pycomps_utils.h macro*/
__H_COMPS_DICT_GETSET_CLOSURE(COMPS_DocGroup) /*pycomps_utils.h macro*/

__H_COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) /*pycomps_utils.h macro*/
__H_COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) /*pycomps_utils.h macro*/

COMPS_ObjList* comps_groups_union(COMPS_ObjList *groups1,
                                  COMPS_ObjList *groups2);


void PyCOMPSGroup_dealloc(PyObject *self);
PyObject * PyCOMPSGroup_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSGroup_init(PyCOMPS_Group *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSGroup_get_name_by_lang(PyCOMPS_Group *self, void *closure);
int PyCOMPSGroup_set_name_by_lang(PyCOMPS_Group *self, PyObject *value,
                                                       void *closure);
PyObject* PyCOMPSGroup_get_desc_by_lang(PyCOMPS_Group *self, void *closure);
int PyCOMPSGroup_set_desc_by_lang(PyCOMPS_Group *self, PyObject *value,
                                                       void *closure);
PyObject* PyCOMPSGroup_packages_match(PyObject *self, PyObject *args,
                                      PyObject *kwds);

PyObject * PyCOMPSGroups_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSGroups_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);

void pycomps_pkg_decref(void * pkg);
void pycomps_pkg_incref(void * pkg);
void pycomps_pkg_destroy(void * pkg);
void comps_pkg_print(FILE *f, void *p);

COMPS_DocGroupPackage* pycomps_pkg_get(PyObject *pypkg);
PyObject* PyCOMPSPack_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSPack_init(PyCOMPS_Package *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSPack_convert(void *p);

int PyCOMPSPacks_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);

extern PyTypeObject PyCOMPS_GroupsType;
extern PyTypeObject PyCOMPS_GroupType;
extern PyTypeObject PyCOMPS_PacksType;
extern PyTypeObject PyCOMPS_PackType;

#endif
