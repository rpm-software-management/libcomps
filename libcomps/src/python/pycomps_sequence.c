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
#include "pycomps_23macros.h"
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
    char *strid=NULL;
    COMPS_ObjListIt *it;
    COMPS_ObjDict *props;
    COMPS_Object *oid;
    PyObject *ret = NULL;
    COMPS_Object *tmpstr;

    if (PyUnicode_Check(id)) {
        if (!__pycomps_stringable_to_char(id, &strid)) {
            return NULL;
        }
    } else if (PyBytes_Check(id)){
        strid = PyBytes_AsString(id);
    }
    tmpstr = (COMPS_Object*)comps_str(strid);
    for (it = ((PyCOMPS_Sequence*)self)->list->first; it != NULL;
         it = it->next) {
        props = (COMPS_ObjDict*)GET_FROM(it->comps_obj,
                                         ((PyCOMPS_Sequence*)self)->it_info->props_offset);

        oid = comps_objdict_get_x(props, "id");
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
}

inline int list_setitem(PyObject *self, Py_ssize_t index, PyObject *item) {
    #define _seq_ ((PyCOMPS_Sequence*)self)
    COMPS_Object *converted_item = NULL;
    unsigned i;
    if (item) {
        for (i = 0; i < _seq_->it_info->item_types_len; i++) {
            if (Py_TYPE(item) == _seq_->it_info->itemtypes[i]) {
                if (_seq_->it_info->in_convert_funcs[i])
                    converted_item = _seq_->it_info->in_convert_funcs[i](item);
                    break;
            }
        }
    }
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
        comps_objlist_set(_seq_->list, index, converted_item);
    } else {
        comps_objlist_remove_at(_seq_->list, index);
    }
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
    printf("list concat\n");

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
    uret = PySlice_GetIndicesEx((SLICE_CAST)key, n,
                               &istart, &istop, &istep, &ilen);

    if (uret) {
        return NULL;
    }

    clen = 0;
    it = ((PyCOMPS_Sequence*)self)->list->first;
    for (i=0 ; i<istart; it=it->next, i++);
    while (clen != ilen) {
        comps_objlist_append(result->list, it->comps_obj);
        clen+=1;
        for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
        if (!it) it = ((PyCOMPS_Sequence*)self)->list->first;
        for (; i<istep; it=it->next, i++);
    }
    return (PyObject*)result;
}

PyObject* PyCOMPSSeq_get(PyObject *self, PyObject *key) {
    if (PySlice_Check(key)) {
        return list_get_slice(self, key);
    } else if (PyINT_CHECK(key)) {
        return list_getitem(self, PyINT_ASLONG(key));
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice");
        return NULL;
    }
}

PyObject* PyCOMPSSeq_id_get(PyObject *self, PyObject *key) {
    if (PySlice_Check(key)) {
        return list_get_slice(self, key);
    } else if (PyINT_CHECK(key)) {
        return list_getitem(self, PyINT_ASLONG(key));
    } else if (PyUnicode_Check(key) || PyBytes_Check(key)){
        return list_getitem_byid(self, key);
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice"
                                         "or string id");
        return NULL;
    }
}

int PyCOMPSSeq_set(PyObject *self, PyObject *key, PyObject *val) {
    COMPS_ObjListIt *it, *it2;

    unsigned int n, uret;
    Py_ssize_t istart, istop, istep, ilen, i, c, clen;

    if (PySlice_Check(key)) {
        n = ((PyCOMPS_Sequence*)self)->list->len;
        uret = PySlice_GetIndicesEx((SLICE_CAST)key, n,
                                   &istart, &istop, &istep, &ilen);
        if (ilen == 0) {
            uret = PySlice_GetIndicesEx((SLICE_CAST)key, n+istart,
                                       &istart, &istop, &istep, &ilen);
        }
        if (uret) return -1;
        if (val) {
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
            clen = 0;
            it = ((PyCOMPS_Sequence*)self)->list->first;
            it2 = ((PyCOMPS_Sequence*)val)->list->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            if (istep != 1) {
                while (clen != ilen) {
                    COMPS_OBJECT_DESTROY(it->comps_obj);
                    it->comps_obj = comps_object_incref(it2->comps_obj);
                    clen += 1;
                    it2 = it2->next;
                    for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                    if (!it) it = ((PyCOMPS_Sequence*)self)->list->first;
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
                        comps_objlist_append(((PyCOMPS_Sequence*)self)->list,
                                          it2->comps_obj);
                    }
                }
                if (it != NULL) {
                    for (c = i; c < istop; c++) {
                        comps_objlist_remove_at(((PyCOMPS_Sequence*)self)->list,
                                              i);
                    }
                }
            }
            return 0;
        } else {
            clen = 0;
            it = ((PyCOMPS_Sequence*)self)->list->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            while (clen != ilen) {
                if (it->comps_obj) {
                    COMPS_OBJECT_DESTROY(it->comps_obj);
                    it->comps_obj = NULL;
                }
                clen+=1;
                for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                if (!it) it = ((PyCOMPS_Sequence*)self)->list->first;
                for (; i<istep; it=it->next, i++);
            }
            it2 = NULL;
            for (i=0, it = ((PyCOMPS_Sequence*)self)->list->first;
                 it != NULL; it2 = it, it = it->next, i++) {
                if (it2 && !it2->comps_obj) {
                    comps_objlist_remove_at(((PyCOMPS_Sequence*)self)->list, i);
                }
            }
            if (it2 && !it2->comps_obj) {
                comps_objlist_remove_at(((PyCOMPS_Sequence*)self)->list, i);
            }
            return 0;
        }

    } else if (PyINT_CHECK(key)) {
        return list_setitem(self, PyINT_ASLONG(key), val);
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice");
        return -1;
    }
}

