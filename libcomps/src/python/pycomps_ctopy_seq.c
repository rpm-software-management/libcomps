#include <Python.h>
#include "structmember.h"

#include "libcomps/comps.h"
#include "libcomps/comps_hslist.h"
#include "libcomps/comps_list.h"
#include "pycomps_sequence.h"
#include "pycomps_23macros.h"
#include "pycomps_ctopy_seq.h"

inline COMPS_List * ctopy_get_list(PyObject *self) {
    return (COMPS_List*)((PyCOMPS_CtoPySeq*)self)->citem->data;
}

Py_ssize_t PyCOMPSCtoPySeq_len(PyObject *self) {
    return ctopy_get_list(self)->len;
}

inline PyObject *ctopy_list_getitem(PyObject *self, Py_ssize_t index) {
    COMPS_ListItem *it;
    PyObject *ret, *ret2;
    PyCOMPS_CtoPySeq *self_seq = (PyCOMPS_CtoPySeq*)self;
    void *key;
    if (index < 0)
        index = ctopy_get_list(self)->len + index;

    it = comps_list_at(ctopy_get_list(self), index);
    if (it == NULL) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return NULL;
    }
    key = ctopy_make_key(it->data);
    ret = (PyObject*)comps_brtree_get(self_seq->ctopy_map, key);
    if (!ret) {
        ret2 = self_seq->item_man->ctopy_convert(it->data);
        comps_brtree_set(self_seq->ctopy_map, key, ret2);
        Py_INCREF(ret2);
        ctopy_key_destroy(key);
        return ret2;
    } else {
        Py_INCREF(ret);
        ctopy_key_destroy(key);
        return ret;
    }
}

inline int ctopy_list_setitem(PyObject *self, Py_ssize_t index, PyObject *item){
    COMPS_ListItem *it;
    void *key;
    PyCOMPS_CtoPySeq *self_seq = (PyCOMPS_CtoPySeq*)self;

    if ((int)index > (int)(ctopy_get_list(self)->len-1)) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return -1;
    }
    if (item != NULL) {
        it = comps_list_at(ctopy_get_list(self), index);

        it->data_destructor(it->data);

        it->data = ((PyCOMPS_CtoPy_PItem*)item)->citem->data;
        self_seq->item_man->data_incref(
                                    ((PyCOMPS_CtoPy_PItem*)item)->citem->data);

        key = ctopy_make_key(it->data);
        Py_INCREF(item);
        comps_brtree_set(self_seq->ctopy_map, key, item);
        ctopy_key_destroy(key);
    } else {
        it = comps_list_remove_pos_r(ctopy_get_list(self), index);
        comps_list_item_destroy(it);
    }
    return 0;
}

PyObject* ctopy_list_concat(PyObject *self, PyObject *other) {
    PyCOMPS_CtoPySeq *result;
    COMPS_HSList * keys;
    void *val;
    COMPS_HSListItem *hsit;
    COMPS_ListItem *it, *newit;

    if (Py_TYPE(self) != Py_TYPE(other)) {
        PyErr_SetString(PyExc_TypeError, "different object class");
        return NULL;
    }

    result = (PyCOMPS_CtoPySeq*)self->ob_type->tp_new(self->ob_type, NULL, NULL);
    self->ob_type->tp_init((PyObject*)result, NULL, NULL);

    ((PyCOMPS_CtoPySeq*)result)->ctopy_map =
            comps_brtree_clone(((PyCOMPS_CtoPySeq*)self)->ctopy_map);

    it = ctopy_get_list(self)->first;
    for (; it != NULL; it = it->next){
        ctopy_citem_incref(it->data);
        newit = comps_list_item_create(it->data, NULL, &ctopy_citem_destroy_v);
        comps_list_append(ctopy_get_list((PyObject*)result), newit);
    }
    keys = comps_brtree_keys(((PyCOMPS_CtoPySeq*)other)->ctopy_map);

    for (hsit = keys->first; hsit != NULL; hsit = hsit->next) {
        val = comps_brtree_get(((PyCOMPS_CtoPySeq*)other)->ctopy_map, hsit->data);
        comps_brtree_set(result->ctopy_map, hsit->data, val);
    }
    comps_hslist_destroy(&keys);

    it = ctopy_get_list(other)->first;
    for (; it != NULL; it = it->next){
        ctopy_citem_incref(it->data);
        newit = comps_list_item_create(it->data, NULL, &ctopy_citem_destroy_v);
        comps_list_append(ctopy_get_list((PyObject*)result), newit);
    }
    return (PyObject*)result;
}

