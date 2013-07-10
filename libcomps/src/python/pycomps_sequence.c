#include <Python.h>
#include "libcomps/comps.h"
#include "structmember.h"
#include "libcomps/comps_list.h"
#include "pycomps_sequence.h"
#include "pycomps_23macros.h"
#include "pycomps_utils.h"

void py_xdecref(void* obj) {
    Py_XDECREF((PyObject*)obj);
}


inline COMPS_List * get_list(PyObject *self) {
    return (COMPS_List*)((PyCOMPS_Sequence*)self)->citem->data;
}
inline COMPS_List** get_listp(PyObject *self) {
    return (COMPS_List**)&((PyCOMPS_Sequence*)self)->citem->data;
}

Py_ssize_t PyCOMPSSeq_len(PyObject *self) {
    return get_list(self)->len;
}

inline PyObject *list_getitem(PyObject *self, Py_ssize_t index) {
    COMPS_ListItem *it;
    it = comps_list_at(get_list(self), index);
    if (it == NULL) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return NULL;
    }
    return ((PyCOMPS_Sequence*)self)->out_convert_func(it->data);
}

inline int list_setitem(PyObject *self, Py_ssize_t index, PyObject *item) {
    COMPS_ListItem *it;
    PyObject * converted_item = NULL;
    unsigned i;
    if (item) {
        for (i = 0; i < ((PyCOMPS_Sequence*)self)->item_types_len; i++) {
            if (Py_TYPE(item) == ((PyCOMPS_Sequence*)self)->itemtypes[i]) {
                if (((PyCOMPS_Sequence*)self)->in_convert_funcs[i])
                    converted_item = ((PyCOMPS_Sequence*)self)->in_convert_funcs[i](item);
                else
                    converted_item = item;
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

    if ((int)index > (int)(get_list(self)->len-1)) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return -1;
    }
    if (item != NULL) {
        it = comps_list_at(get_list(self), index);
        Py_XDECREF(it->data);
        it->data = converted_item;
        Py_INCREF(converted_item);
    } else {
        comps_list_remove_pos(get_list(self), index);
    }
    return 0;
}

PyObject* list_concat(PyObject *self, PyObject *other) {
    COMPS_ListItem *it, *newit;
    PyCOMPS_Sequence *result;
    //PyErr_SetString(PyExc_TypeError, "list concat");

    if (Py_TYPE(self) != Py_TYPE(other)) {
        PyErr_SetString(PyExc_TypeError, "different object class");
        return NULL;
    }

    result = (PyCOMPS_Sequence*)self->ob_type->tp_new(self->ob_type, NULL, NULL);
    self->ob_type->tp_init((PyObject*)result, NULL, NULL);

    it = get_list(self)->first;

    for (; it != NULL; it = it->next){
        Py_INCREF(it->data);
        newit = comps_list_item_create(it->data, NULL, NULL);
        comps_list_append(get_list((PyObject*)result), newit);
    }

    it = get_list(other)->first;
    for (; it != NULL; it = it->next){
        Py_INCREF(it->data);
        newit = comps_list_item_create(it->data, NULL, NULL);
        comps_list_append(get_list((PyObject*)result), newit);
    }
    return (PyObject*)result;
}

PyObject* list_repeat(PyObject *self, Py_ssize_t count) {
    COMPS_ListItem *it, *newit, *end;
    //PyObject *data;
    int i;
    PyCOMPS_Sequence *result;

    result = (PyCOMPS_Sequence*)self->ob_type->tp_new(self->ob_type, NULL, NULL);
    self->ob_type->tp_init((PyObject*)result, NULL, NULL);

    end = get_list(self)->last;
    for (i = 0; i < count; i++) {
        for (it = get_list(self)->first;
             it != end->next;
             it = it->next) {
            Py_INCREF(it->data);
            newit = comps_list_item_create(it->data, NULL, &py_xdecref);
            comps_list_append(get_list((PyObject*)result), newit);
        }
    }
    return (PyObject*)result;
}

PyObject *PyCOMPSSeq_get(PyObject *self, PyObject *key) {
    PyCOMPS_Sequence *result;
    COMPS_ListItem *newit, *it;
    int i;
    unsigned int n, uret, clen;
    Py_ssize_t istart, istop, istep, ilen;

    if (PySlice_Check(key)) {
        n = get_list(self)->len;
        result = (PyCOMPS_Sequence*)self->ob_type->tp_new(self->ob_type,
                                                          NULL, NULL);
        self->ob_type->tp_init((PyObject*)result, NULL, NULL);
        uret = PySlice_GetIndicesEx((SLICE_CAST)key, n,
                                   &istart, &istop, &istep, &ilen);

        if (uret) {
            return NULL;
        }

        clen = 0;
        it = get_list(self)->first;
        for (i=0 ; i<istart; it=it->next, i++);
        while (clen != ilen) {
            Py_INCREF((PyObject*)it->data);
            newit = comps_list_item_create(it->data, NULL, &py_xdecref);
            comps_list_append(get_list((PyObject*)result), newit);
            clen+=1;
            for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
            if (!it) it = get_list(self)->first;
            for (; i<istep; it=it->next, i++);
        }
        return (PyObject*)result;

    } else if (PyINT_CHECK(key)) {
        return list_getitem(self, PyINT_ASLONG(key));
    } else {
        PyErr_SetString(PyExc_TypeError, "Key must be index interger or slice");
        return NULL;
    }
}

int PyCOMPSSeq_set(PyObject *self, PyObject *key, PyObject *val) {
    COMPS_ListItem *it, *it2, *newit;
    int i;
    unsigned int n, c, uret, clen;
    Py_ssize_t istart, istop, istep, ilen;

    if (PySlice_Check(key)) {
        n = get_list(self)->len;
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
            it = get_list(self)->first;
            it2 = get_list(val)->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            if (istep != 1) {
                while (clen != ilen) {
                    Py_XDECREF((PyObject*)it->data);
                    Py_INCREF((PyObject*)it2->data);
                    it->data = it2->data;
                    clen += 1;
                    it2 = it2->next;
                    for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                    if (!it) it = get_list(self)->first;
                    for (; i<istep; it=it->next, i++);
                }
            } else {
                if (istart < 0) istart += n;
                if (istop < 0) istop += n;

                for (; it2 != NULL && it != NULL;
                       it2 = it2->next, it = it->next, i++) {
                    Py_INCREF((PyObject*)it2->data);
                    Py_XDECREF((PyObject*)it->data);
                    it->data = it2->data;
                }
                if (it == NULL) {
                    for (;it2 != NULL; it2 = it2->next) {

                        newit = comps_list_item_create(it2->data, NULL, &py_xdecref);
                        comps_list_append(get_list(self), newit);
                        Py_INCREF(it2->data);
                    }
                }
                if (it != NULL) {
                    for (c = i; c < istop; c++) {
                        comps_list_remove_pos(get_list(self), i);
                    }
                }
            }
            return 0;
        } else {
            clen = 0;
            it = get_list(self)->first;
            for (i=0 ; i<istart && it != NULL; it=it->next, i++);
            while (clen != ilen) {
                if (it->data) {
                    Py_XDECREF((PyObject*)it->data);
                    it->data = NULL;
                }
                clen+=1;
                for (i=0 ; i<istep && it != NULL; it=it->next,  i++);
                if (!it) it = get_list(self)->first;
                for (; i<istep; it=it->next, i++);
            }
            it2 = NULL;
            for (it = get_list(self)->first;
                 it != NULL; it2 = it, it = it->next) {
                if (it2 && !it2->data) {
                    comps_list_remove_item(get_list(self), it2);
                    free(it2);
                }
            }
            if (it2 && !it2->data) {
                comps_list_remove_item(get_list(self), it2);
                free(it2);
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
    COMPS_ListItem *it;
    unsigned i;
    void *converted_item = NULL;
    for (i = 0; i < ((PyCOMPS_Sequence*)self)->item_types_len; i++) {
        if (Py_TYPE(item) == ((PyCOMPS_Sequence*)self)->itemtypes[i]) {
            if (((PyCOMPS_Sequence*)self)->in_convert_funcs[i])
                converted_item = ((PyCOMPS_Sequence*)self)->in_convert_funcs[i](item);
            else
                converted_item = item;
            break;
        }
    }
    if (!converted_item) {
        PyErr_Format(PyExc_TypeError, "Cannot append %s to %s",
                      Py_TYPE(item)->tp_name,
                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    it = comps_list_item_create(converted_item, NULL, &free);
    comps_list_append(get_list(self), it);
    Py_RETURN_NONE;
}


void PyCOMPSSeq_dealloc(PyCOMPS_Sequence *self)
{

    free(self->itemtypes);
    free(self->in_convert_funcs);
    ctopy_citem_decref(self->citem);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSSeq_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    (void)args;
    (void)kwds;

    PyCOMPS_Sequence *self;
    COMPS_List *list;

    self = (PyCOMPS_Sequence*) type->tp_alloc(type, 0);
    if (self != NULL) {
        list = comps_list_create();
        comps_list_init(list);
        self->citem = ctopy_citem_create(list, &comps_list_destroy_v);
    } else return NULL;
    self->itemtypes = NULL;
    self->in_convert_funcs = NULL;
    self->out_convert_func = NULL;
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
    PyObject *ret, *tmp, *tmp2;
    COMPS_ListItem *it;
    ret = PyUnicode_FromString("[");
    if (((PyCOMPS_Sequence*)self)->citem->data == NULL) {
        tmp = ret;
        tmp2 = PyUnicode_FromString(" NULL ");
        ret = PyUnicode_Concat(ret, tmp2);
        Py_XDECREF(tmp);
        Py_XDECREF(tmp2);
    }
    else if (get_list(self)->first != NULL) {
        for (it = get_list(self)->first;
             it != NULL; it = it->next) {
            if PyUnicode_Check(it->data) {
                tmp = ret;
                ret = PyUnicode_Concat(ret, (PyObject*)it->data);
                Py_XDECREF(tmp);
            } else {
                tmp2 = Py_TYPE(((PyObject*)it->data))->tp_str((PyObject*)it->data);
                tmp = ret;
                ret = PyUnicode_Concat(ret, tmp2);
                Py_XDECREF(tmp);
                Py_XDECREF(tmp2);
            }
            if (it != get_list(self)->last) {
                tmp2 = PyUnicode_FromString(", ");
                tmp = ret;
                ret = PyUnicode_Concat(ret, tmp2);
                Py_XDECREF(tmp);
                Py_XDECREF(tmp2);
            }
        }
    }
    tmp = ret;
    tmp2 = PyUnicode_FromString("]");
    ret = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp);
    Py_XDECREF(tmp2);
    return ret;
}
int PyCOMPSSeq_print(PyObject *self, FILE *f, int flags) {
    COMPS_ListItem *it;
    COMPS_List *list;
    (void) flags;

    fprintf(f, "[");
        list = get_list(self);
        if (!list) {
            fprintf(f, "]");
            return 0;
        }
        for (it = list->first; it != list->last; it = it->next) {
            fprintf(f, "%s, ", (char*)it->data);
        }
        fprintf(f, "%s", (char*)it->data);
    fprintf(f, "]");
    return 0;
}

PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op) {
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

    it = get_list(self)->first;
    it2 = get_list(other)->first;
    for (; it!= NULL && it2 != NULL; it = it->next, it2 = it2->next) {
        cmpret = ((PyCOMPS_Sequence*)self)->item_cmp_func(it->data, it2->data); 
        if (op == Py_EQ) {
            if (!cmpret) Py_RETURN_FALSE;
        } else {
            if (!cmpret) Py_RETURN_TRUE;
        }
    }
    if (it != NULL || it2 != NULL) {
        if (op == Py_EQ) Py_RETURN_FALSE;
        else Py_RETURN_TRUE;
    } else {
        return Py_INCREF(ret),ret;
    }
}

PyObject* PyCOMPSSeq_getiter(PyObject *self) {
    PyObject *res;
    res = PyCOMPSSeqIter_new(&PyCOMPS_SeqIterType, NULL, NULL);
    ((PyCOMPS_SeqIter*)res)->it = get_list(self)->first;
    ((PyCOMPS_SeqIter*)res)->seq = self;
    return res;
}

PyObject* PyCOMPSSeq_iternext(PyObject *iter_o) {
    void *ret;
    PyObject *retp;
    PyCOMPS_SeqIter *iter = ((PyCOMPS_SeqIter*)iter_o);
    ret = iter->it?iter->it->data: NULL;
    if (ret) {
        retp = iter->seq->out_convert_func(ret);
        iter->it = iter->it->next;
        return retp;
    }
    return NULL;
}

PyObject* PyCOMPSSeq_clear(PyObject *self) {
    comps_list_clear(get_list(self));
    Py_RETURN_NONE;
}


static PyMappingMethods PyCOMPSSeq_mapping = {
    PyCOMPSSeq_len,
    PyCOMPSSeq_get,
    PyCOMPSSeq_set
};

PyMemberDef PyCOMPSSeq_members[] = {
    {NULL}};

PyMethodDef PyCOMPSSeq_methods[] = {
     {"append", (PyCFunction)PyCOMPSSeq_append, METH_O,
     "Append item to new of the list"},
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
    return 0;
}

PyTypeObject PyCOMPS_SeqIterType = {
    PY_OBJ_HEAD_INIT
    "libcomps.SeqIter",   /*tp_name*/
    sizeof(PyCOMPS_SeqIter), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0, /*tp_dealloc*/
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