PyObject* PyCOMPSSeq_append(PyObject * self, PyObject *item) {
    unsigned i;
    COMPS_Object *converted_item = NULL;
    #define _seq_ ((PyCOMPS_Sequence*)self)
    for (i = 0; i < _seq_->it_info->item_types_len; i++) {
        if (Py_TYPE(item) == _seq_->it_info->itemtypes[i]) {
            if (_seq_->it_info->in_convert_funcs[i]) {
                converted_item = _seq_->it_info->in_convert_funcs[i](item);
                break;
            }
        }
    }
    if (!converted_item) {
        PyErr_Format(PyExc_TypeError, "Cannot append %s to %s",
                      Py_TYPE(item)->tp_name,
                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    comps_objlist_append_x(_seq_->list, converted_item);
    Py_RETURN_NONE;
    #undef _seq_
}

PyObject* PyCOMPSSeq_remove(PyObject * self, PyObject *item) {
    unsigned i;
    COMPS_Object *converted_item = NULL;
    #define _seq_ ((PyCOMPS_Sequence*)self)
    for (i = 0; i < _seq_->it_info->item_types_len; i++) {
        if (Py_TYPE(item) == _seq_->it_info->itemtypes[i]) {
            if (_seq_->it_info->in_convert_funcs[i]) {
                converted_item = _seq_->it_info->in_convert_funcs[i](item);
                break;
            }
        }
    }
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
        self->list = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo,
                                                         NULL);
    } else return NULL;
    self->it_info = NULL;
    return (PyObject*) self;
}

int PyCOMPSSeq_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds)
{
    (void)self;
    (void)args;
    (void)kwds;
    return 0;
}

PyObject* PyCOMPSSeq_str(PyObject *self) {
    return PyUnicode_FromString(comps_object_tostr(
                            (COMPS_Object*)((PyCOMPS_Sequence*)self)->list));
}
int PyCOMPSSeq_print(PyObject *self, FILE *f, int flags) {
    COMPS_ObjList *_list;
    (void) flags;
    char *tmpstr;
    _list = ((PyCOMPS_Sequence*)self)->list;
    tmpstr = comps_object_tostr((COMPS_Object*)_list);
    fprintf(f, "%s", tmpstr);
    free(tmpstr);
    return 0;
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
    //printf("get iter\n");
    //printf("get iter - seq it info %p\n", ((PyCOMPS_Sequence*)self)->it_info);
    res = PyCOMPSSeqIter_new(&PyCOMPS_SeqIterType, NULL, NULL);
    ((PyCOMPS_SeqIter*)res)->it = ((PyCOMPS_Sequence*)self)->list->first;
    Py_INCREF((PyCOMPS_Sequence*)self);
    ((PyCOMPS_SeqIter*)res)->seq = (PyCOMPS_Sequence*)self;
    //printf("res-seq:%p\n", ((PyCOMPS_SeqIter*)res)->seq);
    //printf("get iter - seq it info %p\n", ((PyCOMPS_SeqIter*)res)->seq->it_info);
    return res;
}

PyObject* PyCOMPSSeq_iternext(PyObject *iter_o) {
    COMPS_Object *ret;
    PyObject *retp;
    PyCOMPS_SeqIter *iter = ((PyCOMPS_SeqIter*)iter_o);
    //printf("iternext self-type:%s\n", Py_TYPE(iter_o)->tp_name);
    //printf("res-seq:%p\n", ((PyCOMPS_SeqIter*)iter)->seq);
    //printf("get iter - seq it info %p\n", iter->seq->it_info);

    ret = iter->it?iter->it->comps_obj: NULL;
    //printf("%p \n", iter);
    //printf("%p \n", iter->seq);
    if (ret) {
        //if (iter->it == iter->seq->list->first) {
        //    printf("%p \n", iter);
        //    printf("%p \n", iter->seq);
        //    printf("%p \n", iter->seq->it_info);
        //    printf("%p \n", iter->seq->it_info->out_convert_func);
        //}
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
    PyCOMPSSeq_set
};


PyMemberDef PyCOMPSSeq_members[] = {
    {NULL}};

PyMethodDef PyCOMPSSeq_methods[] = {
     {"append", (PyCFunction)PyCOMPSSeq_append, METH_O,
     "Append item to new of the list"},
     {"remove", (PyCFunction)PyCOMPSSeq_remove, METH_O,
     "Remove item from list"},
     {"clear", (PyCFunction)PyCOMPSSeq_clear, METH_NOARGS,
     "Clear the list"},
    {NULL}  /* Sentinel */
};


PyTypeObject PyCOMPS_SeqType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Sequence",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
    PyCOMPSSeq_print,          /*tp_print*/
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
    (initproc)PyCOMPSSeq_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSSeq_new,                 /* tp_new */};

PyTypeObject PyCOMPS_SeqItemType = {
    PY_OBJ_HEAD_INIT
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
    PY_OBJ_HEAD_INIT
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
    PyCOMPSSeq_getiter,         /* tp_iter */
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

