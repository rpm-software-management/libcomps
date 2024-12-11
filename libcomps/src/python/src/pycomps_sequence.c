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
#include "libcomps/comps_objlist.h"
#include "pycomps_sequence.h"
#include "pycomps_macros.h"
//#include "pycomps_utils.h"

#include <Python.h>
#include "structmember.h"


Py_ssize_t PyCOMPSSeq_len(PyObject *self) {
    return ((PyCOMPS_Sequence*)self)->list->len;
}

inline PyObject* list_getitem(PyObject *self, Py_ssize_t index) {
    COMPS_Object *obj;
    PyObject *ret;
    int i;
    if (index < 0)
        i = ((PyCOMPS_Sequence*)self)->list->len + index;
    else
        i = index;
    obj = comps_objlist_get(((PyCOMPS_Sequence*)self)->list, i);
    if (obj == NULL) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return NULL;
    }
    //COMPS_OBJECT_DESTROY(obj);
    ret = ((PyCOMPS_Sequence*)self)->it_info->out_convert_func(obj);
    return ret;
}

inline PyObject* list_getitem_byid(PyObject *self, PyObject *id) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    char *strid=NULL;
    COMPS_ObjListIt *it;
    COMPS_Object *props, *oid;
    PyObject *ret = NULL;
    COMPS_Object *tmpstr;

    if (PyUnicode_Check(id)) {
        if (__pycomps_stringable_to_char(id, &strid)) {
            printf("stringable to char fail\n");
            return NULL;
        }
    } else if (PyBytes_Check(id)){
        strid = PyBytes_AsString(id);
    }
    tmpstr = (COMPS_Object*)comps_str(strid);
    for (it = ((PyCOMPS_Sequence*)self)->list->first; it != NULL;
         it = it->next) {
        props = (COMPS_Object*)GET_FROM(it->comps_obj,
                                         _seq_->it_info->props_offset);
        if (props->obj_info == &COMPS_ObjDict_ObjInfo) {
            oid = comps_objdict_get_x((COMPS_ObjDict*)props, "id");
        } else {
            oid = props;
        }
        if (comps_object_cmp(oid, tmpstr)) {
            comps_object_incref(it->comps_obj);
            ret = ((PyCOMPS_Sequence*)self)->it_info->out_convert_func(it->comps_obj);
            break;
        }
    }
    if (!ret) {
        PyErr_Format(PyExc_KeyError, "Object with id '%s' is not in list", strid);
    }
    if (PyUnicode_Check(id)) {
        free(strid);
    }
    COMPS_OBJECT_DESTROY(tmpstr);
    return ret;
    #undef _seq_
}

inline COMPS_Object *list_setitem_convert(PyObject *self, PyObject *item) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    COMPS_Object *ret = NULL;
    if (!item)
        return NULL;
    for (unsigned i = 0; i < _seq_->it_info->item_types_len; i++) {
        if (Py_TYPE(item) != _seq_->it_info->itemtypes[i])
            continue;
        if (_seq_->it_info->in_convert_funcs[i]) {
            ret = _seq_->it_info->in_convert_funcs[i](item);
            break;
        }
    }
    return ret;
    #undef _seq_
}

inline int list_setitem(PyObject *self, Py_ssize_t index, PyObject *item) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    COMPS_Object *converted_item = list_setitem_convert(self, item);
    if (!converted_item && item) {
        PyErr_Format(PyExc_TypeError, "Cannot set %s to %s",
                     Py_TYPE(item)->tp_name,
                     Py_TYPE(self)->tp_name);
        return -1;
    }

    if ((int)index > (int)(_seq_->list->len-1)) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return -1;
    }
    if (item != NULL) {
        if (_seq_->it_info->pre_checker &&
            _seq_->it_info->pre_checker(converted_item)) {
                COMPS_OBJECT_DESTROY(converted_item);
                return -1;
        }
        comps_objlist_set(_seq_->list, index, converted_item);
    } else {
        comps_objlist_remove_at(_seq_->list, index);
    }
    return 0;
    #undef _seq_
}

