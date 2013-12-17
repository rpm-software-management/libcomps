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
#include "pycomps_mdict.h"

PyObject* __pycomps_mdict_val_out(COMPS_HSListItem *hsit) {
    PyObject *ret;

    ret = PyCOMPSSeq_new(&PyCOMPS_StrSeqType, NULL, NULL);
    PyCOMPSStrSeq_init((PyCOMPS_Sequence*)ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)ret)->list);
    ((PyCOMPS_Sequence*)ret)->list = (COMPS_ObjList*)comps_object_incref(
                       (COMPS_Object*)((COMPS_ObjMRTreePair*)hsit->data)->data);
    return ret;
}

PyObject* __pycomps_mdict_pair_out(COMPS_HSListItem *hsit) {
    PyObject *key, *val, *tuple;

    key = PyUnicode_FromString((char*) ((COMPS_ObjRTreePair*)hsit->data)->key);
    val = PyCOMPSSeq_new(&PyCOMPS_StrSeqType, NULL, NULL);
    PyCOMPSStrSeq_init((PyCOMPS_Sequence*)val, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)val)->list);
    ((PyCOMPS_Sequence*)val)->list = (COMPS_ObjList*)comps_object_incref(
                       (COMPS_Object*)((COMPS_ObjMRTreePair*)hsit->data)->data);
    tuple = PyTuple_Pack(2, key, val);
    Py_DECREF(key);
    Py_DECREF(val);
    return tuple;
}

void PyCOMPSMDict_dealloc(PyCOMPS_MDict *self)
{
    COMPS_OBJECT_DESTROY(self->dict);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSMDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;

    PyCOMPS_MDict *self;

    self = (PyCOMPS_MDict*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->dict = (COMPS_ObjMDict*)comps_object_create(&COMPS_ObjMDict_ObjInfo,
                                                         NULL);
    }
    return (PyObject*) self;
}


