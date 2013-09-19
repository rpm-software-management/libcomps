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


PyObject* __pycomps_dict_key_out(COMPS_HSListItem *hsit) {
    return PyUnicode_FromString((char*)hsit->data);
}

PyObject* __pycomps_dict_pair_out(COMPS_HSListItem *hsit) {
    PyObject *key, *val, *tuple;
    key = PyUnicode_FromString((char*) ((COMPS_ObjRTreePair*)hsit->data)->key);
    val = PyUnicode_FromString((char*) ((COMPS_ObjRTreePair*)hsit->data)->data);
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
        self->dict = (COMPS_ObjDict*)comps_object_create(&COMPS_ObjDict_ObjInfo,
                                                         NULL);
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
    PyObject *ret, *tmp, *tmp2, *tmpkey, *tmpval;
    ret = PyUnicode_FromString("{");
    pairlist = comps_objdict_pairs(((PyCOMPS_Dict*)self)->dict);

    for (it = pairlist->first; it != pairlist->last; it = it->next) {
        tmp = ret;
        tmpkey = __pycomps_lang_decode(((COMPS_ObjRTreePair*)it->data)->key);
        if (!tmpkey) {
            PyErr_SetString(PyExc_TypeError, "key convert error");
            return NULL;
        }
        tmpval = __pycomps_lang_decode((char*)((COMPS_ObjRTreePair*)it->data)->data);
        if (!tmpval) {
            PyErr_SetString(PyExc_TypeError, "val convert error");
            return NULL;
        }
        tmp2 = PyUnicode_FromFormat("%U = '%U', ", tmpkey, tmpval);
        ret = PyUnicode_Concat(ret, tmp2);
        Py_XDECREF(tmp);
        Py_XDECREF(tmp2);
        Py_XDECREF(tmpkey);
        Py_XDECREF(tmpval);
    }
    tmp = ret;
    tmpkey = __pycomps_lang_decode(((COMPS_RTreePair*)it->data)->key);
    if (!tmpkey) {
        PyErr_SetString(PyExc_TypeError, "key convert error");
        return NULL;
    }
    tmpval = __pycomps_lang_decode((char*)((COMPS_RTreePair*)it->data)->data);
    if (!tmpval) {
        PyErr_SetString(PyExc_TypeError, "val convert error");
        return NULL;
    }
    tmp2 = PyUnicode_FromFormat("%U = '%U'", tmpkey, tmpval);
    ret = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp);
    Py_XDECREF(tmp2);
    Py_XDECREF(tmpkey);
    Py_XDECREF(tmpval);
    
    tmp = ret;
    tmp2 = PyUnicode_FromString("}");
    ret = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp);
    Py_XDECREF(tmp2);

    comps_hslist_destroy(&pairlist);
    return ret;
}

int PyCOMPSDict_print(PyObject *self, FILE *f, int flags) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;

    (void)flags;
    fprintf(f, "{");
    pairlist = comps_objdict_pairs(((PyCOMPS_Dict*)self)->dict);
    for (it = pairlist->first; it != pairlist->last; it = it->next) {
        fprintf(f, "%s = '%s', ", ((COMPS_RTreePair*)it->data)->key,
                                 (char*)((COMPS_ObjRTreePair*)it->data)->data);
    }
    if (it) {
        fprintf(f, "%s = '%s'", ((COMPS_ObjRTreePair*)it->data)->key,
                                 (char*)((COMPS_ObjRTreePair*)it->data)->data);
    }
    fprintf(f, "}");
    comps_hslist_destroy(&pairlist);
    return 0;
}


PyObject* PyCOMPSDict_cmp(PyObject *self, PyObject *other, int op) {
    COMPS_HSList *pairlist, *pairlist2;
    COMPS_HSListItem *hsit, *hsit2;

    if (other == NULL || !PyType_IsSubtype(Py_TYPE(other), Py_TYPE(self))) {
        PyErr_Format(PyExc_TypeError, "Not Dict subclass, %s", Py_TYPE(other)->tp_name);
        return NULL;
    }
    if (op != Py_EQ && op != Py_NE) {
        PyErr_Format(PyExc_TypeError, "Unsuported operator");
        return Py_NotImplemented;
    }
    pairlist = comps_objdict_pairs(((PyCOMPS_Dict*)self)->dict);
    pairlist2 = comps_objdict_pairs(((PyCOMPS_Dict*)other)->dict);
    hsit = pairlist->first;
    hsit2 = pairlist2->first;
    for (; hsit != NULL && hsit2 != NULL; hsit = hsit->next,
                                          hsit2 = hsit2->next) {
        if (strcmp(((COMPS_ObjRTreePair*)hsit->data)->key,
                    ((COMPS_ObjRTreePair*)hsit2->data)->key) ||
            strcmp((char*)((COMPS_RTreePair*)hsit->data)->data,
                   (char*)((COMPS_RTreePair*)hsit->data)->data)) {
            comps_hslist_destroy(&pairlist);
            comps_hslist_destroy(&pairlist2);
            if (op == Py_EQ)
                Py_RETURN_FALSE;
            else
                Py_RETURN_TRUE;
        }
    }
    if (hsit == NULL && hsit2 == NULL) {
        comps_hslist_destroy(&pairlist);
        comps_hslist_destroy(&pairlist2);
        if (op == Py_EQ) Py_RETURN_TRUE;
        else Py_RETURN_FALSE;
    }
    else if (hsit !=NULL || hsit2 != NULL) {
        comps_hslist_destroy(&pairlist);
        comps_hslist_destroy(&pairlist2);
        if (op == Py_EQ) Py_RETURN_FALSE;
        else Py_RETURN_TRUE;
    }
    comps_hslist_destroy(&pairlist);
    comps_hslist_destroy(&pairlist2);
    if (op == Py_EQ) Py_RETURN_TRUE;
    else Py_RETURN_FALSE;
}

