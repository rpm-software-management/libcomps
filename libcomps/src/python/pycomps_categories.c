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

#include "pycomps_categories.h"

inline COMPS_DocCategory* pycomps_cat_gget(PyCOMPS_Category *pycat) {
    return (COMPS_DocCategory*)pycat->citem->data;
}
inline COMPS_DocCategory* pycomps_cat_oget(PyObject *pycat) {
    return (COMPS_DocCategory*)((PyCOMPS_Category*)pycat)->citem->data;
}
inline COMPS_DocCategoryExtra* pycomps_cat_get_extra(PyObject *pycat) {
    return (COMPS_DocCategoryExtra*)
           ((COMPS_DocCategory*)((PyCOMPS_Category*)pycat)->citem->data)
           ->reserved;
}
inline COMPS_DocCategoryExtra* comps_cat_get_extra(void *doccat) {
    return (COMPS_DocCategoryExtra*)((COMPS_DocCategory*)(doccat))->reserved;
}
inline COMPS_DocCategory** pycomps_cat_getp(PyObject *pycat) {
    return (COMPS_DocCategory**)&((PyCOMPS_Category*)pycat)->citem->data;
}

inline void pycomps_cat_incref(PyObject * pycat) {
    comps_cat_incref(pycomps_cat_oget(pycat));
}

inline void comps_cat_incref(void * cat) {
    ctopy_citem_incref(comps_cat_get_extra(cat)->name_by_lang_citem);
    ctopy_citem_incref(comps_cat_get_extra(cat)->desc_by_lang_citem);
    ctopy_citem_incref(comps_cat_get_extra(cat)->group_ids_citem);
    ctopy_citem_incref(comps_cat_get_extra(cat)->citem);
}

inline void pycomps_cat_decref(void * cat) {
    ctopy_citem_decref(comps_cat_get_extra(cat)->name_by_lang_citem);
    ctopy_citem_decref(comps_cat_get_extra(cat)->desc_by_lang_citem);
    ctopy_citem_decref(comps_cat_get_extra(cat)->group_ids_citem);
    ctopy_citem_decref(comps_cat_get_extra(cat)->citem);
}

COMPS_DocCategoryExtra * comps_doccategory_extra_create() {
    COMPS_DocCategoryExtra *ret;
    ret = malloc(sizeof(*ret));
    return ret;
}

void comps_doccategory_extra_destroy(void *cdce)
{
    ctopy_citem_destroy(((COMPS_DocCategoryExtra*)cdce)->citem);
    ctopy_citem_destroy(((COMPS_DocCategoryExtra*)cdce)->group_ids_citem);
    ctopy_citem_destroy(((COMPS_DocCategoryExtra*)cdce)->name_by_lang_citem);
    ctopy_citem_destroy(((COMPS_DocCategoryExtra*)cdce)->desc_by_lang_citem);
    free(cdce);
}

PyObject* PyCOMPSCat_convert(void *c) {
    PyObject *ret;
    ret = PyCOMPSCat_new(&PyCOMPS_CatType, NULL, NULL);
    PyCOMPSCat_init((PyCOMPS_Category*)ret, NULL, NULL);

    pycomps_cat_decref(pycomps_cat_oget(ret));

    ((PyCOMPS_Category*)ret)->citem = comps_cat_get_extra(c)->citem;
    pycomps_cat_get_extra(ret)->group_ids_citem =
                                        comps_cat_get_extra(c)->group_ids_citem;
    pycomps_cat_get_extra(ret)->name_by_lang_citem =
                                     comps_cat_get_extra(c)->name_by_lang_citem;
    pycomps_cat_get_extra(ret)->desc_by_lang_citem =
                                     comps_cat_get_extra(c)->desc_by_lang_citem;

    pycomps_cat_incref(ret);
    return ret;
}