PyObject* ctopy_list_repeat(PyObject *self, Py_ssize_t count) {
    COMPS_ListItem *it, *newit, *end;
    int i;
    PyCOMPS_CtoPySeq *result;

    result = (PyCOMPS_CtoPySeq*)self->ob_type->tp_new(self->ob_type, NULL, NULL);
    self->ob_type->tp_init((PyObject*)result, NULL, NULL);

    end = ctopy_get_list((PyObject*)self)->last;
    for (i = 0; i < count; i++) {
        for (it = ctopy_get_list(self)->first;
             it != end->next;
             it = it->next) {
            ctopy_citem_incref(it->data);
            newit = comps_list_item_create(it->data, NULL, &ctopy_citem_destroy_v);
            comps_list_append(ctopy_get_list((PyObject*)result), newit);
        }
    }
    return (PyObject*)result;
}

PyObject *PyCOMPSCtoPySeq_get(PyObject *self, PyObject *key) {
    PyCOMPS_CtoPySeq *result;
    COMPS_ListItem *newit, *it;
    int i;
    unsigned int n, uret, clen;
    Py_ssize_t istart, istop, istep, ilen;
    PyCOMPS_CtoPySeq *self_seq = (PyCOMPS_CtoPySeq*)self;

    if (PySlice_Check(key)) {
        n = ctopy_get_list(self)->len;
        result = (PyCOMPS_CtoPySeq*)self->ob_type->tp_new(self->ob_type,
                                                          NULL, NULL);
        self->ob_type->tp_init((PyObject*)result, NULL, NULL);
        uret = PySlice_GetIndicesEx((SLICE_CAST)key, n,
                                   &istart, &istop, &istep, &ilen);

        if (uret) {
            return NULL;
        }

        clen = 0;
        it = ctopy_get_list(self)->first;
        for (i=0 ; i<istart; it=it->next, i++);
        while (clen != ilen) {
            newit = comps_list_item_create(it->data,
                                           NULL,
                                           self_seq->item_man->data_decref);
            self_seq->item_man->data_incref(it->data);

            comps_list_append(ctopy_get_list((PyObject*)result), newit);
            clen+=1;
            for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
            if (!it) it = ctopy_get_list(self)->first;
            for (; i<istep; it=it->next, i++);
        }
        return (PyObject*)result;

    } else if (PyINT_CHECK(key)) {
        return ctopy_list_getitem(self, PyINT_ASLONG(key));
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice");
        return NULL;
    }
}

int PyCOMPSCtoPySeq_set(PyObject *self, PyObject *key, PyObject *val) {
    COMPS_ListItem *it, *it2, *newit;
    int i;
    unsigned int n, c, uret, clen;
    Py_ssize_t istart, istop, istep, ilen;
    PyCOMPS_CtoPySeq *self_seq = (PyCOMPS_CtoPySeq*)self;

    if (PySlice_Check(key)) {
        n = ctopy_get_list(self)->len;
        uret = PySlice_GetIndicesEx((SLICE_CAST)key, n,
                                   &istart, &istop, &istep, &ilen);
        if (ilen==0) {
            uret = PySlice_GetIndicesEx((SLICE_CAST)key, n+istart,
                                       &istart, &istop, &istep, &ilen);
        }
        if (uret)
            return -1;
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
            it = ctopy_get_list(self)->first;
            it2 = ctopy_get_list(val)->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            if (istep != 1) {
                while (clen != ilen) {
                    self_seq->item_man->data_decref(it->data);
                    self_seq->item_man->data_incref(it2->data);
                    it->data = it2->data;
                    clen+=1;
                    it2 = it2->next;
                    for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                    if (!it) it = ctopy_get_list(self)->first;
                    for (; i<istep; it=it->next, i++);
                }
            } else {
                if (istart < 0) istart += n;
                if (istop < 0) istop += n;

                for (; it2 != NULL && it != NULL;
                       it2 = it2->next, it = it->next, i++) {
                    self_seq->item_man->data_decref(it->data);
                    self_seq->item_man->data_incref(it2->data);
                    it->data = it2->data;
                }
                if (it == NULL) {
                    for (;it2 != NULL; it2 = it2->next) {
                        self_seq->item_man->data_incref(it2->data);
                        newit = comps_list_item_create(it2->data,
                                                   NULL,
                                                   self_seq
                                                   ->item_man->data_decref);
                        comps_list_append(ctopy_get_list(self), newit);
                    }
                }
                if (it != NULL) {
                    for (c = i; c < istop; c++) {
                        it = comps_list_remove_pos_r(ctopy_get_list(self), i);
                        self_seq->item_man->data_decref(it->data);
                        free(it);
                    }
                }
            }
            return 0;
        } else {
            clen = 0;
            it = ctopy_get_list(self)->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            while (clen != ilen) {
                if (it->data) {
                    self_seq->item_man->data_decref(it->data);
                    it->data = NULL;
                }
                clen+=1;
                for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                if (!it) it = ctopy_get_list(self)->first;
                for (; i<istep; it=it->next, i++);
            }
            it2 = NULL;
            for (it = ctopy_get_list(self)->first;
                 it != NULL; it2 = it, it = it->next) {
                if (it2 && !it2->data) {
                    comps_list_remove_item(ctopy_get_list(self),
                                           it2);
                    free(it2);
                }
            }
            if (it2 && !it2->data) {
                comps_list_remove_item(ctopy_get_list(self),
                                       it2);
                free(it2);
            }
            return 0;
        }

    } else if (PyINT_CHECK(key)) {
        return ctopy_list_setitem(self, PyINT_ASLONG(key), val);
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice");
        return -1;
    }
}

