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

#ifndef PYCOMPS_ENVS_H
#define PYCOMPS_ENVS_H

#include "pycomps_macros.h"

#include "libcomps/comps_doc.h"
//#include "libcomps/comps_list.h"

#include "pycomps_sequence.h"
#include "pycomps_gids.h"
#include "pycomps_utils.h"
#include "pycomps_macros.h"
#include "pycomps_dict.h"
#include "pycomps_sequence.h"

/*typedef struct {
    PyObject_HEAD
    COMPS_List *list;
} PyCOMPS_Envs;
*/

typedef struct {
    PyObject_HEAD
    COMPS_DocEnv *c_obj;
    PyObject *p_group_list;
    PyObject *p_option_list;
    PyObject *p_name_by_lang;
    PyObject *p_desc_by_lang;
} PyCOMPS_Env;

__H_COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocEnv) /*pycomps_utils.h macro*/
__H_COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocEnv) /*pycomps_utils.h macro*/
__H_COMPS_LIST_GETSET_CLOSURE(COMPS_DocEnv) /*pycomps_utils.h macro*/
__H_COMPS_DICT_GETSET_CLOSURE(COMPS_DocEnv) /*pycomps_utils.h macro*/

COMPS_ObjList* comps_envs_union(COMPS_ObjList *envs1, COMPS_ObjList *envs2);

PyObject* PyCOMPSEnv_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSEnvs_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);

int PyCOMPSEnv_init(PyCOMPS_Env *self, PyObject *args, PyObject *kwds);

PyObject* PyCOMPSEnv_get_name_by_lang(PyObject *self, void *closure);
int PyCOMPSEnv_set_name_by_lang(PyObject *self, PyObject *value, void *closure);
PyObject* PyCOMPSEnv_get_desc_by_lang(PyObject *self, void *closure);
int PyCOMPSEnv_set_desc_by_lang(PyObject *self, PyObject *value, void *closure);

PyObject* PyCOMPSEnvs_append(PyObject * self, PyObject *item);

extern PyTypeObject PyCOMPS_EnvType;
extern PyTypeObject PyCOMPS_EnvsType;

#endif