PyObject* PyCOMPSCat_union(PyObject *self, PyObject *other) {
    COMPS_DocCategory *c;
    PyObject *res;

    if (other->ob_type != &PyCOMPS_CatType) {
        PyErr_SetString(PyExc_TypeError, "Not Category instance");
        return NULL;
    }
    c = comps_doccategory_union(pycomps_cat_oget(self),
                                pycomps_cat_oget(other));
    res = PyCOMPSCat_new(&PyCOMPS_CatType, NULL, NULL);
    PyCOMPSCat_init((PyCOMPS_Category*)res, NULL, NULL);
    c->reserved = pycomps_cat_get_extra(res);
    comps_doccategory_destroy(pycomps_cat_oget(res));
    ((PyCOMPS_Category*)res)->citem->data = c;
    pycomps_cat_get_extra(res)->group_ids_citem->data = c->group_ids;
    pycomps_cat_get_extra(res)->name_by_lang_citem->data = c->name_by_lang;
    pycomps_cat_get_extra(res)->desc_by_lang_citem->data = c->desc_by_lang;
    return res;
}

void PyCOMPSCat_dealloc(PyObject *self)
{
    pycomps_cat_decref(pycomps_cat_oget(self));
    if (((PyCOMPS_Category*)self)->group_ids_pobj)
        Py_XDECREF(((PyCOMPS_Category*)self)->group_ids_pobj);
    if (((PyCOMPS_Category*)self)->name_by_lang_pobj)
        Py_XDECREF(((PyCOMPS_Category*)self)->name_by_lang_pobj);
    if (((PyCOMPS_Category*)self)->desc_by_lang_pobj)
        Py_XDECREF(((PyCOMPS_Category*)self)->desc_by_lang_pobj);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

void pycomps_cat_destroy(void * cat) {
    comps_dict_destroy(((COMPS_DocCategory*)cat)->properties);
    free(((COMPS_DocCategory*)cat)->reserved);
    free(cat);
}


PyObject* PyCOMPSCat_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Category *self;
    COMPS_DocCategory *cat;
    (void) args;
    (void) kwds;

    self = (PyCOMPS_Category*) type->tp_alloc(type, 0);
    if (self != NULL) {
        cat = comps_doccategory_create();
        cat->reserved = (void*)comps_doccategory_extra_create();
        ((COMPS_DocCategoryExtra*)(cat->reserved))->citem =\
                                  ctopy_citem_create(cat, &pycomps_cat_destroy);
        self->citem = ((COMPS_DocCategoryExtra*)cat->reserved)->citem;

        ((COMPS_DocCategoryExtra*)(cat->reserved))->group_ids_citem =
                                    ctopy_citem_create(cat->group_ids,
                                                       &comps_list_destroy_v);
        self->group_ids_pobj = NULL;

        ((COMPS_DocCategoryExtra*)(cat->reserved))->name_by_lang_citem =
                                    ctopy_citem_create(cat->name_by_lang,
                                                       &comps_dict_destroy_v);
        self->name_by_lang_pobj = NULL;

        ((COMPS_DocCategoryExtra*)(cat->reserved))->desc_by_lang_citem =
                                    ctopy_citem_create(cat->desc_by_lang,
                                                       &comps_dict_destroy_v);
        self->desc_by_lang_pobj = NULL;
    }
    return (PyObject*) self;
}

int PyCOMPSCat_init(PyCOMPS_Category *self, PyObject *args, PyObject *kwds)
{
    char *name, *id, *desc;
    unsigned disp_ord;
    static char *kwlist[] = {"id", "name", "desc", "display_order", NULL};
    id = NULL;
    name = NULL;
    desc = NULL;
    disp_ord = 0;
    if (!args && !kwds){
        return 0;
    }
    else if (PyArg_ParseTupleAndKeywords(args, kwds, "|sssi", kwlist,
                                    &id, &name, &desc, &disp_ord)) {
        comps_doccategory_set_id(pycomps_cat_gget(self), id, 1);
        comps_doccategory_set_name(pycomps_cat_gget(self), name, 1);
        comps_doccategory_set_desc(pycomps_cat_gget(self), desc, 1);
        comps_doccategory_set_displayorder(pycomps_cat_gget(self), disp_ord);
        return 0;
    }else {
        return -1;
    }
}