PyObject* PyCOMPSCtoPySeq_append(PyObject * self, PyObject *item) {
    COMPS_ListItem *it;
    void *key;
    PyCOMPS_CtoPySeq *self_seq = (PyCOMPS_CtoPySeq*)self;

    if (Py_TYPE(item) != self_seq->item_man->item_type) {
        PyErr_Format(PyExc_TypeError, "Cannot append %s to %s %s|",
                      Py_TYPE(item)->tp_name,
                      Py_TYPE(self)->tp_name,
                      self_seq->item_man->item_type->tp_name);
        return NULL;
    }

    self_seq->item_man->data_incref(
                                    ((PyCOMPS_CtoPy_PItem*)item)->citem->data);

    it = comps_list_item_create(((PyCOMPS_CtoPy_PItem*)item)->citem->data,
                                 NULL,
                                self_seq->item_man
                                ->data_decref);
    key = ctopy_make_key(it->data);
    comps_brtree_set(self_seq->ctopy_map, key, item);
    Py_INCREF(item);
    ctopy_key_destroy(key);
    if (!comps_list_append(ctopy_get_list(self), it)) {
        PyErr_SetString(PyExc_TypeError, "Cannot append\n");
        return NULL;
    }
    Py_RETURN_NONE;
}


void PyCOMPSCtoPySeq_dealloc(PyCOMPS_CtoPySeq *self)
{
    comps_brtree_destroy(self->ctopy_map);
    ctopy_citem_destroy(self->citem);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSCtoPySeq_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_CtoPySeq *self;
    COMPS_List *list;

    (void)args;
    (void)kwds;

    self = (PyCOMPS_CtoPySeq*) type->tp_alloc(type, 0);
    if (self != NULL) {
        list = comps_list_create();
        comps_list_init(list);
        self->citem = ctopy_citem_create(list, &comps_list_destroy_v);
    } else return NULL;
    self->ctopy_map = comps_brtree_create(NULL, NULL, &py_xdecref, &ctopy_key_clone,
                                          &ctopy_key_destroy, &ctopy_key_cmp,
                                          &ctopy_key_len, &ctopy_subkey,
                                          &ctopy_key_concat);
    return (PyObject*) self;
}

int PyCOMPSCtoPySeq_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;
    (void)self;
    return 0;
}

PyObject* PyCOMPSCtoPySeq_str(PyObject *self) {
    PyObject *ret, *tmp, *tmp2;
    COMPS_ListItem *it;
    ret = PyUnicode_FromString("[");
    if (ctopy_get_list(self) == NULL) {
        tmp = ret;
        tmp2 = PyUnicode_FromString(" NULL ");
        ret = PyUnicode_Concat(ret, tmp2);
        Py_XDECREF(tmp);
        Py_XDECREF(tmp2);
    }
    else if (ctopy_get_list(self)->first != NULL) {
        for (it = ctopy_get_list(self)->first;
             it != NULL; it = it->next) {
            tmp2 = ((PyCOMPS_CtoPySeq*)self)->item_man->str_f(it->data);
            tmp = PyUnicode_Concat(ret, tmp2);
            Py_DECREF(ret);
            Py_DECREF(tmp2);
            ret = tmp;
        }
    }
    tmp = ret;
    tmp2 = PyUnicode_FromString("]");
    ret = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp);
    Py_XDECREF(tmp2);
    return ret;
}
int PyCOMPSCtoPySeq_print(PyObject *self, FILE *f, int flags) {
    COMPS_ListItem *it;
    PyCOMPS_CtoPySeq *self_seq = (PyCOMPS_CtoPySeq*)self;

    (void)flags;
    fprintf(f, "[");
        it = ctopy_get_list(self)?ctopy_get_list(self)->first:NULL;
        for (;it != NULL && it != ctopy_get_list(self)->last; it = it->next) {
            self_seq->item_man->fprint_f(f, it->data);
            fprintf(f, ", ");
        }
    if (it) {
        self_seq->item_man->fprint_f(f, it->data);
    }
    fprintf(f, "]");
    return 0;
}