PyObject* PyCOMPSMDict_str(PyObject *self) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    PyObject *ret, *tmp, *tmp2, *tmpkey, *tmpval;
    ret = PyUnicode_FromString("{");
    pairlist = comps_objmdict_pairs(((PyCOMPS_MDict*)self)->dict);
    char *tmpstr;

    for (it = pairlist->first; it != pairlist->last; it = it->next) {
        tmp = ret;
        tmpkey = __pycomps_lang_decode(((COMPS_ObjMRTreePair*)it->data)->key);
        if (!tmpkey) {
            PyErr_SetString(PyExc_TypeError, "key convert error");
            return NULL;
        }
        tmpstr = comps_object_tostr((COMPS_Object*)
                                    ((COMPS_ObjMRTreePair*)it->data)->data);
        tmpval = __pycomps_lang_decode(tmpstr);
        free(tmpstr);
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
    tmpkey = __pycomps_lang_decode(((COMPS_ObjMRTreePair*)it->data)->key);
    if (!tmpkey) {
        //PyErr_SetString(PyExc_TypeError, "key convert error");
        return NULL;
    }
    tmpstr = comps_object_tostr((COMPS_Object*)
                                ((COMPS_ObjMRTreePair*)it->data)->data);
    //printf("Dict_str val:%s\n", tmpstr);
    tmpval = __pycomps_lang_decode(tmpstr);
    //printf("after decode\n");
    free(tmpstr);
    if (!tmpval) {
        //PyErr_SetString(PyExc_TypeError, "val convert error");
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

int PyCOMPSMDict_print(PyObject *self, FILE *f, int flags) {
    COMPS_HSList *pairlist;
    COMPS_HSListItem *it;
    char *tmpstr;

    (void)flags;
    fprintf(f, "{");


    pairlist = comps_objmdict_pairs(((PyCOMPS_MDict*)self)->dict);
    for (it = pairlist->first; it != pairlist->last; it = it->next) {
        //printf("x\n");
        tmpstr = comps_object_tostr((COMPS_Object*)
                                    ((COMPS_ObjMRTreePair*)it->data)->data);
        fprintf(f, "%s = '%s', ", ((COMPS_RTreePair*)it->data)->key, tmpstr);
        free(tmpstr);
    }
    if (it) {
        tmpstr = comps_object_tostr((COMPS_Object*)
                                    ((COMPS_ObjMRTreePair*)it->data)->data);
        fprintf(f, "%s = '%s'", ((COMPS_ObjMRTreePair*)it->data)->key, tmpstr);
        free(tmpstr);
    }
    fprintf(f, "}");
    comps_hslist_destroy(&pairlist);
    return 0;
}


PyObject* PyCOMPSMDict_cmp(PyObject *self, PyObject *other, int op) {
    COMPS_HSList *pairlist, *pairlist2;
    COMPS_HSListItem *hsit, *hsit2;

    if (other == NULL || (Py_TYPE(self) != Py_TYPE(other) &&
                          !PyType_IsSubtype(Py_TYPE(other), Py_TYPE(self)))) {
        PyErr_Format(PyExc_TypeError, "%s subclass, %s",
                                      Py_TYPE(self)->tp_name,
                                      Py_TYPE(other)->tp_name);
        return NULL;
    }
    if (op != Py_EQ && op != Py_NE) {
        PyErr_Format(PyExc_TypeError, "Unsuported operator");
        return Py_INCREF(Py_NotImplemented), Py_NotImplemented;
    }
    pairlist = comps_objmdict_pairs(((PyCOMPS_MDict*)self)->dict);
    pairlist2 = comps_objmdict_pairs(((PyCOMPS_MDict*)other)->dict);
    hsit = pairlist->first;
    hsit2 = pairlist2->first;
    for (; hsit != NULL && hsit2 != NULL; hsit = hsit->next,
                                          hsit2 = hsit2->next) {
        if (strcmp(((COMPS_ObjRTreePair*)hsit->data)->key,
                    ((COMPS_ObjRTreePair*)hsit2->data)->key) ||
            !COMPS_OBJECT_CMP(((COMPS_ObjMRTreePair*)hsit->data)->data,
                             ((COMPS_ObjMRTreePair*)hsit->data)->data)) {
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
    else {
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

Py_ssize_t PyCOMPSMDict_len(PyObject *self) {
    (void)self;
    return 0;
}

PyObject* PyCOMPSMDict_get(PyObject *self, PyObject *key) {
    char *ckey;
    COMPS_ObjList* val;
    PyObject *ret;

    if (__pycomps_stringable_to_char(key, &ckey)) {
        return NULL;
    }
    val = comps_objmdict_get(((PyCOMPS_MDict*)self)->dict, ckey);
    if (!val) {
        PyErr_Format(PyExc_KeyError, "KeyError: '%s'", ckey);
        free(ckey);
        return NULL;
    } else {
        ret = PyCOMPSSeq_new(&PyCOMPS_StrSeqType, NULL, NULL);
        PyCOMPSStrSeq_init((PyCOMPS_Sequence*)ret, NULL, NULL);
        COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)ret)->list);
        ((PyCOMPS_Sequence*)ret)->list = val;
        free(ckey);
        return ret;
    }
}

int PyCOMPSMDict_set(PyObject *self, PyObject *key, PyObject *val) {
    #define _dict_ ((PyCOMPS_MDict*)self)
    char *ckey;
    COMPS_Object *ret = NULL;
    if (val) {
        for (unsigned i = 0; i < _dict_->it_info->item_types_len; i++) {
            if (Py_TYPE(val) != _dict_->it_info->itemtypes[i])
                continue;
            if (_dict_->it_info->in_convert_funcs[i]) {
                ret = _dict_->it_info->in_convert_funcs[i](val);
                break;
            }
        }
    }
    if (__pycomps_stringable_to_char(key, &ckey)) {
        return -1;
    }

    if (!ret && val) {
        PyErr_Format(PyExc_TypeError, "Cannot set %s to %s",
                     Py_TYPE(val)->tp_name,
                     Py_TYPE(self)->tp_name);
        free(ckey);
        return -1;
    } else if (val) {
        comps_objmdict_unset(((PyCOMPS_MDict*)self)->dict, ckey);
        for (COMPS_ObjListIt * it = ((COMPS_ObjList*)ret)->first;
             it != NULL; it = it->next) {
            comps_objmdict_set(((PyCOMPS_MDict*)self)->dict, ckey,
                                 it->comps_obj);
        }
        if (!((COMPS_ObjList*)ret)->first) {
            COMPS_OBJECT_DESTROY(ret);
            comps_objmdict_set(((PyCOMPS_MDict*)self)->dict, ckey, NULL);
            ret = (COMPS_Object*)comps_objmdict_get(
                                        ((PyCOMPS_MDict*)self)->dict,
                                        ckey);
            comps_objlist_remove(((COMPS_ObjList*)ret), NULL);
            COMPS_OBJECT_DESTROY(ret);
        } else {
            COMPS_OBJECT_DESTROY(ret);
        }
    } else if (!val){
        comps_objmdict_unset(((PyCOMPS_MDict*)self)->dict, ckey);
    }
    free(ckey);

    return 0;
    #undef _dict_
}

PyObject* PyCOMPSMDict_has_key(PyObject * self, PyObject *key) {
    char *ckey;
    COMPS_ObjList *val;

    if (__pycomps_stringable_to_char(key, &ckey)) {
        return NULL;
    }
    val = comps_objmdict_get(((PyCOMPS_MDict*)self)->dict, ckey);
    free(ckey);
    if (!val)
        Py_RETURN_FALSE;
    else
        Py_RETURN_TRUE;
}

PyObject* PyCOMPSMDict_getiter(PyObject *self) {
    PyObject *res;
    res = PyCOMPSMDictIter_new(&PyCOMPS_MDictIterType, NULL, NULL);
    PyCOMPSMDictIter_init((PyCOMPS_MDictIter*)res, NULL, NULL);
    ((PyCOMPS_MDictIter*)res)->hslist = comps_objmdict_keys(((PyCOMPS_MDict*)self)->dict);
    ((PyCOMPS_MDictIter*)res)->hsit = ((PyCOMPS_MDictIter*)res)->hslist->first;
    ((PyCOMPS_MDictIter*)res)->out_func = &__pycomps_dict_key_out;
    return res;
}

PyObject* PyCOMPSMDict_getiteritems(PyObject *self) {
    PyObject *res;
    res = PyCOMPSMDictIter_new(&PyCOMPS_MDictIterType, NULL, NULL);
    PyCOMPSMDictIter_init((PyCOMPS_MDictIter*)res, NULL, NULL);
    ((PyCOMPS_MDictIter*)res)->hslist = comps_objmdict_pairs(((PyCOMPS_MDict*)self)->dict);
    ((PyCOMPS_MDictIter*)res)->hsit = ((PyCOMPS_MDictIter*)res)->hslist->first;
    ((PyCOMPS_MDictIter*)res)->out_func = &__pycomps_mdict_pair_out;
    return res;
}

PyObject* PyCOMPSMDict_getitervalues(PyObject *self) {
    PyObject *res;
    res = PyCOMPSMDictIter_new(&PyCOMPS_MDictIterType, NULL, NULL);
    PyCOMPSMDictIter_init((PyCOMPS_MDictIter*)res, NULL, NULL);
    ((PyCOMPS_MDictIter*)res)->hslist = comps_objmdict_values(((PyCOMPS_MDict*)self)->dict);
    ((PyCOMPS_MDictIter*)res)->hsit = ((PyCOMPS_MDictIter*)res)->hslist->first;
    ((PyCOMPS_MDictIter*)res)->out_func = &__pycomps_mdict_val_out;
    return res;
}

PyMappingMethods PyCOMPSMDict_mapping = {
    NULL, //PyCOMPSMDict_len,
    PyCOMPSMDict_get,
    PyCOMPSMDict_set
};


PyMemberDef PyCOMPSMDict_members[] = {
    {NULL}};

PyMethodDef PyCOMPSMDict_methods[] = {
     {"get", (PyCFunction)PyCOMPSMDict_get, METH_O,
     "alias for libcomps.Dict[key]"},
     {"has_key", (PyCFunction)PyCOMPSMDict_has_key, METH_O,
     "alias for key in dict"},
     {PYCOMPS_DICT_ITERITEMS, (PyCFunction)PyCOMPSMDict_getiteritems, METH_NOARGS,
     "return iterator returning (key, value) tuple"},
     {"itervalues", (PyCFunction)PyCOMPSMDict_getitervalues, METH_NOARGS,
     "return iterator returning (key, value) tuple"},
    {NULL}  /* Sentinel */
};

COMPS_Object* __pycomps_strseq_in(PyObject *obj) {
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Sequence*)obj)->list);
}