PyObject* comps_cat_str(void * cat) {
    PyObject *ret, *tmp, *tmp2, *emptytmp;
    const char *id, *name, *desc;
    char *empty;
    COMPS_ListItem *it;
    COMPS_Prop *tmp_prop;
    int disp_ord;

    emptytmp = Py_TYPE(Py_None)->tp_str(Py_None);
    if (PyUnicode_Check(emptytmp)) {
        __pycomps_PyUnicode_AsString(emptytmp, &empty);
    } else {
        empty = PyBytes_AsString(emptytmp);
    }

    tmp_prop = __comps_doccat_get_prop(cat, "id");
    id = (tmp_prop)?tmp_prop->prop.str:empty;
    tmp_prop = __comps_doccat_get_prop(cat, "name");
    name = (tmp_prop)?tmp_prop->prop.str:empty;
    tmp_prop = __comps_doccat_get_prop(cat, "desc");
    desc = (tmp_prop)?tmp_prop->prop.str:empty;
    tmp_prop = __comps_doccat_get_prop(cat, "display_order");
    disp_ord = (tmp_prop)?tmp_prop->prop.num:0;

    ret = PyUnicode_FromFormat("<COMPS_Category: id='%s', name='%s', "
                          "description='%s', display_order=%d, name_by_lang=",
                              id, name, desc, disp_ord);
    if (PyUnicode_Check(emptytmp)) {
        free(empty);
    }
    Py_XDECREF(emptytmp);
    tmp2 = comps_lang_str(((COMPS_DocGroup*)cat)->name_by_lang);
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp2);
    Py_XDECREF(ret);
    ret = tmp;

    tmp2 =PyUnicode_FromString(", desc_by_lang=");
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(ret);
    Py_XDECREF(tmp2);
    ret = tmp;

    tmp2 = comps_lang_str(((COMPS_DocGroup*)cat)->desc_by_lang);
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp2);
    Py_XDECREF(ret);
    ret = tmp;

    tmp2 = PyUnicode_FromString(", group_ids=[");
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(ret);
    Py_XDECREF(tmp2);
    ret = tmp;
    it = (((COMPS_DocCategory*)cat)->group_ids)?
         ((COMPS_DocCategory*)cat)->group_ids->first:NULL;
    for (; it != ((COMPS_DocCategory*)cat)->group_ids->last; it = it->next){
        tmp2 = PyUnicode_FromFormat("'%s', ", (char*)it->data);
        tmp = PyUnicode_Concat(ret, tmp2);
        Py_XDECREF(tmp2);
        Py_XDECREF(ret);
        ret = tmp;
    }
    if (it) {
        tmp2 = PyUnicode_FromFormat("'%s']", (char*)it->data);
        tmp = PyUnicode_Concat(ret, tmp2);
        Py_XDECREF(tmp2);
        Py_XDECREF(ret);
        ret = tmp;
    } else {
        tmp2 = PyUnicode_FromString("]");
        tmp = PyUnicode_Concat(ret, tmp2);
        Py_XDECREF(ret);
        Py_XDECREF(tmp2);
        ret = tmp;
    }

    tmp2 = PyUnicode_FromString(">");
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(ret);
    Py_XDECREF(tmp2);
    ret = tmp;
    return ret;
}

PyObject* PyCOMPSCat_str(PyObject *self) {
    return comps_cat_str(pycomps_cat_oget(self));
}

