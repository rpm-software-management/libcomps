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

#include "pycomps_gids.h"

inline COMPS_DocGroupId* pycomps_gid_get(PyObject *pygid) {
    return (COMPS_DocGroupId*)((PyCOMPS_GID*)pygid)->citem->data;
}

inline void pycomps_gid_destroy(void * gid) {
    comps_docgroupid_destroy(gid);
}

inline void pycomps_gid_incref(void * gid) {
    ctopy_citem_incref(
                (PyCOMPS_CtoPy_CItem*)((COMPS_DocGroupId*)gid)->reserved);
}

inline void pycomps_gid_decref(void * gid) {
    ctopy_citem_destroy(((COMPS_DocGroupId*)gid)->reserved);
}

PyObject* PyCOMPSGID_convert(void *gid) {
    PyObject *ret;
    ret = PyCOMPSGID_new(&PyCOMPS_GIDType, NULL, NULL);
    PyCOMPSGID_init((PyCOMPS_GID*)ret, NULL, NULL);
    ctopy_citem_decref(((PyCOMPS_GID*)ret)->citem);
    ((PyCOMPS_GID*)ret)->citem = ((COMPS_DocGroupId*)gid)->reserved;
    ctopy_citem_incref(((PyCOMPS_GID*)ret)->citem);
    return ret;
}

void PyCOMPSGID_dealloc(PyCOMPS_GID *self)
{
    ctopy_citem_decref(self->citem);
    Py_TYPE(self)->tp_free((PyObject*)self);
    self = NULL;
}

PyObject* PyCOMPSGID_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_GID *self;
    COMPS_DocGroupId *gid;

   (void) args;
   (void) kwds;

    self = (PyCOMPS_GID*) type->tp_alloc(type, 0);
    if (self != NULL) {
        gid = comps_docgroupid_create();
        gid->reserved = ctopy_citem_create(gid, &pycomps_gid_destroy);
        self->citem = gid->reserved;
    }
    return (PyObject*) self;
}

char pycomps_p2c_bool_convert(PyObject* pobj, void *cobj) {
    if (!PyBool_Check(pobj)) {
        return 0;
    } else {
        if (pobj == Py_True)
            *(char*)cobj = 1;
        else
            *(char*)cobj = 0;
        return 1;
    }
}

PyObject* pycomps_c2p_bool_convert(void *cobj) {
    if (*(char*)cobj)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

int PyCOMPSGID_init(PyCOMPS_GID *self, PyObject *args, PyObject *kwds)
{
    char *name = NULL;
    char def;

    char* keywords[] = {"default"};

    if (args!=NULL && kwds && PyArg_ParseTupleAndKeywords(args, kwds,
                                    "s|O&", keywords, &name,
                                            &pycomps_p2c_bool_convert,&def)) {
        comps_docgroupid_set_name(pycomps_gid_get((PyObject*)self), name, 1);
        comps_docgroupid_set_default(pycomps_gid_get((PyObject*)self), def);
        return 0;
    } else {
        pycomps_gid_get((PyObject*)self)->name = NULL;
        pycomps_gid_get((PyObject*)self)->def = 0;
        return 0;
    }
    return 1;
}

PyObject* PyCOMPSGID_strget_(PyCOMPS_GID *self, void *closure) {
    char *tmp = GET_FROM(pycomps_gid_get((PyObject*)self),
                                         (size_t)closure);
    return PyUnicode_FromString(tmp);
}
int PyCOMPSGID_strset_(PyCOMPS_GID *self, PyObject *value, void *closure) {
    char *tmp;
    (void) closure;
    if (__pycomps_arg_to_char(value, &tmp)) {
        return -1;
    }
    __comps_doc_char_setter((void**)&GET_FROM(pycomps_gid_get((PyObject*)self),
                                    (size_t)closure), tmp, 0);
    return 0;
}

PyObject* PyCOMPSGID_get_default(PyCOMPS_GID *self, void *closure) {
    (void) closure;
    char def = pycomps_gid_get((PyObject*)self)->def;
        return Py_BuildValue("O&", &pycomps_c2p_bool_convert, &def);
    Py_RETURN_NONE;
}

int PyCOMPSGID_set_default(PyCOMPS_GID *self, PyObject *value, void *closure){
   (void) closure;
    if (!PyBool_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "Not a Bool");
        return -1;
    }
    if (value == Py_True)
        pycomps_gid_get((PyObject*)self)->def = 1;
    else
        pycomps_gid_get((PyObject*)self)->def = 0;
    return 0;
}

PyGetSetDef gid_getset[] = {
    {"name",
     (getter)PyCOMPSGID_strget_, (setter)PyCOMPSGID_strset_,
     "Group ID name",
     (void*)offsetof(COMPS_DocGroupId, name)},
    {"default",
     (getter)PyCOMPSGID_get_default, (setter)PyCOMPSGID_set_default,
     "Group ID default",
     NULL},
    {NULL}  /* Sentinel */
};

PyObject* PyCOMPSGID_str(PyObject *self) {
    PyObject * ret;
    char *name;
    const char *def;
    char * empty = "";
    name = (pycomps_gid_get(self)->name)?
          pycomps_gid_get(self)->name:empty;
    def = (pycomps_gid_get(self)->def)?"true":"false";
    ret = PyUnicode_FromFormat("<COMPS_GroupID: name='%s', default=%s>",
                               name, def);
    return ret;
}

inline void comps_gid_print(FILE *f, void *gid) {
    const char *def;
    def = (((COMPS_DocGroupId*)gid)->def)?"true":"false";
    fprintf(f, "<COPMS_Package name='%s' default='%s'>",
            ((COMPS_DocGroupId*)gid)->name, def);
}