int list_unique_id_check(PyObject *self, COMPS_Object *converted) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    COMPS_ObjDict *props1, *props2;
    COMPS_Object *strid1, *strid2;

    props1 = (COMPS_ObjDict*)GET_FROM(converted,
                                     _seq_->it_info->props_offset);
    strid1 = comps_objdict_get_x(props1, "id");
    for (COMPS_ObjListIt *it = _seq_->list->first; it != NULL;
         it = it->next) {
        props2 = (COMPS_ObjDict*)GET_FROM(it->comps_obj,
                                          _seq_->it_info->props_offset);
        strid2 = comps_objdict_get_x(props2, "id");

        if (comps_object_cmp(strid1, strid2)) {
            char *cstrid;
            cstrid = comps_object_tostr(strid1);
            PyErr_Format(PyExc_KeyError, "Object with id '%s' already "
                                         "exists in list", cstrid);
            free(cstrid);
            return -1;
        }
    }
    return 0;
}

inline int list_setitem_id_unique(PyObject *self,
                                  Py_ssize_t index,
                                  PyObject *item) {
    #define _seq_ ((PyCOMPS_Sequence*)self)

    COMPS_Object *converted_item = list_setitem_convert(self, item);
    if (!converted_item && item) {
        PyErr_Format(PyExc_TypeError, "Cannot set %s to %s",
                     Py_TYPE(item)->tp_name,
                     Py_TYPE(self)->tp_name);
        return -1;
    }

    if ((int)index > (int)(_seq_->list->len-1)) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return -1;
    }
    if (!item) {
        comps_objlist_remove_at(_seq_->list, index);
        return 0;
    }
    if (list_unique_id_check(self, converted_item)) {
        COMPS_OBJECT_DESTROY(converted_item);
        return -1;
    }
    comps_objlist_set(_seq_->list, index, converted_item);
    return 0;
    #undef _seq_
}

PyObject* list_concat(PyObject *self, PyObject *other) {
    COMPS_ObjListIt *it;
    PyCOMPS_Sequence *result;
    //PyErr_SetString(PyExc_TypeError, "list concat");

    if (Py_TYPE(self) != Py_TYPE(other)) {
        PyErr_SetString(PyExc_TypeError, "different object class");
        return NULL;
    }

    result = (PyCOMPS_Sequence*)Py_TYPE(self)->tp_new(self->ob_type, NULL, NULL);
    Py_TYPE(self)->tp_init((PyObject*)result, NULL, NULL);

    it = ((PyCOMPS_Sequence*)self)->list->first;

    for (; it != NULL; it = it->next){
        comps_objlist_append(result->list, it->comps_obj);
    }

    it = ((PyCOMPS_Sequence*)other)->list->first;
    for (; it != NULL; it = it->next){
        comps_objlist_append(result->list, it->comps_obj);
    }
    return (PyObject*)result;
}

PyObject* list_repeat(PyObject *self, Py_ssize_t count) {
    COMPS_ObjListIt *it, *end;
    //PyObject *data;
    int i;
    PyCOMPS_Sequence *result;

    result = (PyCOMPS_Sequence*)self->ob_type->tp_new(self->ob_type, NULL, NULL);
    self->ob_type->tp_init((PyObject*)result, NULL, NULL);

    end = ((PyCOMPS_Sequence*)self)->list->last;
    for (i = 0; i < count; i++) {
        for (it = ((PyCOMPS_Sequence*)self)->list->first;
             it != end->next; it = it->next) {
            comps_objlist_append(result->list, it->comps_obj);
        }
    }
    return (PyObject*)result;
}