void comps_cat_print(FILE *f, void *c) {
    COMPS_ListItem *it;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;
    char *id, *name, *desc;
    int disp_ord;
    COMPS_Prop *tmp_prop;

    tmp_prop = __comps_doccat_get_prop(c, "id");
    id = (tmp_prop)?tmp_prop->prop.str:NULL;
    tmp_prop = __comps_doccat_get_prop(c, "name");
    name = (tmp_prop)?tmp_prop->prop.str:NULL;
    tmp_prop = __comps_doccat_get_prop(c, "desc");
    desc = (tmp_prop)?tmp_prop->prop.str:NULL;
    tmp_prop = __comps_doccat_get_prop(c, "display_order");
    disp_ord = (tmp_prop)?tmp_prop->prop.num:0;

    fprintf(f, "<COMPS_Category: id='%s', name='%s', description='%s', "
            " display_order=%d, ", id, name, desc, disp_ord);
    fprintf(f, "name_by_lang={");
    pairlist = comps_rtree_pairs(((COMPS_DocCategory*)c)->name_by_lang);
    for (hsit = pairlist->first; hsit != pairlist->last; hsit = hsit->next) {
        printf("'%s': '%s', ", ((COMPS_RTreePair*)hsit->data)->key,
                               (char*)((COMPS_RTreePair*)hsit->data)->data);
    }
    if (hsit)
        printf("'%s': '%s'}", ((COMPS_RTreePair*)hsit->data)->key,
                               (char*)((COMPS_RTreePair*)hsit->data)->data);
    else
        printf("}");
    comps_hslist_destroy(&pairlist);

    fprintf(f, ", desc_by_lang={");
    pairlist = comps_rtree_pairs(((COMPS_DocCategory*)c)->desc_by_lang);
    for (hsit = pairlist->first; hsit != pairlist->last; hsit = hsit->next) {
        printf("'%s': '%s', ", ((COMPS_RTreePair*)hsit->data)->key,
                               (char*)((COMPS_RTreePair*)hsit->data)->data);
    }
    if (hsit)
        printf("'%s': '%s'}", ((COMPS_RTreePair*)hsit->data)->key,
                               (char*)((COMPS_RTreePair*)hsit->data)->data);
    else
        printf("}");
    comps_hslist_destroy(&pairlist);

    fprintf(f, ", group_ids=[");
    if (((COMPS_DocCategory*)c)->group_ids) {
        for (it = (((COMPS_DocCategory*)c)->group_ids)->first; it != NULL &&
             it != ((COMPS_DocCategory*)c)->group_ids->last; it = it->next){
            fprintf(f, "'%s', ", (char*)it->data);
        }
        if (it)
            fprintf(f, "'%s'", (char*)it->data);
    }
    fprintf(f, "]>");
}

int PyCOMPSCat_print(PyObject *self, FILE *f, int flags) {
    (void) flags;
    comps_cat_print(f, pycomps_cat_oget(self));
    return 0;
}