COMPS_Object* __pycomps_strlist_in(PyObject *obj) {
    size_t i = 0;
    size_t len = PyList_Size(obj);
    PyObject *item;

    COMPS_Object *ret = comps_object_create(&COMPS_ObjList_ObjInfo, NULL);
    for (; i < len; i++) {
        item = PyList_GetItem(obj, i);
        if (PyUnicode_Check(item)) {
            comps_objlist_append_x(((COMPS_ObjList*)ret),
                                 __pycomps_unicode_in(item));
        } else if (PyBytes_Check(item)) {
            comps_objlist_append_x(((COMPS_ObjList*)ret),
                                 __pycomps_bytes_in(item));
        } else {
            PyErr_Format(PyExc_ValueError,
                         "%zd.item is not a string or unicode\n", i);
            return NULL;
        }
    }
    return ret;
}

PyCOMPS_ItemInfo PyCOMPS_MDictInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_StrSeqType, &PyList_Type},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&__pycomps_strseq_in, &__pycomps_strlist_in},
    .item_types_len = 2,
    .out_convert_func = &__pycomps_str_out
};


int PyCOMPSMDict_init(PyCOMPS_MDict *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    self->it_info = &PyCOMPS_MDictInfo;
    return 0;
}

PyTypeObject PyCOMPS_MDictType = {
    PY_OBJ_HEAD_INIT
    "libcomps.MDict",            /*tp_name*/
    sizeof(PyCOMPS_MDict),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)PyCOMPSMDict_dealloc, /*tp_dealloc*/
    &PyCOMPSMDict_print,         /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    &PyCOMPSMDict_mapping,       /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    PyCOMPSMDict_str,            /*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Comps Dict",               /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    &PyCOMPSMDict_cmp,           /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    &PyCOMPSMDict_getiter,       /* tp_iter */
    0,                          /* tp_iternext */
    PyCOMPSMDict_methods,        /* tp_methods */
    PyCOMPSMDict_members,        /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSMDict_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSMDict_new,                 /* tp_new */};