PyObject* list_get_slice(PyObject *self, PyObject *key) {
    PyCOMPS_Sequence *result;
    COMPS_ObjListIt *it;
    int i;
    unsigned int n, uret;
    Py_ssize_t istart, istop, istep, ilen, clen;

    n = ((PyCOMPS_Sequence*)self)->list->len;
    result = (PyCOMPS_Sequence*)Py_TYPE((PyCOMPS_Sequence*)self)->tp_new(
                                                      Py_TYPE(self),
                                                      NULL, NULL);
    self->ob_type->tp_init((PyObject*)result, NULL, NULL);
    uret = PySlice_GetIndicesEx((PyObject*)key, n,
                               &istart, &istop, &istep, &ilen);

    if (uret) {
        return NULL;
    }

    clen = 0;
    it = ((PyCOMPS_Sequence*)self)->list->first;
    if (!it)
        return (PyObject*)result;
    for (i=0 ; i<istart; it=it->next, i++);
    while (clen != ilen) {
        comps_objlist_append(result->list, it->comps_obj);
        clen+=1;
        i = 0;
        do {
            for (; i<istep && it != NULL; it=it->next, i++);
            if (!it) it = ((PyCOMPS_Sequence*)self)->list->first;
        } while (i < istep);
    }
    return (PyObject*)result;
}

PyObject* PyCOMPSSeq_get(PyObject *self, PyObject *key) {
    if (PySlice_Check(key)) {
        return list_get_slice(self, key);
    } else if (PyLong_Check(key)) {
        return list_getitem(self, PyLong_AsLong(key));
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice");
        return NULL;
    }
}

PyObject* PyCOMPSSeq_id_get(PyObject *self, PyObject *key) {
    if (PySlice_Check(key)) {
        return list_get_slice(self, key);
    } else if (PyLong_Check(key)) {
        return list_getitem(self, PyLong_AsLong(key));
    } else if (PyUnicode_Check(key) || PyBytes_Check(key)){
        return list_getitem_byid(self, key);
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice"
                                         "or string id");
        return NULL;
    }
}

int list_set_slice(PyObject *self, PyObject *key, PyObject *val) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    COMPS_ObjListIt *it, *it2;

    unsigned int n, uret;
    Py_ssize_t istart, istop, istep, ilen, i, c, clen;
    if (PySlice_Check(key)) {
        n = _seq_->list->len;
        uret = PySlice_GetIndicesEx((PyObject*)key, n,
                                   &istart, &istop, &istep, &ilen);
        if (uret) return -1;
        if (ilen == 0) {
            uret = PySlice_GetIndicesEx((PyObject*)key, n+istart,
                                       &istart, &istop, &istep, &ilen);
        }
        if (uret) return -1;
        if (val) {
            // set val for list items indexed by given slice
            if (Py_TYPE(self) != Py_TYPE(val)) {
                PyErr_SetString(PyExc_TypeError, "different object class");
                return -1;
            }

            if (istep != 1 && n != (unsigned int)ilen) {
                PyErr_Format(PyExc_ValueError,
                             "attempt to assign sequence of size %d to extended"
                             "slice of size %d",
                             n, (unsigned int)ilen);
                return -1;
            }
            COMPS_ObjListIt *it;
            it = ((PyCOMPS_Sequence*)val)->list->first;
            for (; it != NULL; it = it->next) {
                if (list_unique_id_check(self, it->comps_obj)) {
                    return -1;
                }
            }


            clen = 0;
            it = _seq_->list->first;
            it2 = ((PyCOMPS_Sequence*)val)->list->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            if (istep != 1) {
                while (clen != ilen) {
                    if (!it) {
                        PyErr_SetString(PyExc_ValueError,
                                        "failed to index list using the given slice");
                        return -1;
                    }
                    COMPS_OBJECT_DESTROY(it->comps_obj);
                    it->comps_obj = comps_object_incref(it2->comps_obj);
                    clen += 1;
                    it2 = it2->next;
                    for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                    if (!it) it = _seq_->list->first;
                    for (; i<istep; it=it->next, i++);
                }
            } else {
                if (istart < 0) istart += n;
                if (istop < 0) istop += n;

                for (; it2 != NULL && it != NULL;
                       it2 = it2->next, it = it->next, i++) {
                    COMPS_OBJECT_DESTROY(it->comps_obj);
                    it->comps_obj = comps_object_incref(it2->comps_obj);
                }
                if (it == NULL) {
                    for (;it2 != NULL; it2 = it2->next) {
                        comps_objlist_append(_seq_->list, it2->comps_obj);
                    }
                }
                if (it != NULL) {
                    for (c = i; c < istop; c++) {
                        comps_objlist_remove_at(_seq_->list, i);
                    }
                }
            }
            return 0;
        } else {
            // if val is NULL we want to delete list items indexed by given slice
            clen = 0;
            it = _seq_->list->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            while (clen != ilen) {
                if (!it) {
                    PyErr_SetString(PyExc_ValueError,
                                    "failed to index list using the given slice");
                    return -1;
                }
                if (it->comps_obj) {
                    COMPS_OBJECT_DESTROY(it->comps_obj);
                    it->comps_obj = NULL;
                }
                clen+=1;
                for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                if (!it) it = _seq_->list->first;
                for (; i<istep; it=it->next, i++);
            }
            it2 = NULL;
            for (i=0, it = _seq_->list->first;
                 it != NULL; it2 = it, it = it->next, i++) {
                if (it2 && !it2->comps_obj) {
                    comps_objlist_remove_at(_seq_->list, i);
                }
            }
            if (it2 && !it2->comps_obj) {
                comps_objlist_remove_at(_seq_->list, i);
            }
            return 0;
        }
    }
    return 0;
    #undef _seq_
}