PyObject* PyCOMPSCat_cmp(PyObject *self, PyObject *other, int op) {
    char ret;

    CMP_OP_EQ_NE_CHECK(op)

    if (other == NULL || ( Py_TYPE(other) != &PyCOMPS_CatType
                           && other != Py_None)) {
        PyErr_SetString(PyExc_TypeError, "Not Category instance");
        return NULL;
    }
    CMP_NONE_CHECK(op, self, other)

    ret = comps_doccategory_cmp(pycomps_cat_oget(self), pycomps_cat_oget(other));
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

PyObject* PyCOMPSCat_get_groupids(PyCOMPS_Category *self, void *closure) {
    (void) closure;
    PyObject *ret;
    if (!self->group_ids_pobj) {
        ret = PyCOMPSCtoPySeq_new(&PyCOMPS_GIDsType, NULL, NULL);
        PyCOMPSGIDs_init((PyCOMPS_CtoPySeq*)ret, NULL, NULL);
        if (pycomps_cat_gget(self)->group_ids == NULL) {
            pycomps_cat_gget(self)->group_ids = comps_list_create();
            comps_list_init(pycomps_cat_gget(self)->group_ids);
            pycomps_cat_get_extra((PyObject*)self)->group_ids_citem->data =
                                    pycomps_cat_gget(self)->group_ids;
        }
        ctopy_citem_destroy(((PyCOMPS_Sequence*)ret)->citem);
        ((PyCOMPS_CtoPySeq*)ret)->citem =
                        pycomps_cat_get_extra((PyObject*)self)->group_ids_citem;
        self->group_ids_pobj = ret;
        ctopy_citem_incref(((PyCOMPS_CtoPySeq*)ret)->citem);
        Py_INCREF(ret);
    } else {
        ret = self->group_ids_pobj;
        
        Py_INCREF(ret);
    }
    return  ret;
}

int PyCOMPSCat_set_groupids(PyCOMPS_Category *self,
                                  PyObject *value, void *closure) {
    (void) closure;
    if (!value) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete attribute group_list");
        return -1;
    }
    if (value->ob_type != &PyCOMPS_GIDsType) {
        PyErr_SetString(PyExc_TypeError, "Not GroupIds instance");
        return -1;
    }
    ctopy_citem_decref(pycomps_cat_get_extra((PyObject*)self)->group_ids_citem);
    pycomps_cat_get_extra((PyObject*)self)->group_ids_citem =
                                              ((PyCOMPS_Sequence*)value)->citem;
    ctopy_citem_incref(pycomps_cat_get_extra((PyObject*)self)->group_ids_citem);
    pycomps_cat_gget(self)->group_ids =
                    (COMPS_List*)((PyCOMPS_Sequence*)value)->citem->data;
    if (self->group_ids_pobj) {
        Py_XDECREF(self->group_ids_pobj);
    }
    self->group_ids_pobj = value;
    Py_INCREF(value);
    return 0;
}

inline PyObject* PyCOMPSCat_get_name_by_lang(PyObject *self, void *closure) {
    (void)closure;
    return pycomps_lang_get_dict(pycomps_cat_get_extra(self)->name_by_lang_citem,
                                 &((PyCOMPS_Category*)self)->name_by_lang_pobj);
}

inline int PyCOMPSCat_set_name_by_lang(PyObject *self, PyObject *value,
                                                                void *closure) {
    if (!value) {
        PyErr_Format(PyExc_TypeError, "Cannot remove attribute '%s'",(char*)closure);
        return -1;
    }
    return pycomps_lang_set_dict(&pycomps_cat_get_extra(self)->name_by_lang_citem,
                                 &((PyCOMPS_Category*)self)->name_by_lang_pobj,
                                 value,
                       (void**)&pycomps_cat_oget(self)->name_by_lang);
}

inline PyObject* PyCOMPSCat_get_desc_by_lang(PyObject *self, void *closure) {
    (void) closure;
    return pycomps_lang_get_dict(pycomps_cat_get_extra(self)->desc_by_lang_citem,
                                 &((PyCOMPS_Category*)self)->desc_by_lang_pobj);
}

inline int PyCOMPSCat_set_desc_by_lang(PyObject *self, PyObject *value,
                                                              void *closure) {
    if (!value) {
        PyErr_Format(PyExc_TypeError, "Cannot remove attribute '%s'",(char*)closure);
        return -1;
    }
    return pycomps_lang_set_dict(&pycomps_cat_get_extra(self)->desc_by_lang_citem,
                                 &((PyCOMPS_Category*)self)->desc_by_lang_pobj,
                                 value,
                       (void**)&pycomps_cat_oget(self)->desc_by_lang);
}

int pycomps_cat_strattr_setter(PyObject *self, PyObject *val, void *closure) {
    char *tmp;
    COMPS_Prop *tmp_prop;
    if (!val) {
        PyErr_Format(PyExc_TypeError, "Cannot remove attribute '%s'",(char*)closure);
        return -1;
    }

    if (__pycomps_stringable_to_char(val, &tmp) < 0) {
        return -1;
    }
    tmp_prop = comps_dict_get(pycomps_cat_oget(self)->properties, (char*)closure);
    if (!tmp_prop) {
        if (tmp) {
            tmp_prop = comps_doc_prop_str_create(tmp, 0);
            comps_dict_set(pycomps_cat_oget(self)->properties,
                           (char*)closure, tmp_prop);
        }
    } else {
        if (tmp)
            __comps_doc_char_setter((void**)&tmp_prop->prop.str, tmp, 0);
        else {
            comps_dict_unset(pycomps_cat_oget(self)->properties, (char*)closure);
        }
    }
    return 0;
}