int PyCOMPSGID_print(PyObject *self, FILE *f, int flags) {
    (void) flags;
    comps_gid_print(f, (void*)pycomps_gid_get(self));
    return 0;
}

PyObject* pycomps_gid_from_str(PyObject *other) {
    PyObject *pygid = PyCOMPSGID_new(&PyCOMPS_GIDType, NULL, NULL);
    PyCOMPSGID_init((PyCOMPS_GID*)pygid, NULL, NULL);
    __pycomps_stringable_to_char(other, &pycomps_gid_get(pygid)->name);
    if (!pycomps_gid_get(pygid)->name)
        return NULL;
    return pygid;
}

PyObject* PyCOMPSGID_cmp(PyObject *self, PyObject *other, int op) {
    PyObject *tmpgid;
    char ret;
    char created = 0;
    if (PyUnicode_Check(other) || PyBytes_Check(other)) {
        created = 1;
        tmpgid = pycomps_gid_from_str(other);
    } else if (other && other->ob_type == &PyCOMPS_GIDType) {
        tmpgid = other;
    } else {
        PyErr_SetString(PyExc_TypeError, "Not COMPS_GroupID instance");
        return NULL;
    }
    if (op != Py_EQ && op != Py_NE) {
        if (created)
            Py_DECREF(tmpgid);
        return Py_NotImplemented;
    }
    ret = comps_docgroupid_cmp(pycomps_gid_get(self), pycomps_gid_get(tmpgid));
    if (created) {
        Py_DECREF(tmpgid);
    }
    if (ret) {
        if (op == Py_EQ){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    } else {
        if (op == Py_EQ){
            Py_RETURN_FALSE;
        } else {
            Py_RETURN_TRUE;
        }
    }
}

char pycomps_gid_cmp(void *p1, void *p2) {
    return comps_docgroupid_cmp((COMPS_DocGroupId*)p1, (COMPS_DocGroupId*)p2);
}

PyMemberDef PyCOMPSGID_members[] = {
    {NULL}};

PyMethodDef PyCOMPSGID_methods[] = {
    {NULL}  /* Sentinel */
};

PyCOMPS_CtoPySeqItemMan PyCOMPSGID_ItemMan = {
    .item_type = &PyCOMPS_GIDType,
    .ctopy_convert = PyCOMPSGID_convert,
    .data_incref = pycomps_gid_incref,
    .data_cmp = pycomps_gid_cmp,
    .data_decref = pycomps_gid_decref,
    .fprint_f = comps_gid_print
};

PyTypeObject PyCOMPS_GIDType = {
    PY_OBJ_HEAD_INIT
    "libcomps.GroupId",   /*tp_name*/
    sizeof(PyCOMPS_GID), /*tp_basicsize*/
    0,                        /*tp_itemsize*/
    (destructor)PyCOMPSGID_dealloc, /*tp_dealloc*/
    PyCOMPSGID_print,         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSPack_cmp,           /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSGID_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Comps GroupId",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    PyCOMPSGID_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSGID_methods,        /* tp_methods */
    PyCOMPSGID_members,        /* tp_members */
    gid_getset,               /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSGID_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSGID_new,                 /* tp_new */};

int PyCOMPSGIDs_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds)
{
    (void) args;
    (void) kwds;
    self->item_man = &PyCOMPSGID_ItemMan;
    return 0;
}

PyObject* PyCOMPSGIDs_append(PyObject * self, PyObject *item) {
    COMPS_ListItem *it;
    void *key;
    PyCOMPS_CtoPySeq *self_seq = (PyCOMPS_CtoPySeq*)self;
    if (PyUnicode_Check(item) || PyBytes_Check(item)) {
        PyObject *item2 = pycomps_gid_from_str(item);
        if (!item2)
            return NULL;
        it = comps_list_item_create(((PyCOMPS_CtoPy_PItem*)item2)->citem->data,
                                     NULL,
                                     &pycomps_gid_decref);
        key = ctopy_make_key(it->data);
        comps_brtree_set(self_seq->ctopy_map, key, item2);
        Py_INCREF(item2);
        ctopy_key_destroy(key);
        if (!comps_list_append(ctopy_get_list(self), it)) {
            PyErr_SetString(PyExc_TypeError, "Cannot append\n");
            return NULL;
        }
    } else if (Py_TYPE(item) == self_seq->item_man->item_type) {
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
    } else {
        PyErr_Format(PyExc_TypeError, "Cannot append %s to %s %s|",
                      Py_TYPE(item)->tp_name,
                      Py_TYPE(self)->tp_name,
                      self_seq->item_man->item_type->tp_name);
        return NULL;

    }
    Py_RETURN_NONE;
}

PyMemberDef PyCOMPSGIDs_members[] = {
    {NULL}};

PyMethodDef PyCOMPSGIDs_methods[] = {
     {"append", (PyCFunction)PyCOMPSGIDs_append, METH_O,
     "Append item to new of the list"},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSGIDs_Nums = {0};

PyTypeObject PyCOMPS_GIDsType = {
    PY_OBJ_HEAD_INIT
    "libcomps.IdList",   /*tp_name*/
    sizeof(PyCOMPS_CtoPySeq), /*tp_basicsize*/
    sizeof(PyCOMPS_GID),   /*tp_itemsize*/
    (destructor)PyCOMPSCtoPySeq_dealloc, /*tp_dealloc*/
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
    PyCOMPSCtoPySeq_str,       /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps GIDs list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSCtoPySeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSGIDs_methods,        /* tp_methods */
    PyCOMPSGIDs_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_CtoPySeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSGIDs_init,      /* tp_init */
    0,                               /* tp_alloc */
    &PyCOMPSCtoPySeq_new,                 /* tp_new */};
