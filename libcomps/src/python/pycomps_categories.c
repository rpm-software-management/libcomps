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

PyObject* PyCOMPSCat_convert(void *c) {
    (void)c;
}

PyObject* PyCOMPSCat_union(PyObject *self, PyObject *other) {
    COMPS_DocCategory *c;
    PyObject *res;

    if (other->ob_type != &PyCOMPS_CatType) {
        PyErr_SetString(PyExc_TypeError, "Not Category instance");
        return NULL;
    }
    c = comps_doccategory_union(((PyCOMPS_Category*)self)->cat,
                                ((PyCOMPS_Category*)other)->cat);
    res = PyCOMPSCat_new(&PyCOMPS_CatType, NULL, NULL);
    PyCOMPSCat_init((PyCOMPS_Category*)res, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Category*)res)->cat);
    ((PyCOMPS_Category*)res)->cat = c;
    return res;
}

void PyCOMPSCat_dealloc(PyObject *self)
{
    #define _cat_ ((PyCOMPS_Category*)self)
    Py_XDECREF(_cat_->p_group_ids);
    Py_XDECREF(_cat_->p_name_by_lang);
    Py_XDECREF(_cat_->p_desc_by_lang);
    COMPS_OBJECT_DESTROY(_cat_->cat);
    Py_TYPE(self)->tp_free((PyObject*)self);
    #undef _cat_
}

PyObject* PyCOMPSCat_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Category *self;
    (void) args;
    (void) kwds;

    self = (PyCOMPS_Category*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->cat = (COMPS_DocCategory*)comps_object_create(
                                                    &COMPS_DocCategory_ObjInfo,
                                                    NULL);
        self->p_group_ids = NULL;
        self->p_name_by_lang = NULL;
        self->p_desc_by_lang = NULL;
    }
    return (PyObject*) self;
}

int PyCOMPSCat_init(PyCOMPS_Category *self, PyObject *args, PyObject *kwds)
{
    #define _cat_ ((PyCOMPS_Category*)self)->cat
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
        comps_doccategory_set_id(_cat_, id, 1);
        comps_doccategory_set_name(_cat_, name, 1);
        comps_doccategory_set_desc(_cat_, desc, 1);
        comps_doccategory_set_display_order(_cat_, disp_ord);
        return 0;
    }else {
        return -1;
    }
    #undef _cat_
}

PyObject* PyCOMPSCat_str(PyObject *self) {
    return PyUnicode_FromString(comps_object_tostr((COMPS_Object*)
                                               ((PyCOMPS_Category*)self)->cat));
}

