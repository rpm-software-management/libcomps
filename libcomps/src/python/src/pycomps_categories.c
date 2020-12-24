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
#include "libcomps/comps_set.h"

PyObject* PyCOMPSCat_union(PyObject *self, PyObject *other) {
    COMPS_DocCategory *c;
    PyObject *res;

    if (other->ob_type != &PyCOMPS_CatType) {
        PyErr_SetString(PyExc_TypeError, "Not Category instance");
        return NULL;
    }
    c = comps_doccategory_union(((PyCOMPS_Category*)self)->c_obj,
                                ((PyCOMPS_Category*)other)->c_obj);
    res = PyCOMPSCat_new(&PyCOMPS_CatType, NULL, NULL);
    PyCOMPSCat_init((PyCOMPS_Category*)res, NULL, NULL);
    //COMPS_OBJECT_DESTROY(c);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Category*)res)->c_obj);
    ((PyCOMPS_Category*)res)->c_obj = c;
    return res;
}

void PyCOMPSCat_dealloc(PyObject *self)
{
    #define _cat_ ((PyCOMPS_Category*)self)
    Py_XDECREF(_cat_->p_group_ids);
    Py_XDECREF(_cat_->p_name_by_lang);
    Py_XDECREF(_cat_->p_desc_by_lang);
    COMPS_OBJECT_DESTROY(_cat_->c_obj);
    Py_TYPE(self)->tp_free(self);
    #undef _cat_
}

PyObject* PyCOMPSCat_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Category *self;
    (void) args;
    (void) kwds;

    self = (PyCOMPS_Category*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->c_obj = COMPS_OBJECT_CREATE(COMPS_DocCategory, NULL);
        self->p_group_ids = NULL;
        self->p_name_by_lang = NULL;
        self->p_desc_by_lang = NULL;
    }
    return (PyObject*) self;
}

int PyCOMPSCat_init(PyCOMPS_Category *self, PyObject *args, PyObject *kwds)
{
    #define _cat_ ((PyCOMPS_Category*)self)->c_obj
    char *name, *id, *desc;
    int disp_ord;
    static char *kwlist[] = {"id", "name", "desc", "display_order", NULL};
    id = NULL;
    name = NULL;
    desc = NULL;
    disp_ord = -1;
    if (!args && !kwds){
        return 0;
    }
    else if (PyArg_ParseTupleAndKeywords(args, kwds, "|sssi", kwlist,
                                    &id, &name, &desc, &disp_ord)) {
        comps_doccategory_set_id(_cat_, id, 1);
        comps_doccategory_set_name(_cat_, name, 1);
        comps_doccategory_set_desc(_cat_, desc, 1);
        if (disp_ord != -1)
            comps_doccategory_set_display_order(_cat_, disp_ord, false);
        return 0;
    }else {
        return -1;
    }
    #undef _cat_
}

PyObject* PyCOMPSCat_str(PyObject *self) {
    char *x;
    COMPS_Object *id;
    PyObject *ret;
    id = comps_doccategory_get_id(((PyCOMPS_Category*)self)->c_obj);
    x = comps_object_tostr(id);
    ret = PyUnicode_FromFormat("%s", x);
    free(x);
    comps_object_destroy(id);
    return ret;
}

