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

#include <Python.h>
#include "pycomps_dict.h"

PyObject* __pycomps_dict_val_out(COMPS_HSListItem *hsit) {
    char *str = comps_object_tostr((COMPS_Object*)hsit->data);
    PyObject *ret;
    ret = PyUnicode_FromString(str);
    free(str);
    return ret;
}

PyObject* __pycomps_dict_pair_out(COMPS_HSListItem *hsit) {
    PyObject *key, *val, *tuple;
    char *x;

    key = PyUnicode_FromString((char*) ((COMPS_ObjRTreePair*)hsit->data)->key);
    x = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
    val = PyUnicode_FromString(x);
    free(x);
    tuple = PyTuple_Pack(2, key, val);
    Py_DECREF(key);
    Py_DECREF(val);
    return tuple;
}

void PyCOMPSDict_dealloc(PyCOMPS_Dict *self)
{
    COMPS_OBJECT_DESTROY(self->dict);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;

    PyCOMPS_Dict *self;

    self = (PyCOMPS_Dict*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->dict = COMPS_OBJECT_CREATE(COMPS_ObjDict, NULL);
    }
    return (PyObject*) self;
}

int PyCOMPSDict_init(PyCOMPS_Dict *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    (void)self;
    return 0;
}

PyObject* PyCOMPSDict_str(PyObject *self) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    PyObject *ret, *tmp = NULL, *tmp2 = NULL, *tmpkey = NULL, *tmpval = NULL;
    ret = PyUnicode_FromString("{");
    pairlist = comps_objdict_pairs(((PyCOMPS_Dict*)self)->dict);
    char *tmpstr;

    for (it = pairlist->first; it != NULL; it = it->next) {
        tmp = ret;
        tmpkey = __pycomps_lang_decode(((COMPS_ObjRTreePair*)it->data)->key);
        if (!tmpkey) {
            PyErr_SetString(PyExc_TypeError, "key convert error");
            goto out;
        }
        tmpstr = comps_object_tostr(((COMPS_ObjRTreePair*)it->data)->data);
        tmpval = __pycomps_lang_decode(tmpstr);
        free(tmpstr);
        if (!tmpval) {
            PyErr_SetString(PyExc_TypeError, "val convert error");
            goto out;
        }
        tmp2 = PyUnicode_FromFormat("%U = '%U', ", tmpkey, tmpval);
        ret = PyUnicode_Concat(ret, tmp2);
        Py_XDECREF(tmp);
        Py_XDECREF(tmp2);
        Py_XDECREF(tmpkey);
        Py_XDECREF(tmpval);
    }

    tmp = ret;
    tmp2 = PyUnicode_FromString("}");
    ret = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp);
    Py_XDECREF(tmp2);

    comps_hslist_destroy(&pairlist);
    return ret;

    out:
    Py_XDECREF(tmp);
    Py_XDECREF(tmp2);
    Py_XDECREF(tmpkey);
    Py_XDECREF(tmpval);
    comps_hslist_destroy(&pairlist);
    return NULL;
}