PyObject* PyCOMPSCtoPySeq_cmp(PyObject *self, PyObject *other, int op) {
    COMPS_ListItem *it, *it2;
    PyObject *ret;
    char cmpret;

    if (other == NULL || !PyType_IsSubtype(Py_TYPE(other), Py_TYPE(self))) {
        PyErr_Format(PyExc_TypeError, "Not %s subclass, %s",
                     Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
        return NULL;
    }
    if (op != Py_EQ && op != Py_NE) {
        PyErr_Format(PyExc_TypeError, "Unsuported operator");
        return Py_NotImplemented;
    }
    if (other == Py_None && self == Py_None) {
        return Py_NotImplemented;
    } else if ((other == Py_None && self != Py_None) ||
             (other != Py_None && self == Py_None)) {
        return Py_NotImplemented;
    }
    ret = (op == Py_EQ)?Py_True:Py_False;

    it = ctopy_get_list(self)->first;
    it2 = ctopy_get_list(other)->first;
    for (; it!= NULL && it2 != NULL; it = it->next, it2 = it2->next) {
        cmpret = ((PyCOMPS_CtoPySeq*)self)->item_man->data_cmp(it->data,
                                                              it2->data);
        if (op == Py_EQ) {
            if (!cmpret) Py_RETURN_FALSE;
        } else {
            if (!cmpret) Py_RETURN_TRUE;
        }
    }
    if (it != NULL || it2 != NULL) {
        if (op == Py_EQ) Py_RETURN_FALSE;
        else Py_RETURN_TRUE;
        return NULL;
    } else {
        return Py_INCREF(ret),ret;
    }
}
PyObject* PyCOMPSCtoPySeq_getiter(PyObject *self) {
    PyObject *res;
    res = PyCOMPSCtoPySeqIter_new(&PyCOMPS_CtoPySeqIterType, NULL, NULL);
    ((PyCOMPS_CtoPySeqIter*)res)->it = ctopy_get_list(self)->first;
    ((PyCOMPS_CtoPySeqIter*)res)->seq = self;
    return res;
}

PyObject* PyCOMPSCtoPySeq_clear(PyObject *self) {
    comps_list_clear(ctopy_get_list(self));
    Py_RETURN_NONE;
}


static PyMappingMethods PyCOMPSCtoPySeq_mapping = {
    PyCOMPSCtoPySeq_len,
    PyCOMPSCtoPySeq_get,
    PyCOMPSCtoPySeq_set
};

PyMemberDef PyCOMPSCtoPySeq_members[] = {
    {NULL}};

PyMethodDef PyCOMPSCtoPySeq_methods[] = {
     {"append", (PyCFunction)PyCOMPSCtoPySeq_append, METH_O,
     "Append item to new of the list"},
     {"clear", (PyCFunction)PyCOMPSCtoPySeq_clear, METH_NOARGS,
     "Clear the list"},
    {NULL}  /* Sentinel */
};


PyTypeObject PyCOMPS_CtoPySeqType = {
    PY_OBJ_HEAD_INIT
    "libcomps.CtoPySeq",   /*tp_name*/
    sizeof(PyCOMPS_CtoPySeq), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSCtoPySeq_dealloc, /*tp_dealloc*/
    PyCOMPSCtoPySeq_print,     /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &PyCOMPSCtoPySeq_mapping,  /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSCtoPySeq_str,       /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Comps CtoPySeq",           /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    &PyCOMPSCtoPySeq_cmp,       /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    PyCOMPSCtoPySeq_getiter,    /* tp_iter */
    0,                          /* tp_iternext */
    PyCOMPSCtoPySeq_methods,    /* tp_methods */
    PyCOMPSCtoPySeq_members,    /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)PyCOMPSCtoPySeq_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSCtoPySeq_new,                 /* tp_new */};