int PyCOMPSCat_print(PyObject *self, FILE *f, int flags) {
    #define _cat_ ((PyCOMPS_Category*)self)
    COMPS_ObjListIt *it;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;
    char *id, *name, *desc, *disp_ord;
    COMPS_Object *tmp_prop;

    (void)flags;

    tmp_prop = comps_doccategory_get_id(_cat_->cat);
    id = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_doccategory_get_name(_cat_->cat);
    name = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_doccategory_get_desc(_cat_->cat);
    desc = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_doccategory_get_display_order(_cat_->cat);
    disp_ord = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;

    fprintf(f, "<COMPS_Category: id='%s', name='%s', description='%s', "
            " display_order=%s, ", id, name, desc, disp_ord);
    fprintf(f, "name_by_lang={");
    pairlist = comps_objrtree_pairs(_cat_->cat->name_by_lang);
    for (hsit = pairlist->first; hsit != pairlist->last; hsit = hsit->next) {
        printf("'%s': '%s', ", ((COMPS_ObjRTreePair*)hsit->data)->key,
                               (char*)((COMPS_ObjRTreePair*)hsit->data)->data);
    }
    if (hsit)
        printf("'%s': '%s'}", ((COMPS_ObjRTreePair*)hsit->data)->key,
                               (char*)((COMPS_ObjRTreePair*)hsit->data)->data);
    else
        printf("}");
    comps_hslist_destroy(&pairlist);

    fprintf(f, ", desc_by_lang={");
    pairlist = comps_objrtree_pairs(_cat_->cat->desc_by_lang);
    for (hsit = pairlist->first; hsit != pairlist->last; hsit = hsit->next) {
        printf("'%s': '%s', ", ((COMPS_ObjRTreePair*)hsit->data)->key,
                               (char*)((COMPS_ObjRTreePair*)hsit->data)->data);
    }
    if (hsit)
        printf("'%s': '%s'}", ((COMPS_ObjRTreePair*)hsit->data)->key,
                               (char*)((COMPS_ObjRTreePair*)hsit->data)->data);
    else
        printf("}");
    comps_hslist_destroy(&pairlist);

    fprintf(f, ", group_ids=[");
    if (_cat_->cat->group_ids) {
        for (it = _cat_->cat->group_ids->first; it != NULL &&
             it != _cat_->cat->group_ids->last; it = it->next){
            if (((COMPS_DocGroupId*)it->comps_obj)->def) {
                name = comps_object_tostr(it->comps_obj);
                fprintf(f, "['%s' default=true], ", name);
                free(name);
            } else {
                name = comps_object_tostr(it->comps_obj);
                fprintf(f, "'%s', ", name);
                free(name);
            }
        }
        if (it) {
            if (((COMPS_DocGroupId*)it->comps_obj)->def) {
                name = comps_object_tostr(it->comps_obj);
                fprintf(f, "['%s' default=true]", name);
                free(name);
            } else {
                name = comps_object_tostr(it->comps_obj);
                fprintf(f, "'%s'", name);
                free(name);
            }
        }
    }
    fprintf(f, "]>");
    return 0;
    #undef _cat_
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

    ret = COMPS_OBJECT_CMP(((PyCOMPS_Category*)self)->cat,
                           ((PyCOMPS_Category*)self)->cat);
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

inline PyObject* PyCOMPSCat_get_name_by_lang(PyObject *self, void *closure) {
    (void)closure;
    (void)self;
//    return pycomps_lang_get_dict(pycomps_cat_get_extra(self)->name_by_lang_citem,
//                                 &((PyCOMPS_Category*)self)->name_by_lang_pobj);
}

inline int PyCOMPSCat_set_name_by_lang(PyObject *self, PyObject *value,
                                                                void *closure) {
    (void)closure;
    (void)self;
    (void)value;
/*    if (!value) {
        PyErr_Format(PyExc_TypeError, "Cannot remove attribute '%s'",(char*)closure);
        return -1;
    }
    return pycomps_lang_set_dict(&pycomps_cat_get_extra(self)->name_by_lang_citem,
                                 &((PyCOMPS_Category*)self)->name_by_lang_pobj,
                                 value,
                       (void**)&pycomps_cat_oget(self)->name_by_lang);
*/
}

inline PyObject* PyCOMPSCat_get_desc_by_lang(PyObject *self, void *closure) {
    (void)closure;
    (void)self;
/*    (void) closure;
    return pycomps_lang_get_dict(pycomps_cat_get_extra(self)->desc_by_lang_citem,
                                 &((PyCOMPS_Category*)self)->desc_by_lang_pobj);
*/
}

inline int PyCOMPSCat_set_desc_by_lang(PyObject *self, PyObject *value,
                                                              void *closure) {
    (void)closure;
    (void)self;
    (void)value;
/*    if (!value) {
        PyErr_Format(PyExc_TypeError, "Cannot remove attribute '%s'",(char*)closure);
        return -1;
    }
    return pycomps_lang_set_dict(&pycomps_cat_get_extra(self)->desc_by_lang_citem,
                                 &((PyCOMPS_Category*)self)->desc_by_lang_pobj,
                                 value,
                       (void**)&pycomps_cat_oget(self)->desc_by_lang);
*/
}

__COMPS_LIST_GETSET_CLOSURE(COMPS_DocCategory) DocCat_GroupIdsClosure = {
    .get_f = &comps_doccategory_group_ids,
    .set_f = &comps_doccategory_set_group_ids,
    .p_offset = offsetof(PyCOMPS_Category, p_group_ids),
    .c_offset = offsetof(PyCOMPS_Category, cat),
    .list_init = &PyCOMPSGIDs_init
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_IdClosure = {
    .get_f = &comps_doccategory_get_id,
    .set_f = &comps_doccategory_set_id,
    .c_offset = offsetof(PyCOMPS_Category, cat)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_NameClosure = {
    .get_f = &comps_doccategory_get_name,
    .set_f = &comps_doccategory_set_name,
    .c_offset = offsetof(PyCOMPS_Category, cat)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_DescClosure = {
    .get_f = &comps_doccategory_get_desc,
    .set_f = &comps_doccategory_set_desc,
    .c_offset = offsetof(PyCOMPS_Category, cat)
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_DispOrdClosure = {
    .get_f = &comps_doccategory_get_display_order,
    .set_f = &comps_doccategory_set_display_order,
    .c_offset = offsetof(PyCOMPS_Category, cat)
};

PyMemberDef PyCOMPSCat_members[] = {
    {NULL}};

PyMethodDef PyCOMPSCat_methods[] = {
    {NULL}  /* Sentinel */
};

PyGetSetDef PyCOMPSCat_getset[] = {
    {"id",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Category id", (void*)&DocCategory_IdClosure},
    {"name",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Category name", (void*)&DocCategory_NameClosure},
    {"desc",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Category description", (void*)&DocCategory_DescClosure},
    {"display_order",
     (getter)__PyCOMPS_get_numattr, (setter)__PyCOMPS_set_numattr,
     "Category display order attribute", (void*)&DocCategory_DispOrdClosure},
    {"group_ids",
     (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     "Category group ids",
     (void*)&DocCat_GroupIdsClosure},
    {"name_by_lang",
    (getter)PyCOMPSCat_get_name_by_lang, (setter)PyCOMPSCat_set_name_by_lang,
     "Category name locales", NULL},
    {"desc_by_lang",
    (getter)PyCOMPSCat_get_desc_by_lang, (setter)PyCOMPSCat_set_desc_by_lang,
     "Category description locales", NULL},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSCat_Nums = {
    .nb_add = PyCOMPSCat_union
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


COMPS_Object* comps_cats_in(PyObject *item) {
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Category*)item)->cat);
}

PyObject* comps_cats_out(COMPS_Object *cobj) {
    PyCOMPS_Category *ret;
    ret = (PyCOMPS_Category*)PyCOMPSCat_new(&PyCOMPS_CatType, NULL, NULL);
    PyCOMPSCat_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->cat);
    ret->cat = (COMPS_DocCategory*)cobj;
    return (PyObject*)ret;
}

PyCOMPS_SeqInfo PyCOMPS_CatsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_CatType},
    .in_convert_funcs = (PyCOMPSSeq_in_itemconvert[])
                        {&comps_cats_in},
    .out_convert_func = &comps_cats_out,
    .item_types_len = 1
};

int PyCOMPSCats_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds)
{
    (void) args;
    (void) kwds;
    self->it_info = &PyCOMPS_CatsInfo;
    return 0;
}

COMPS_ObjList* comps_cats_union(COMPS_ObjList *cats1, COMPS_ObjList *cats2) {
    COMPS_ObjListIt *it;
    COMPS_Set *set;
    COMPS_DocCategory *tmpcat;
    COMPS_HSListItem *hsit;
    COMPS_ObjList *ret;
    void *tmpdata;

    ret = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo, NULL);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_doccategory_idcmp);
    for (it = cats1 ? cats1->first : NULL; it != NULL; it = it->next) {
        comps_set_add(set, comps_object_copy(it->comps_obj));
    }
    for (it = cats2 ? cats2->first : NULL; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            tmpcat = comps_doccategory_union(
                                (COMPS_DocCategory*)it->comps_obj,
                                (COMPS_DocCategory*)comps_set_data_at(set,
                                                                it->comps_obj));
            tmpdata = comps_set_data_at(set, it->comps_obj);
            comps_set_remove(set, it->comps_obj);
            comps_object_destroy((COMPS_Object*)tmpdata);
            comps_set_add(set, tmpcat);
        } else {
            comps_set_add(set, comps_object_copy(it->comps_obj));
        }
    }
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_objlist_append_x(ret, (COMPS_Object*)hsit->data);
    }
    comps_set_destroy(&set);
    return ret;
}

PyObject* PyCOMPSCats_union(PyObject *self, PyObject *other) {
    PyCOMPS_Sequence *res;
    COMPS_ObjList *res_list;

    if (other == NULL || Py_TYPE(other) != &PyCOMPS_CatsType) {
        PyErr_Format(PyExc_TypeError, "Not %s instance", Py_TYPE(self)->tp_name);
        return NULL;
    }

    res = (PyCOMPS_Sequence*) Py_TYPE(self)->tp_new(Py_TYPE(self), NULL, NULL);
    PyCOMPSCats_init(res, NULL, NULL);
    res_list = comps_cats_union(((PyCOMPS_Sequence*)self)->list,
                                ((PyCOMPS_Sequence*)other)->list);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)res)->list);
    res->list = res_list;
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
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    sizeof(PyCOMPS_Category),   /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
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
    PyCOMPSSeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Category list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSSeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSCats_methods,        /* tp_methods */
    PyCOMPSCats_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_SeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSCats_init,      /* tp_init */
    0,                               /* tp_alloc */
    &PyCOMPSSeq_new,                 /* tp_new */};