PyObject* pycomps_cat_strattr_getter(PyObject *self, void *closure) {
    COMPS_Prop *tmp_prop;
    tmp_prop = comps_dict_get(pycomps_cat_oget(self)->properties, (char*)closure);
    if (tmp_prop != NULL)
        return PyUnicode_FromString(tmp_prop->prop.str);
    else
        Py_RETURN_NONE;
}

PyMemberDef PyCOMPSCat_members[] = {
    {NULL}};

PyMethodDef PyCOMPSCat_methods[] = {
    {NULL}  /* Sentinel */
};

PyGetSetDef PyCOMPSCat_getset[] = {
    {"id",
     (getter)pycomps_cat_strattr_getter, (setter)pycomps_cat_strattr_setter,
     "Category id", "id"},
    {"name",
     (getter)pycomps_cat_strattr_getter, (setter)pycomps_cat_strattr_setter,
     "Category name", "name"},
    {"desc",
     (getter)pycomps_cat_strattr_getter, (setter)pycomps_cat_strattr_setter,
     "Category description", "desc"},
    {"group_ids",
     (getter)PyCOMPSCat_get_groupids, (setter)PyCOMPSCat_set_groupids,
     "Category group ids list", NULL},
    {"name_by_lang",
     (getter)PyCOMPSCat_get_name_by_lang, (setter)PyCOMPSCat_set_name_by_lang,
     "Category name locales", "name_by_lang"},
    {"desc_by_lang",
     (getter)PyCOMPSCat_get_desc_by_lang, (setter)PyCOMPSCat_set_desc_by_lang,
     "Category description locales", "desc_by_lang"},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSCat_Nums = {
    .nb_add = PyCOMPSCat_union
};

PyCOMPS_CtoPySeqItemMan PyCOMPSCat_ItemMan = {
    .item_type = &PyCOMPS_CatType,
    .ctopy_convert = PyCOMPSCat_convert,
    .data_decref = pycomps_cat_decref,
    .data_incref = comps_cat_incref,
    .data_cmp = comps_doccategory_cmp_v,
    .fprint_f = comps_cat_print,
    .str_f = comps_cat_str
};

PyTypeObject PyCOMPS_CatType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Category",   /*tp_name*/
    sizeof(PyCOMPS_Category), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSCat_dealloc, /*tp_dealloc*/
    PyCOMPSCat_print,          /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSCat_cmp,            /*tp_compare*/
    0,                         /*tp_repr*/
    &PyCOMPSCat_Nums,          /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    &PyCOMPS_hash,          /*tp_hash */
    0,                         /*tp_call*/
    &PyCOMPSCat_str,            /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Comps Category",          /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSCat_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSCat_methods,        /* tp_methods */
    PyCOMPSCat_members,        /* tp_members */
    PyCOMPSCat_getset,         /* tp_getset */
    0,//&PyCOMPS_SeqItemType,      /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSCat_init,      /* tp_init */
    0,                               /* tp_alloc */
    &PyCOMPSCat_new,                 /* tp_new */};


int PyCOMPSCats_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds)
{
    (void) args;
    (void) kwds;
    self->item_man = &PyCOMPSCat_ItemMan;
    return 0;
}