Py_ssize_t PyCOMPSDict_len(PyObject *self) {
    (void)self;
    return 0;
}

PyObject* PyCOMPSDict_get(PyObject *self, PyObject *key) {
    char *ckey;
    COMPS_Object* val;

    if (__pycomps_stringable_to_char(key, &ckey)) {
        return NULL;
    }
    val = comps_objdict_get(((PyCOMPS_Dict*)self)->dict, ckey);
    free(ckey);
    if (!val)
        Py_RETURN_NONE;
    else
       return PyUnicode_FromString(comps_object_tostr(val));
}

int PyCOMPSDict_set(PyObject *self, PyObject *key, PyObject *val) {
    char *ckey, *cval;

    if (__pycomps_stringable_to_char(key, &ckey)) {
        return -1;
    }

    if (val != NULL) {
        if (__pycomps_stringable_to_char(val, &cval)) {
            return -1;
        }
        if (!ckey) {
            return -1;
        }
        if (!cval) {
            return -1;
        }
        comps_objdict_set_x(((PyCOMPS_Dict*)self)->dict, ckey,
                          (COMPS_Object*)comps_str_x(cval));
    } else {
        comps_objdict_unset(((PyCOMPS_Dict*)self)->dict, ckey);
    }
    free(ckey);

    return 0;
}

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
    ((PyCOMPS_DictIter*)res)->hslist = comps_objdict_keys(((PyCOMPS_Dict*)self)->dict);
    ((PyCOMPS_DictIter*)res)->hsit = ((PyCOMPS_DictIter*)res)->hslist->first;
    ((PyCOMPS_DictIter*)res)->out_func = &__pycomps_dict_key_out;
    return res;
}

PyObject* PyCOMPSDict_getiteritems(PyObject *self) {
    PyObject *res;
    res = PyCOMPSDictIter_new(&PyCOMPS_DictIterType, NULL, NULL);
    ((PyCOMPS_DictIter*)res)->hslist = comps_objdict_pairs(((PyCOMPS_Dict*)self)->dict);
    ((PyCOMPS_DictIter*)res)->hsit = ((PyCOMPS_DictIter*)res)->hslist->first;
    ((PyCOMPS_DictIter*)res)->out_func = &__pycomps_dict_pair_out;
    return res;
}

PyObject* PyCOMPSDict_getitervalues(PyObject *self) {
    PyObject *res;
    res = PyCOMPSDictIter_new(&PyCOMPS_DictIterType, NULL, NULL);
    ((PyCOMPS_DictIter*)res)->objlist = comps_objdict_values(((PyCOMPS_Dict*)self)->dict);
    ((PyCOMPS_DictIter*)res)->it = ((PyCOMPS_DictIter*)res)->objlist->first;
    ((PyCOMPS_DictIter*)res)->out_func = &__pycomps_dict_key_out;
    return res;
}

PyMappingMethods PyCOMPSDict_mapping = {
    NULL, //PyCOMPSDict_len,
    PyCOMPSDict_get,
    PyCOMPSDict_set
};


PyMemberDef PyCOMPSDict_members[] = {
    {NULL}};

PyMethodDef PyCOMPSDict_methods[] = {
     {"get", (PyCFunction)PyCOMPSDict_get, METH_O,
     "alias for libcomps.Dict[key]"},
     {"has_key", (PyCFunction)PyCOMPSDict_has_key, METH_O,
     "alias for key in dict"},
     {PYCOMPS_DICT_ITERITEMS, (PyCFunction)PyCOMPSDict_getiteritems, METH_NOARGS,
     "return iterator returning (key, value) tuple"},
     {"itervalues", (PyCFunction)PyCOMPSDict_getitervalues, METH_NOARGS,
     "return iterator returning (key, value) tuple"},
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_DictType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Dict",            /*tp_name*/
    sizeof(PyCOMPS_Dict),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)PyCOMPSDict_dealloc, /*tp_dealloc*/
    &PyCOMPSDict_print,         /*tp_print*/
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
    return (PyObject*) self;
}

void PyCOMPSDictIter_dealloc(PyCOMPS_DictIter *self)
{
    comps_hslist_destroy(&self->hslist);
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
    return 0;
}

PyTypeObject PyCOMPS_DictIterType = {
    PY_OBJ_HEAD_INIT
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
