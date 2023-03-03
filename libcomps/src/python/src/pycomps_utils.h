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

#ifndef PYCOMPS_UTILS_H
#define PYCOMPS_UTILS_H

#include <Python.h>
#include "structmember.h"

#include "pycomps_types.h"
#include "pycomps_macros.h"
#include "pycomps_dict.h"

#include "libcomps/comps_objdict.h"
#include "libcomps/comps_objlist.h"
#include "libcomps/comps_utils.h"

typedef struct PyCompsObject {
    PyObject_HEAD
    COMPS_Object *c_obj;
} PyCompsObject;

typedef struct {
    COMPS_Object* (*get_f)(COMPS_Object*);
    void (*set_f)(COMPS_Object*, int, bool);
    char deleteable;
} __PyCOMPS_NumPropGetSetClosure;

typedef struct {
    COMPS_Object* (*get_f)(COMPS_Object*);
    void (*set_f)(COMPS_Object*, char*, char);
    char deleteable;
} __PyCOMPS_StrPropGetSetClosure;

typedef struct {
    COMPS_ObjList* (*get_f)(COMPS_Object*);
    PyTypeObject *type;
    void (*set_f)(COMPS_Object*, COMPS_ObjList*);
    size_t p_offset;
} __PyCOMPS_ListGetSetClosure;

typedef struct {
    size_t p_offset;
    size_t dict_offset;
    PyCOMPS_ItemInfo *dict_info;
    PyTypeObject *dict_type;
} __PyCOMPS_DictGetSetClosure;

#define __H_COMPS_NUMPROP_GETSET_CLOSURE(C_TYPE)\
typedef struct {\
    COMPS_Object* (*get_f)(C_TYPE*);\
    void (*set_f)(C_TYPE*, int, bool);\
    char deleteable;\
} CONCAT(CONCAT(PyCOMPS_, C_TYPE), _NumPropGetSetClosure);

#define __COMPS_NUMPROP_GETSET_CLOSURE(C_TYPE)\
        CONCAT(CONCAT(PyCOMPS_, C_TYPE), _NumPropGetSetClosure)

#define __H_COMPS_STRPROP_GETSET_CLOSURE(C_TYPE)\
typedef struct {\
    COMPS_Object* (*get_f)(C_TYPE*);\
    void (*set_f)(C_TYPE*, char*, char);\
    char deleteable;\
} CONCAT(CONCAT(PyCOMPS_, C_TYPE), _StrPropGetSetClosure);

#define __COMPS_STRPROP_GETSET_CLOSURE(C_TYPE)\
        CONCAT(CONCAT(PyCOMPS_, C_TYPE), _StrPropGetSetClosure)

#define __H_COMPS_LIST_GETSET_CLOSURE(C_TYPE)\
typedef struct {\
    COMPS_ObjList *(*get_f)(C_TYPE*);\
    PyTypeObject *type;\
    void (*set_f)(C_TYPE*, COMPS_ObjList*);\
    size_t p_offset;\
} CONCAT(CONCAT(PyCOMPS_, C_TYPE), _ListGetSetClosure);

#define __COMPS_LIST_GETSET_CLOSURE(C_TYPE)\
        CONCAT(CONCAT(PyCOMPS_, C_TYPE), _ListGetSetClosure)

#define __H_COMPS_DICT_GETSET_CLOSURE(C_TYPE)\
typedef struct {\
    size_t p_offset;\
    size_t dict_offset;\
    PyCOMPS_ItemInfo *dict_info;\
    PyTypeObject *dict_type;\
} CONCAT(CONCAT(PyCOMPS_, C_TYPE), _DictGetSetClosure);

#define __COMPS_DICT_GETSET_CLOSURE(C_TYPE)\
        CONCAT(CONCAT(PyCOMPS_, C_TYPE), _DictGetSetClosure)


PyObject* __PyCOMPS_get_ids(PyObject *self, void *closure);
int __PyCOMPS_set_ids(PyObject *self, PyObject *value, void *closure);

PyObject* __PyCOMPS_get_dict(PyObject *self, void *closure);
int __PyCOMPS_set_dict(PyObject *self, PyObject *value, void *closure);

PyObject* __PyCOMPS_get_strattr(PyObject *self, void *closure);
int __PyCOMPS_set_strattr(PyObject *self, PyObject *val, void *closure);
PyObject* __PyCOMPS_get_numattr(PyObject *self, void *closure);
int __PyCOMPS_set_numattr(PyObject *self, PyObject *val, void *closure);
PyObject* __PyCOMPS_get_boolattr(PyObject *self, void *closure);
int __PyCOMPS_set_boolattr(PyObject *self, PyObject *val, void *closure);

int __pycomps_validate_process(COMPS_ValGenResult *result);

PyObject* __pycomps_str_getter(char * prop);
char __pycomps_strcmp(const char *s1, const char *s2);
char __pycomps_strcmp_v(void *c1, void *c2);
char __pycomps_set_nodel_check(PyObject * value);
PyObject* __pycomps_arg_to_unicode(PyObject *o);
PyObject* __pycomps_arg_to_unicode2(PyObject *o);
signed char __pycomps_arg_to_char(PyObject *value, char ** ret);
signed char __pycomps_stringable_to_char(PyObject *value, char ** ret);
signed char __pycomps_PyUnicode_AsString(PyObject *val, char **ret);
void* __pycomps_strcloner(void *str);
PyObject* __pycomps_lang_decode(char * lang);
PyObject* __pycomps_dict_key_out(COMPS_HSListItem *hsit);
COMPS_Object* __pycomps_unicode_in(PyObject *obj);
COMPS_Object* __pycomps_bytes_in(PyObject *pobj);
PyObject* __pycomps_str_out(COMPS_Object *obj);
PyObject *str_to_unicode(void* str);

Py_hash_t PyCOMPS_hash(PyObject *self);

PyObject* PyCOMPSSeq_extra_get(PyObject *self, PyObject *key);

#endif