PyObject* PyCOMPSCat_repr(PyObject *self) {
    char *x;
    COMPS_Object *id;
    PyObject *ret;
    id = comps_doccategory_get_id(((PyCOMPS_Category*)self)->c_obj);
    x = comps_object_tostr(id);
    ret = PyUnicode_FromFormat("<libcomps.Category object '%s' at %p>", x, self);
    free(x);
    comps_object_destroy(id);
    return ret;
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

    ret = COMPS_OBJECT_CMP(((PyCOMPS_Category*)self)->c_obj,
                           ((PyCOMPS_Category*)other)->c_obj);
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

int pycomps_category_validate(COMPS_Object *obj) {
    COMPS_ValGenResult *result = comps_validate_execute(obj,
                                        COMPS_DocCategory_ValidateRules);
    int ret = __pycomps_validate_process(result);
    COMPS_OBJECT_DESTROY(result);
    return ret;
}

PyObject* PyCOMPSCat_validate(PyCOMPS_Category *cat) {
    if (pycomps_category_validate((COMPS_Object*)cat->c_obj))
        return NULL;
    else
        Py_RETURN_NONE;
}


__COMPS_LIST_GETSET_CLOSURE(COMPS_DocCategory) DocCat_GroupIdsClosure = {
    .get_f = &comps_doccategory_group_ids,
    .set_f = &comps_doccategory_set_group_ids,
    .p_offset = offsetof(PyCOMPS_Category, p_group_ids),
    .type = &PyCOMPS_GIDsType
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_IdClosure = {
    .get_f = &comps_doccategory_get_id,
    .set_f = &comps_doccategory_set_id,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_NameClosure = {
    .get_f = &comps_doccategory_get_name,
    .set_f = &comps_doccategory_set_name,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_DescClosure = {
    .get_f = &comps_doccategory_get_desc,
    .set_f = &comps_doccategory_set_desc,
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_DispOrdClosure = {
    .get_f = &comps_doccategory_get_display_order,
    .set_f = &comps_doccategory_set_display_order,
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_NameByLangClosure = {
    .p_offset = offsetof(PyCOMPS_Category, p_name_by_lang),
    .dict_offset = offsetof(COMPS_DocCategory, name_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocCategory) DocCategory_DescByLangClosure = {
    .p_offset = offsetof(PyCOMPS_Category, p_desc_by_lang),
    .dict_offset = offsetof(COMPS_DocCategory, desc_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

PyMemberDef PyCOMPSCat_members[] = {
    {NULL}};

PyMethodDef PyCOMPSCat_methods[] = {
    {"validate", (PyCFunction)PyCOMPSCat_validate, METH_NOARGS,
    "validate inner category structure"},
    {NULL}  /* Sentinel */
};

PyGetSetDef PyCOMPSCat_getset[] = {
    {"id",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "category string identificator", (void*)&DocCategory_IdClosure},
    {"name",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "name of category", (void*)&DocCategory_NameClosure},
    {"desc",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "description of category", (void*)&DocCategory_DescClosure},
    {"display_order",
     (getter)__PyCOMPS_get_numattr, (setter)__PyCOMPS_set_numattr,
     "category display order attribute", (void*)&DocCategory_DispOrdClosure},
    {"group_ids",
     (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     ":py:class:`libcomps.IdList` of :py:class:`libcomps.GroupId`",
     (void*)&DocCat_GroupIdsClosure},
    {"name_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "dictionary of locale category names", &DocCategory_NameByLangClosure},
    {"desc_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "dictionary of locale category description", &DocCategory_DescByLangClosure},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSCat_Nums = {
    .nb_add = PyCOMPSCat_union
};

PyTypeObject PyCOMPS_CatType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.Category",   /*tp_name*/
    sizeof(PyCOMPS_Category), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSCat_dealloc, /*tp_dealloc*/
    0,                          /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSCat_cmp,            /*tp_compare*/
    &PyCOMPSCat_repr,          /*tp_repr*/
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
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Category*)item)->c_obj);
}

PyObject* comps_cats_out(COMPS_Object *cobj) {
    PyCOMPS_Category *ret;
    ret = (PyCOMPS_Category*)PyCOMPSCat_new(&PyCOMPS_CatType, NULL, NULL);
    PyCOMPSCat_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->c_obj);
    ret->c_obj = (COMPS_DocCategory*)cobj;
    return (PyObject*)ret;
}

PyCOMPS_ItemInfo PyCOMPS_CatsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_CatType},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&comps_cats_in},
    .out_convert_func = &comps_cats_out,
    .item_types_len = 1,
    .props_offset = offsetof(COMPS_DocCategory, properties),
    .pre_checker = &pycomps_category_validate
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
    COMPS_ObjList *ret;
    void *data;
    int index;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, &comps_object_destroy_v,
                                    &__comps_doccategory_idcmp);
    for (it = cats1 ? cats1->first : NULL; it != NULL; it = it->next) {
        tmpcat = (COMPS_DocCategory*) comps_object_copy(it->comps_obj);
        comps_set_add(set, tmpcat);
        comps_objlist_append(ret, (COMPS_Object*)tmpcat);
    }
    for (it = cats2 ? cats2->first : NULL; it != NULL; it = it->next) {
        if ((data = comps_set_data_at(set, it->comps_obj)) != NULL) {
            index = comps_objlist_index(ret, data);
            comps_objlist_remove_at(ret, index);
            tmpcat = comps_doccategory_union((COMPS_DocCategory*)data,
                                             (COMPS_DocCategory*)it->comps_obj);
            comps_objlist_insert_at_x(ret, index, (COMPS_Object*)tmpcat);

        } else {
            comps_objlist_append(ret, it->comps_obj);
        }
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
     {"append", (PyCFunction)PyCOMPSSeq_append_unique, METH_O,
     "Append item to new of the list"},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSCats_Nums = {
    .nb_add = PyCOMPSCats_union
};

PyTypeObject PyCOMPS_CatsType = {
    PyVarObject_HEAD_INIT(NULL, 0)
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
    &PyCOMPSSeq_mapping_extra, /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSSeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
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