int __PyCOMPSSeq_set(PyObject *self, PyObject *key, PyObject *val,
                     int (*setter)(PyObject*, Py_ssize_t, PyObject*)) {
    if (PySlice_Check(key)) {
        return list_set_slice(self, key, val);
    } else if (PyLong_Check(key)) {
        return setter(self, PyLong_AsLong(key), val);
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice");
        return -1;
    }
}

int PyCOMPSSeq_set(PyObject *self, PyObject *key, PyObject *val) {
    return __PyCOMPSSeq_set(self, key, val, list_setitem);
}

int PyCOMPSSeq_set_unique(PyObject *self, PyObject *key, PyObject *val) {
    return __PyCOMPSSeq_set(self, key, val, list_setitem_id_unique);
}

PyObject* PyCOMPSSeq_append(PyObject * self, PyObject *item) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    COMPS_Object *converted_item = list_setitem_convert(self, item);
    if (!converted_item) {
        PyErr_Format(PyExc_TypeError, "Cannot append %s to %s",
                      Py_TYPE(item)->tp_name,
                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    if (_seq_->it_info->pre_checker &&
        _seq_->it_info->pre_checker(converted_item)) {
        COMPS_OBJECT_DESTROY(converted_item);
        return NULL;
    }
    comps_objlist_append_x(_seq_->list, converted_item);
    Py_RETURN_NONE;
    #undef _seq_
}

PyObject* PyCOMPSSeq_insert(PyObject * self, PyObject *args) {
    #define _seq_ ((PyCOMPS_Sequence*)self)

    int index;
    PyObject *item;
    if (!PyArg_ParseTuple(args, "|iO", &index, &item))
        return NULL;

    COMPS_Object *converted_item = list_setitem_convert(self, item);
    if (!converted_item) {
        PyErr_Format(PyExc_TypeError, "Cannot insert %s to %s",
                     Py_TYPE(item)->tp_name, Py_TYPE(self)->tp_name);
        return NULL;
    }
    if (_seq_->it_info->pre_checker &&
        _seq_->it_info->pre_checker(converted_item)) {
        COMPS_OBJECT_DESTROY(converted_item);
        return NULL;
    }
    if (index<0) {
        index = index + _seq_->list->len;
        if (index < 0) {
            index = 0;
        }
    } else if (index > (int)_seq_->list->len) {
        index = _seq_->list->len;
    }
    comps_objlist_insert_at_x(_seq_->list, index, converted_item);
    Py_RETURN_NONE;
    #undef _seq_
}

PyObject* PyCOMPSSeq_append_unique(PyObject * self, PyObject *item) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    COMPS_Object *converted_item = list_setitem_convert(self, item);
    if (!converted_item) {
        PyErr_Format(PyExc_TypeError, "Cannot append %s to %s",
                      Py_TYPE(item)->tp_name,
                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    if (list_unique_id_check(self, converted_item)) {
        COMPS_OBJECT_DESTROY(converted_item);
        return NULL;
    }
    if (_seq_->it_info->pre_checker &&
        _seq_->it_info->pre_checker(converted_item)) {
        COMPS_OBJECT_DESTROY(converted_item);
        return NULL;
    }
    comps_objlist_append_x(_seq_->list, converted_item);
    Py_RETURN_NONE;
    #undef _seq_
}

PyObject* PyCOMPSSeq_remove(PyObject * self, PyObject *item) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    int i;
    COMPS_Object *converted_item = list_setitem_convert(self, item);
    if (!converted_item) {
        PyErr_Format(PyExc_TypeError, "Cannot remove %s from %s",
                      Py_TYPE(item)->tp_name,
                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    i = comps_objlist_remove(_seq_->list, converted_item);
    if (!i) {
        char *tmpstr;
        tmpstr = comps_object_tostr(converted_item);
        PyErr_Format(PyExc_ValueError, "Canot remove %s. Not in list", tmpstr);
        free(tmpstr);
        return NULL;
    }
    Py_RETURN_NONE;
    #undef _seq_
}

void PyCOMPSSeq_dealloc(PyCOMPS_Sequence *self)
{
    COMPS_OBJECT_DESTROY(self->list);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSSeq_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;

    PyCOMPS_Sequence *self;

    self = (PyCOMPS_Sequence*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->list = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);
    } else return NULL;
    self->it_info = NULL;
    return (PyObject*) self;
}

/*int PyCOMPSSeq_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds)
{
    (void)self;
    (void)args;
    (void)kwds;
    return 0;
}*/

PyObject* PyCOMPSSeq_str(PyObject *self) {
    return PyUnicode_FromString(comps_object_tostr(
                            (COMPS_Object*)((PyCOMPS_Sequence*)self)->list));
}

PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op) {
    char res;
    res = COMPS_OBJECT_CMP(((PyCOMPS_Sequence*)self)->list,
                           ((PyCOMPS_Sequence*)other)->list);
    if (op == Py_EQ && res) {
        Py_RETURN_TRUE;
    } else if (op == Py_NE && !res){
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

PyObject* PyCOMPSSeq_getiter(PyObject *self) {
    PyObject *res;
    res = PyCOMPSSeqIter_new(&PyCOMPS_SeqIterType, NULL, NULL);
    ((PyCOMPS_SeqIter*)res)->it = ((PyCOMPS_Sequence*)self)->list->first;
    Py_INCREF((PyCOMPS_Sequence*)self);
    ((PyCOMPS_SeqIter*)res)->seq = (PyCOMPS_Sequence*)self;
    return res;
}

PyObject* PyCOMPSSeq_iternext(PyObject *iter_o) {
    COMPS_Object *ret;
    PyObject *retp;
    PyCOMPS_SeqIter *iter = ((PyCOMPS_SeqIter*)iter_o);

    ret = iter->it?iter->it->comps_obj: NULL;
    if (ret) {
        ret = comps_object_incref(ret);
        retp = iter->seq->it_info->out_convert_func(ret);
        iter->it = iter->it->next;
        return retp;
    }
    return NULL;
}

PyObject* PyCOMPSSeq_clear(PyObject *self) {
    comps_objlist_clear(((PyCOMPS_Sequence*)self)->list);
    Py_RETURN_NONE;
}


static PyMappingMethods PyCOMPSSeq_mapping = {
    PyCOMPSSeq_len,
    PyCOMPSSeq_get,
    PyCOMPSSeq_set
};

PyMappingMethods PyCOMPSSeq_mapping_extra = {
    PyCOMPSSeq_len,
    PyCOMPSSeq_id_get,
    PyCOMPSSeq_set_unique
};


PyMemberDef PyCOMPSSeq_members[] = {
    {NULL}};

PyDoc_STRVAR(PyCOMPSSeq_insert__doc__,
             "insert(pos, item) -> None\n"
             "Insert item into list at position pos"
             "\n\n"
             ":param pos: int"
             "position\n"
             "\n"
             ":returns: None\n");

PyDoc_STRVAR(PyCOMPSSeq_remove__doc__,
             "remove(item) -> None\n"
             "Remove first occurence of item from list"
             "\n\n"
             ":raise TypeError: If type of item is not type acceptable for list\n"
             ":raise ValueError: If item is not in the list\n");

PyMethodDef PyCOMPSSeq_methods[] = {
     {"append", (PyCFunction)PyCOMPSSeq_append, METH_O,
     "Append item to new of the list"},
     {"remove", (PyCFunction)PyCOMPSSeq_remove, METH_O,
     PyCOMPSSeq_remove__doc__},
     {"clear", (PyCFunction)PyCOMPSSeq_clear, METH_NOARGS,
     "Clear the list"},
     {"insert", (PyCFunction)PyCOMPSSeq_insert, METH_VARARGS,
     PyCOMPSSeq_insert__doc__},
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_SeqType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.Sequence",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &PyCOMPSSeq_mapping,       /*tp_as_mapping*/
    &PyCOMPS_hash,             /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSSeq_str,            /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Comps Sequence",           /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    &PyCOMPSSeq_cmp,            /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    PyCOMPSSeq_getiter,         /* tp_iter */
    0,                          /* tp_iternext */
    PyCOMPSSeq_methods,         /* tp_methods */
    PyCOMPSSeq_members,         /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    0,//(initproc)PyCOMPSSeq_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSSeq_new,                 /* tp_new */};

PyTypeObject PyCOMPS_SeqItemType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    };

PyMemberDef PyCOMPSSeqIter_members[] = {
    {NULL}};

PyMethodDef PyCOMPSSeqIter_methods[] = {
    {NULL}  /* Sentinel */
};

PyObject* PyCOMPSSeqIter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;

    PyCOMPS_SeqIter *self;
    self = (PyCOMPS_SeqIter*) type->tp_alloc(type, 0);
    return (PyObject*) self;
}

int PyCOMPSSeqIter_init(PyCOMPS_SeqIter *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    self->it = NULL;
    self->seq = NULL;
    return 0;
}

void PyCOMPSSeqIter_dealloc(PyObject *self)
{
    #define _iter_ ((PyCOMPS_SeqIter*)self)
    Py_XDECREF(_iter_->seq);
    Py_TYPE(self)->tp_free((PyObject*)self);
    #undef _iter_
}

PyTypeObject PyCOMPS_SeqIterType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.SeqIter",   /*tp_name*/
    sizeof(PyCOMPS_SeqIter), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    &PyCOMPSSeqIter_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
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
    "Comps Sequence Iterator",           /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    PyObject_SelfIter,          /* tp_iter */
    PyCOMPSSeq_iternext,         /* tp_iternext */
    PyCOMPSSeqIter_methods,         /* tp_methods */
    PyCOMPSSeqIter_members,         /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSSeqIter_init,  /* tp_init */
    0,                              /* tp_alloc */
    PyCOMPSSeqIter_new,             /* tp_new */};