COMPS_List* comps_cats_union(COMPS_List *cats1, COMPS_List *cats2) {
    COMPS_ListItem *it, *newit;
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_List *res;
    COMPS_DocCategory *tmpcat, *tmpcat2, *unicat;

    res = comps_list_create();
    comps_list_init(res);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_doccategory_idcmp);

    it = cats1?cats1->first:NULL;
    for (; it != NULL; it = it->next) {
        comps_cat_incref(it->data);
        comps_set_add(set, it->data);
    }

    it = cats2?cats2->first:NULL;
    for (; it != NULL; it = it->next) {
        tmpcat = it->data;
        if (tmpcat->group_ids == NULL) {
            tmpcat->group_ids = comps_list_create();
            comps_list_init(tmpcat->group_ids);
            comps_cat_get_extra(tmpcat)->group_ids_citem->data =
                                                         tmpcat->group_ids;
        }
        if (comps_set_in(set, tmpcat)) {
            tmpcat2 = (COMPS_DocCategory*)comps_set_data_at(set, tmpcat);
            unicat = comps_doccategory_union(tmpcat, tmpcat2);
            unicat->reserved = comps_doccategory_extra_create();
            ((COMPS_DocCategoryExtra*)(unicat->reserved))->citem =
                               ctopy_citem_create(unicat, &pycomps_cat_destroy);
            ((COMPS_DocCategoryExtra*)(unicat->reserved))->group_ids_citem =
                                        ctopy_citem_create(unicat->group_ids,
                                                           &comps_list_destroy_v);
            ((COMPS_DocCategoryExtra*)(unicat->reserved))->name_by_lang_citem =
                                        ctopy_citem_create(unicat->name_by_lang,
                                                           &comps_dict_destroy_v);
            ((COMPS_DocCategoryExtra*)(unicat->reserved))->desc_by_lang_citem =
                                        ctopy_citem_create(unicat->desc_by_lang,
                                                           &comps_dict_destroy_v);
            pycomps_cat_decref(tmpcat2);
            comps_set_remove(set, tmpcat2);
            comps_set_add(set, unicat);
        } else {
            comps_cat_incref((void*)tmpcat);
            comps_set_add(set, tmpcat);
        }
    }
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        newit = comps_list_item_create(hsit->data, NULL, &pycomps_cat_decref);
        comps_list_append(res, newit);
    }
    comps_set_destroy(&set);
    return res;

}

PyObject* PyCOMPSCats_union(PyObject *self, PyObject *other) {
    PyCOMPS_CtoPySeq *res;
    COMPS_List *res_list;

    if (other == NULL || Py_TYPE(other) != &PyCOMPS_CatsType) {
        PyErr_Format(PyExc_TypeError, "Not %s instance", Py_TYPE(self)->tp_name);
        return NULL;
    }

    res = (PyCOMPS_CtoPySeq*) Py_TYPE(self)->tp_new(Py_TYPE(self), NULL, NULL);
    PyCOMPSCats_init(res, NULL, NULL);

    res_list = comps_cats_union(ctopy_get_list(self), ctopy_get_list(other));
    comps_list_destroy((COMPS_List**)&res->citem->data);
    res->citem->data = res_list;
    return (PyObject*)res;
}

PyMemberDef PyCOMPSCats_members[] = {
    {NULL}};

PyMethodDef PyCOMPSCats_methods[] = {
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSCats_Nums = {
    .nb_add = PyCOMPSCats_union
};

PyTypeObject PyCOMPS_CatsType = {
    PY_OBJ_HEAD_INIT
    "libcomps.CategoryList",   /*tp_name*/
    sizeof(PyCOMPS_CtoPySeq), /*tp_basicsize*/
    sizeof(PyCOMPS_Category),   /*tp_itemsize*/
    (destructor)PyCOMPSCtoPySeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0, //&PyCOMPSCats_cmp,          /*tp_compare*/
    0,                         /*tp_repr*/
    &PyCOMPSCats_Nums,         /*tp_as_number*/
    0,//&PyCOMPSCat_sequence,       /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSCtoPySeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Category list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSCtoPySeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSCats_methods,        /* tp_methods */
    PyCOMPSCats_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_CtoPySeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSCats_init,      /* tp_init */
    0,                               /* tp_alloc */
    &PyCOMPSCtoPySeq_new,                 /* tp_new */};