PyMemberDef PyCOMPSMDictIter_members[] = {
    {NULL}};

PyMethodDef PyCOMPSMDictIter_methods[] = {
    {NULL}  /* Sentinel */
};

PyObject* PyCOMPSMDictIter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    PyCOMPS_MDictIter *self;
    self = (PyCOMPS_MDictIter*)type->tp_alloc(type, 0);
    self->hslist = NULL;
    self->hsit = NULL;
    return (PyObject*) self;
}

void PyCOMPSMDictIter_dealloc(PyCOMPS_MDictIter *self)
{
    comps_hslist_destroy(&self->hslist);
    COMPS_OBJECT_DESTROY(self->objlist);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSMDict_iternext(PyObject *iter_o) {
    void *ret;
    PyObject *retp;
    PyCOMPS_MDictIter *iter = ((PyCOMPS_MDictIter*)iter_o);
    ret = iter->hsit?iter->hsit->data: NULL;
    if (ret) {
        retp = iter->out_func(iter->hsit);
        iter->hsit = iter->hsit->next;
        return retp;
    }
    return NULL;
}

int PyCOMPSMDictIter_init(PyCOMPS_MDictIter *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    self->hsit = NULL;
    self->hslist = NULL;
    self->objlist = NULL;
    self->it = NULL;
    return 0;
}

PyTypeObject PyCOMPS_MDictIterType = {
    PY_OBJ_HEAD_INIT
    "libcomps.DictIter",   /*tp_name*/
    sizeof(PyCOMPS_MDictIter), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSMDictIter_dealloc, /*tp_dealloc*/
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
    PyCOMPSMDict_iternext,         /* tp_iternext */
    PyCOMPSMDictIter_methods,         /* tp_methods */
    PyCOMPSMDictIter_members,         /* tp_members */
    0,                          /* tp_getset */
    &PySeqIter_Type,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSMDictIter_init,  /* tp_init */
    0,                              /* tp_alloc */
    PyCOMPSMDictIter_new,             /* tp_new */};

PyCOMPS_ItemInfo PyCOMPS_StrSeqInfo = {
#if PY_MAJOR_VERSION >= 3
    .itemtypes = (PyTypeObject*[]){&PyUnicode_Type, &PyBytes_Type},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&__pycomps_unicode_in, &__pycomps_bytes_in},
    .item_types_len = 2,
#else
    .itemtypes = (PyTypeObject*[]){&PyUnicode_Type, &PyString_Type},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&__pycomps_unicode_in, &__pycomps_bytes_in},
    .item_types_len = 2,
#endif
    .out_convert_func = &__pycomps_str_out
};

int PyCOMPSStrSeq_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds) {
    (void) args;
    (void) kwds;
    self->it_info = &PyCOMPS_StrSeqInfo;
    return 0;
}

PyTypeObject PyCOMPS_StrSeqType = {
    PY_OBJ_HEAD_INIT
    "libcomps.StrSeq",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    0,   /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Comps Str list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    0,                     /* tp_methods */
    0,                     /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_SeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSStrSeq_init,      /* tp_init */
    0,                               /* tp_alloc */
    &PyCOMPSSeq_new,                 /* tp_new */};
