#ifndef PYCOMPS_SEQ_H
#define PYCOMPS_SEQ_H
#include "pycomps_ctopy.h"

typedef void* (*PyCOMPSSeq_in_itemconvert)(PyObject*);

typedef PyObject* (*PyCOMPSSeq_out_itemconvert)(void*);

typedef char (*PyCOMPSSeq_cmp_func)(void*, void*);

typedef struct PyCOMPS_Sequence {
    PyObject_HEAD
    PyCOMPS_CtoPy_CItem *citem;
    //COMPS_List *list;

    PyTypeObject ** itemtypes;
    PyCOMPSSeq_in_itemconvert *in_convert_funcs;
    PyCOMPSSeq_out_itemconvert out_convert_func;
    PyCOMPSSeq_cmp_func item_cmp_func;
    unsigned item_types_len;
    //COMPS_List *reflist;
} PyCOMPS_Sequence;

typedef struct PyCOMPS_CtoPySeqIter{
    PyObject_HEAD
    COMPS_ListItem *it;
} PyCOMPS_SeqIter;

COMPS_List * get_list(PyObject *self);

Py_ssize_t list_len(PyObject *self);
PyObject *list_getitem(PyObject *self, Py_ssize_t index);
int list_setitem(PyObject *self, Py_ssize_t index, PyObject *value);
PyObject* list_concat(PyObject *self, PyObject *other);
PyObject *PyCOMPSSeq_get(PyObject *self, PyObject *key);
int PyCOMPSSeq_set(PyObject *self, PyObject *key, PyObject *val);

PyObject* list_repeat(PyObject *self, Py_ssize_t count);
PyObject* PyCOMPSSeq_clear(PyObject *self);

void PyCOMPSSeq_dealloc(PyCOMPS_Sequence *self);
PyObject* PyCOMPSSeq_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSSeq_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSSeq_append(PyObject * self, PyObject *item);
PyObject* PyCOMPSSeq_str(PyObject *self);
int PyCOMPSSeq_print(PyObject *self, FILE *f, int flags);
PyObject* PyCOMPSSeq_cmp(PyObject *self, PyObject *other, int op);

void py_xdecref(void* obj);

PyObject *list_getitem_X(PyObject *self, PyObject *key);

PyObject* PyCOMPSSeqIter_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSSeqIter_init(PyCOMPS_SeqIter *self, PyObject *args, PyObject *kwds);

extern PyTypeObject PyCOMPS_SeqType;
extern PyTypeObject PyCOMPS_SeqIterType;

#endif