PyObject* PyCOMPSDict_cmp(PyObject *self, PyObject *other, int op) {
    char ret;
    if (other == NULL) {
        PyErr_Format(PyExc_TypeError, "Get NULL as Dict subclass");
        return NULL;
    }
    if ((Py_TYPE(other) != Py_TYPE(self) &&
         !PyType_IsSubtype(Py_TYPE(other), Py_TYPE(self)))) {
        PyErr_Format(PyExc_TypeError, "Not Dict subclass, %s",
                                      Py_TYPE(other)->tp_name);
        return NULL;
    }
    if (op != Py_EQ && op != Py_NE) {
        PyErr_Format(PyExc_TypeError, "Unsuported operator");
        return Py_INCREF(Py_NotImplemented), Py_NotImplemented;
    }
    ret = comps_object_cmp((COMPS_Object*)((PyCOMPS_Dict*)self)->dict,
                           (COMPS_Object*)((PyCOMPS_Dict*)other)->dict);
    if ((!ret && op == Py_NE) || (ret && op == Py_EQ)) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

Py_ssize_t PyCOMPSDict_len(PyObject *self) {
    return ((PyCOMPS_Dict*)self)->dict->len;
}
PyObject* PyCOMPSDict_clear(PyObject *self) {
    comps_objrtree_clear(((PyCOMPS_Dict*)self)->dict);
    Py_RETURN_NONE;
}
PyObject* PyCOMPSDict_copy(PyObject *self) {
    PyObject *ret;
    ret = PyCOMPSDict_new(Py_TYPE(self), NULL, NULL);
    Py_TYPE(self)->tp_init(ret, NULL, NULL);
    comps_objrtree_copy_shallow(((PyCOMPS_Dict*)ret)->dict,
                                ((PyCOMPS_Dict*)self)->dict);
    return ret;
}
PyObject* PyCOMPSDict_update(PyObject *self, PyObject *other) {
    if (other == NULL) {
        PyErr_Format(PyExc_TypeError, "Get NULL as Dict subclass");
        return NULL;
    }
    if ((Py_TYPE(other) != Py_TYPE(self) &&
         !PyType_IsSubtype(Py_TYPE(other), Py_TYPE(self)))) {
        PyErr_Format(PyExc_TypeError, "Not %s type or subclass, %s",
                     Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
        return NULL;
    }
    comps_objrtree_unite(((PyCOMPS_Dict*)self)->dict,
                           ((PyCOMPS_Dict*)other)->dict);
    Py_RETURN_NONE;
}

PyObject* PyCOMPSDict_get(PyObject *self, PyObject *key) {
    char *ckey;
    COMPS_Object* val;
    PyObject *ret = NULL;

    if (__pycomps_stringable_to_char(key, &ckey)) {
        return NULL;
    }
    val = comps_objdict_get(((PyCOMPS_Dict*)self)->dict, ckey);
    if (!val) {
        PyErr_Format(PyExc_KeyError, "KeyError: '%s'", ckey);
    }
    else {
        ret = ((PyCOMPS_Dict*)self)->it_info->out_convert_func(val);
        COMPS_OBJECT_DESTROY(val);
    }
    free(ckey);
    return ret;
}

PyObject* PyCOMPSDict_get_(PyObject *self, PyObject *key) {
    char *ckey;
    COMPS_Object* val;
    PyObject *ret;

    if (__pycomps_stringable_to_char(key, &ckey))
        return NULL;

    val = comps_objdict_get(((PyCOMPS_Dict*)self)->dict, ckey);
    if (!val) {
        free(ckey);
        Py_RETURN_NONE;
    }
    else {
        free(ckey);
        ckey = comps_object_tostr(val);
        COMPS_OBJECT_DESTROY(val);
        ret = PyUnicode_FromString(ckey);
        free(ckey);
        return ret;
    }
}

#define _DICT_ ((PyCOMPS_Dict*)self)
#define _INFO_ ((PyCOMPS_Dict*)self)->it_info
int PyCOMPSDict_set(PyObject *self, PyObject *key, PyObject *val) {
    char *ckey;
    COMPS_Object *ret = NULL;
    for (unsigned i = 0; i < _INFO_->item_types_len; i++) {
        if (Py_TYPE(val) != _INFO_->itemtypes[i])
            continue;
        if (_INFO_->in_convert_funcs[i]) {
            ret = _INFO_->in_convert_funcs[i](val);
            break;
        }
    }
    if (__pycomps_stringable_to_char(key, &ckey))
        return -1;

    if (!ret && val) {
        PyErr_Format(PyExc_TypeError, "Cannot set %s to %s",
                     Py_TYPE(val)->tp_name,
                     Py_TYPE(self)->tp_name);
        free(ckey);
        return -1;
    } else if (val)
        comps_objdict_set_x(_DICT_->dict, ckey, ret);
    else if (!val)
        comps_objdict_unset(_DICT_->dict, ckey);

    free(ckey);
    return 0;
}

PyObject* PyCOMPSDict_keys(PyObject * self, PyObject *args) {
    (void)args;
    PyObject *ret, *item;
    COMPS_HSList *list = comps_objrtree_keys(_DICT_->dict);

    ret = PyList_New(0);
    for (COMPS_HSListItem *it = list->first; it != NULL; it = it->next) {
        item = PyUnicode_FromString(it->data);
        PyList_Append(ret, item);
        Py_DECREF(item);
    }
    comps_hslist_destroy(&list);
    return ret;
}

PyObject* PyCOMPSDict_values(PyObject * self, PyObject *args) {
    (void)args;
    PyObject *ret, *item;
    COMPS_HSList *list = comps_objrtree_values(_DICT_->dict);

    ret = PyList_New(0);
    for (COMPS_HSListItem *it = list->first; it != NULL; it = it->next) {
        item = _INFO_->out_convert_func(it->data);
        PyList_Append(ret, item);
        Py_DECREF(item);
    }
    comps_hslist_destroy(&list);
    return ret;
}

PyObject* PyCOMPSDict_items(PyObject * self, PyObject *args) {
    (void)args;
    PyObject *ret, *k, *v, *tp;
    COMPS_HSList *list = comps_objrtree_pairs(((PyCOMPS_Dict*)self)->dict);

    ret = PyList_New(0);
    for (COMPS_HSListItem *it = list->first; it != NULL; it = it->next) {
        k = PyUnicode_FromString(((COMPS_ObjRTreePair*)it->data)->key);
        v = _INFO_->out_convert_func(((COMPS_ObjRTreePair*)it->data)->data);
        tp = PyTuple_Pack(2, k, v);
        Py_DECREF(k);
        Py_DECREF(v);
        PyList_Append(ret, tp);
        Py_DECREF(tp);
    }
    comps_hslist_destroy(&list);
    return ret;
}
#undef _DICT_
#undef _INFO_


PyObject* PyCOMPSDict_has_key(PyObject * self, PyObject *key) {
    char *ckey;
    COMPS_Object *val;

    if (__pycomps_stringable_to_char(key, &ckey)) {
        return NULL;
    }
    val = comps_objdict_get(((PyCOMPS_Dict*)self)->dict, ckey);
    free(ckey);
    if (!val)
        Py_RETURN_FALSE;
    else
        Py_RETURN_TRUE;
}

PyObject* PyCOMPSDict_getiter(PyObject *self) {
    PyObject *res;
    res = PyCOMPSDictIter_new(&PyCOMPS_DictIterType, NULL, NULL);
    PyCOMPSDictIter_init((PyCOMPS_DictIter*)res, NULL, NULL);
    ((PyCOMPS_DictIter*)res)->hslist = comps_objdict_keys(((PyCOMPS_Dict*)self)->dict);
    ((PyCOMPS_DictIter*)res)->hsit = ((PyCOMPS_DictIter*)res)->hslist->first;
    ((PyCOMPS_DictIter*)res)->out_func = &__pycomps_dict_key_out;
    return res;
}

PyObject* PyCOMPSDict_getiteritems(PyObject *self) {
    PyObject *res;
    res = PyCOMPSDictIter_new(&PyCOMPS_DictIterType, NULL, NULL);
    PyCOMPSDictIter_init((PyCOMPS_DictIter*)res, NULL, NULL);
    ((PyCOMPS_DictIter*)res)->hslist = comps_objdict_pairs(((PyCOMPS_Dict*)self)->dict);
    ((PyCOMPS_DictIter*)res)->hsit = ((PyCOMPS_DictIter*)res)->hslist->first;
    ((PyCOMPS_DictIter*)res)->out_func = &__pycomps_dict_pair_out;
    return res;
}

PyObject* PyCOMPSDict_getitervalues(PyObject *self) {
    PyObject *res;
    res = PyCOMPSDictIter_new(&PyCOMPS_DictIterType, NULL, NULL);
    PyCOMPSDictIter_init((PyCOMPS_DictIter*)res, NULL, NULL);
    ((PyCOMPS_DictIter*)res)->hslist = comps_objdict_values(((PyCOMPS_Dict*)self)->dict);
    ((PyCOMPS_DictIter*)res)->hsit = ((PyCOMPS_DictIter*)res)->hslist->first;
    ((PyCOMPS_DictIter*)res)->out_func = &__pycomps_dict_val_out;
    return res;
}

PyMappingMethods PyCOMPSDict_mapping = {
    PyCOMPSDict_len,
    PyCOMPSDict_get,
    PyCOMPSDict_set
};

PyMemberDef PyCOMPSDict_members[] = {
    {NULL}};

PyDoc_STRVAR(PyCOMPSDict_get__doc__,
             "get(key)->object\n"
             "Return object associated with key\n"
             "\n"
             ":param str/unicode key: object key\n"
             "\n"
             ":returns: object if there's object associated with key\n\n"
             "          None otherwise\n");

PyDoc_STRVAR(PyCOMPSDict_has_key__doc__,
             "has_key(key)->bool\n"
             "Tests if there's key in object\n"
             "\n"
             ":param str/unicode key: object key\n"
             "\n"
             ":returns: True if there's object associated with key\n\n"
             "          False otherwise\n");
PyDoc_STRVAR(PyCOMPSDict_update__doc__,
             "update(dict)->None\n"
             "Update dictionary with (key,value) pair from another dictionary."
             "Existing pairs are overwritten\n"
             "\n"
             ":param dict: :py:class:`libcomps.Dict`"
             " instance or subclass instance\n"
             "\n"
             ":returns: None\n");

PyMethodDef PyCOMPSDict_methods[] = {
     {"get", (PyCFunction)PyCOMPSDict_get_, METH_O, PyCOMPSDict_get__doc__},
     {"has_key", (PyCFunction)PyCOMPSDict_has_key, METH_O,
       PyCOMPSDict_has_key__doc__},
     {"items", (PyCFunction)PyCOMPSDict_getiteritems, METH_NOARGS,
     "return iterator returning (key, value) tuple"},
     {"values", (PyCFunction)PyCOMPSDict_getitervalues, METH_NOARGS,
     "return iterator returning item's value"},
     {"keys", (PyCFunction)PyCOMPSDict_getiter, METH_NOARGS,
     "return iterator returning item's key"},
     {"clear", (PyCFunction)PyCOMPSDict_clear, METH_NOARGS,
     "clear the dict"},
     {"copy", (PyCFunction)PyCOMPSDict_copy, METH_NOARGS,
     "return shallow copy of dict"},
     {"update", (PyCFunction)PyCOMPSDict_update, METH_O,
      PyCOMPSDict_update__doc__},
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_DictType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.Dict",            /*tp_name*/
    sizeof(PyCOMPS_Dict),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)PyCOMPSDict_dealloc, /*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    &PyCOMPSDict_mapping,       /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    PyCOMPSDict_str,            /*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Comps Dict",               /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    &PyCOMPSDict_cmp,           /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    &PyCOMPSDict_getiter,       /* tp_iter */
    0,                          /* tp_iternext */
    PyCOMPSDict_methods,        /* tp_methods */
    PyCOMPSDict_members,        /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSDict_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSDict_new,                 /* tp_new */};


PyMemberDef PyCOMPSDictIter_members[] = {
    {NULL}};

PyMethodDef PyCOMPSDictIter_methods[] = {
    {NULL}  /* Sentinel */
};

PyObject* PyCOMPSDictIter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    PyCOMPS_DictIter *self;
    self = (PyCOMPS_DictIter*)type->tp_alloc(type, 0);
    self->hslist = NULL;
    self->hsit = NULL;
    return (PyObject*) self;
}

void PyCOMPSDictIter_dealloc(PyCOMPS_DictIter *self)
{
    comps_hslist_destroy(&self->hslist);
    COMPS_OBJECT_DESTROY(self->objlist);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSDict_iternext(PyObject *iter_o) {
    void *ret;
    PyObject *retp;
    PyCOMPS_DictIter *iter = ((PyCOMPS_DictIter*)iter_o);
    ret = iter->hsit?iter->hsit->data: NULL;
    if (ret) {
        retp = iter->out_func(iter->hsit);
        iter->hsit = iter->hsit->next;
        return retp;
    }
    return NULL;
}

int PyCOMPSDictIter_init(PyCOMPS_DictIter *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    self->hsit = NULL;
    self->hslist = NULL;
    self->objlist = NULL;
    self->it = NULL;
    return 0;
}

PyTypeObject PyCOMPS_DictIterType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.DictIter",   /*tp_name*/
    sizeof(PyCOMPS_DictIter), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSDictIter_dealloc, /*tp_dealloc*/
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
    "Comps Dict Iterator",           /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,         /* tp_iter */
    PyCOMPSDict_iternext,         /* tp_iternext */
    PyCOMPSDictIter_methods,         /* tp_methods */
    PyCOMPSDictIter_members,         /* tp_members */
    0,                          /* tp_getset */
    &PySeqIter_Type,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSDictIter_init,  /* tp_init */
    0,                              /* tp_alloc */
    PyCOMPSDictIter_new,             /* tp_new */};


/*PyMemberDef PyCOMPSStrDict_members[] = {
    {NULL}};

PyMethodDef PyCOMPSStrDict_methods[] = {
    {NULL}
};*/

PyCOMPS_ItemInfo PyCOMPS_StrDictInfo = {
    .itemtypes = (PyTypeObject*[]){&PyUnicode_Type, &PyBytes_Type},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&__pycomps_unicode_in, &__pycomps_bytes_in},
    .out_convert_func = &__pycomps_str_out,
    .item_types_len = 2
};

int PyCOMPSStrDict_init(PyCOMPS_Dict *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    self->it_info = &PyCOMPS_StrDictInfo;
    return 0;
}

PyTypeObject PyCOMPS_StrDictType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.StrDict",            /*tp_name*/
    sizeof(PyCOMPS_Dict),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    0,                          /*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    0,                          /*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,// | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Str Dict",               /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    0,//PyCOMPSStrDict_methods,        /* tp_methods */
    0,//PyCOMPSStrDict_members,        /* tp_members */
    0,                          /* tp_getset */
    &PyCOMPS_DictType,           /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSStrDict_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSDict_new,                 /* tp_new */};
