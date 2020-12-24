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

#include "pycomps_macros.h"
#include "pycomps_envs.h"
#include "libcomps/comps_set.h"

PyObject* PyCOMPSEnv_union(PyObject *self, PyObject *other) {
    COMPS_DocEnv *e;
    PyObject *res;

    if (Py_TYPE(other) != &PyCOMPS_EnvType) {
        PyErr_SetString(PyExc_TypeError, "Not Environment instance");
        return NULL;
    }
    e = comps_docenv_union(((PyCOMPS_Env*)self)->c_obj,
                           ((PyCOMPS_Env*)other)->c_obj);

    res = PyCOMPSEnv_new(&PyCOMPS_EnvType, NULL, NULL);
    PyCOMPSEnv_init((PyCOMPS_Env*)res, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Env*)res)->c_obj);
    ((PyCOMPS_Env*)res)->c_obj = e;
    return res;
}


void PyCOMPSEnv_dealloc(PyObject *self)
{
    #define _env_ ((PyCOMPS_Env*)self)
    Py_XDECREF(_env_->p_group_list);
    Py_XDECREF(_env_->p_option_list);
    Py_XDECREF(_env_->p_name_by_lang);
    Py_XDECREF(_env_->p_desc_by_lang);
    COMPS_OBJECT_DESTROY(_env_->c_obj);
    Py_TYPE(self)->tp_free((PyObject*)self);
    #undef _env_
}

PyObject* PyCOMPSEnv_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Env *self;

    (void)args;
    (void)kwds;

    self = (PyCOMPS_Env*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->c_obj = COMPS_OBJECT_CREATE(COMPS_DocEnv, NULL);
        self->p_group_list = NULL;
        self->p_option_list = NULL;
        self->p_name_by_lang = NULL;
        self->p_desc_by_lang = NULL;
    }
    return (PyObject*) self;
}

int PyCOMPSEnv_init(PyCOMPS_Env *self, PyObject *args, PyObject *kwds)
{
    char *name, *id, *desc;
    int disp_ord;
    static char *kwlist[] = {"id", "name", "desc", "display_order", NULL};

    name = NULL;
    id = NULL;
    desc = NULL;
    disp_ord = -1;
    if (!args && !kwds){
        return 0;
    }
    else if (PyArg_ParseTupleAndKeywords(args, kwds, "|sssi", kwlist,
                                    &id, &name, &desc, &disp_ord)) {
        comps_docenv_set_id(self->c_obj, id, 1);
        comps_docenv_set_name(self->c_obj, name, 1);
        comps_docenv_set_desc(self->c_obj, desc, 1);
        if (disp_ord != -1)
            comps_docenv_set_display_order(self->c_obj, disp_ord, false);
        return 0;
    } else {
        return -1;
    }
}

PyObject* PyCOMPSEnv_str(PyObject *self) {
    char *x;
    COMPS_Object *id;
    PyObject *ret;
    id = comps_docenv_get_id(((PyCOMPS_Env*)self)->c_obj);
    x = comps_object_tostr(id);
    ret = PyUnicode_FromFormat("%s", x);
    free(x);
    comps_object_destroy(id);
    return ret;
}

PyObject* PyCOMPSEnv_repr(PyObject *self) {
    char *x;
    COMPS_Object *id;
    PyObject *ret;
    id = comps_docenv_get_id(((PyCOMPS_Env*)self)->c_obj);
    x = comps_object_tostr(id);
    ret = PyUnicode_FromFormat("<libcomps.Environment object '%s' at %p>", x, self);
    free(x);
    comps_object_destroy(id);
    return ret;
}

PyObject* PyCOMPSEnv_cmp(PyObject *self, PyObject *other, int op) {
    char ret;
    CMP_OP_EQ_NE_CHECK(op)

    if (other == NULL || ( Py_TYPE(other) != &PyCOMPS_EnvType
                           && other != Py_None)) {
        PyErr_SetString(PyExc_TypeError, "Not Environment instance");
        return NULL;
    }
    CMP_NONE_CHECK(op, self, other)
    ret = COMPS_OBJECT_CMP((COMPS_Object*)((PyCOMPS_Env*)self)->c_obj,
                           (COMPS_Object*)((PyCOMPS_Env*)other)->c_obj);
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

int pycomps_env_validate(COMPS_Object *obj) {
    COMPS_ValGenResult *result = comps_validate_execute(obj,
                                        COMPS_DocEnv_ValidateRules);
    int ret = __pycomps_validate_process(result);
    COMPS_OBJECT_DESTROY(result);
    return ret;
}

PyObject* PyCOMPSEnv_validate(PyCOMPS_Env *env) {
    if (pycomps_env_validate((COMPS_Object*)env->c_obj))
        return NULL;
    else
        Py_RETURN_NONE;
}

PyDoc_STRVAR(PyCOMPS_env_validate__doc__,
             "Validate inner environment structure. Raise exception on first"
             "occured error.\n\n"
             ":return: None if validation successful\n\n"
             ":raises ValueError: on first occured error");

PyMemberDef PyCOMPSEnv_members[] = {
    {NULL}};

PyMethodDef PyCOMPSEnv_methods[] = {
    {"validate", (PyCFunction)PyCOMPSEnv_validate, METH_NOARGS,
    PyCOMPS_env_validate__doc__},
    {NULL}  /* Sentinel */
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_IdClosure = {
    .get_f = &comps_docenv_get_id,
    .set_f = &comps_docenv_set_id,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_NameClosure = {
    .get_f = &comps_docenv_get_name,
    .set_f = &comps_docenv_set_name,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_DescClosure = {
    .get_f = &comps_docenv_get_desc,
    .set_f = &comps_docenv_set_desc,
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_DispOrdClosure = {
    .get_f = &comps_docenv_get_display_order,
    .set_f = &comps_docenv_set_display_order,
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_NameByLangClosure = {
    .p_offset = offsetof(PyCOMPS_Env, p_name_by_lang),
    .dict_offset = offsetof(COMPS_DocEnv, name_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_DescByLangClosure = {
    .p_offset = offsetof(PyCOMPS_Env, p_desc_by_lang),
    .dict_offset = offsetof(COMPS_DocEnv, desc_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_LIST_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_GroupIdsClosure = {
    .get_f = &comps_docenv_group_list,
    .set_f = &comps_docenv_set_group_list,
    .p_offset = offsetof(PyCOMPS_Env, p_group_list),
    .type = &PyCOMPS_GIDsType
};

__COMPS_LIST_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_OptionIdsClosure = {
    .get_f = &comps_docenv_option_list,
    .set_f = &comps_docenv_set_option_list,
    .p_offset = offsetof(PyCOMPS_Env, p_option_list),
    .type = &PyCOMPS_GIDsType
};

PyGetSetDef PyCOMPSEnv_getset[] = {
    {"id",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "environment string identificator", (void*)&DocEnv_IdClosure},
    {"name",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "name of environment", (void*)&DocEnv_NameClosure},
    {"desc",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "description of environment", (void*)&DocEnv_DescClosure},
    {"display_order",
     (getter)__PyCOMPS_get_numattr, (setter)__PyCOMPS_set_numattr,
     "environment display order attribute", (void*)&DocEnv_DispOrdClosure},
    {"group_ids",
     (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     ":py:class:`libcomps.IdList` of :py:class:`libcomps.GroupId` "
     "representing environments group list",
     &DocEnv_GroupIdsClosure},
    {"option_ids",
     (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     ":py:class:`libcomps.IdList` of :py:class:`libcomps.GroupId` "
     "representing environments option list",
     &DocEnv_OptionIdsClosure},
    {"name_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "dictionary of locale environment names", &DocEnv_NameByLangClosure},
    {"desc_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "dictionary of locale environment descriptions", &DocEnv_DescByLangClosure},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSEnv_Nums = {
    .nb_add = PyCOMPSEnv_union
};

PyTypeObject PyCOMPS_EnvType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.Environment",   /*tp_name*/
    sizeof(PyCOMPS_Env), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSEnv_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSEnv_cmp,            /*tp_compare*/
    &PyCOMPSEnv_repr,          /*tp_repr*/
    &PyCOMPSEnv_Nums,          /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    PyCOMPS_hash,              /*tp_hash */
    0,                         /*tp_call*/
    &PyCOMPSEnv_str,            /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Environment",          /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSEnv_cmp,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSEnv_methods,        /* tp_methods */
    PyCOMPSEnv_members,        /* tp_members */
    PyCOMPSEnv_getset,         /* tp_getset */
    0,//&PyCOMPS_SeqItemType,        /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSEnv_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSEnv_new,                 /* tp_new */};


COMPS_Object* comps_envs_in(PyObject *item) {
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Env*)item)->c_obj);
}

PyObject* comps_envs_out(COMPS_Object *cobj) {
    PyCOMPS_Env *ret;
    ret = (PyCOMPS_Env*)PyCOMPSEnv_new(&PyCOMPS_EnvType, NULL, NULL);
    PyCOMPSEnv_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->c_obj);
    ret->c_obj = (COMPS_DocEnv*)cobj;
    return (PyObject*)ret;
}

PyCOMPS_ItemInfo PyCOMPS_EnvsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_EnvType},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&comps_envs_in},
    .out_convert_func = &comps_envs_out,
    .item_types_len = 1,
    .props_offset = offsetof(COMPS_DocEnv, properties),
    .pre_checker = &pycomps_env_validate
};

int PyCOMPSEnvs_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds)
{
    (void) args;
    (void) kwds;
    self->it_info = &PyCOMPS_EnvsInfo;
    return 0;
}

COMPS_ObjList* comps_envs_union(COMPS_ObjList *envs1, COMPS_ObjList *envs2) {
    COMPS_ObjListIt *it;
    COMPS_Set *set;
    COMPS_DocEnv *tmpenv;
    COMPS_ObjList *ret;
    void *data;
    int index;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, &comps_object_destroy_v,
                                    &__comps_docenv_idcmp);
    for (it = envs1 ? envs1->first : NULL; it != NULL; it = it->next) {
        tmpenv = (COMPS_DocEnv*) comps_object_copy(it->comps_obj);
        comps_set_add(set, tmpenv);
        comps_objlist_append(ret, (COMPS_Object*)tmpenv);
    }
    for (it = envs2 ? envs2->first : NULL; it != NULL; it = it->next) {
        if ((data = comps_set_data_at(set, it->comps_obj)) != NULL) {
            index = comps_objlist_index(ret, data);
            comps_objlist_remove_at(ret, index);

            tmpenv = comps_docenv_union((COMPS_DocEnv*)data,
                                        (COMPS_DocEnv*)it->comps_obj);

            comps_objlist_insert_at_x(ret, index, (COMPS_Object*)tmpenv);

        } else {
            comps_objlist_append(ret, it->comps_obj);
        }
    }
    comps_set_destroy(&set);
    return ret;
}

PyObject* PyCOMPSEnvs_union(PyObject *self, PyObject *other) {
    PyCOMPS_Sequence *res;
    COMPS_ObjList *res_list;

    if (other == NULL || Py_TYPE(other) != &PyCOMPS_EnvsType) {
        PyErr_SetString(PyExc_TypeError, "Not EnvironmentList instance");
        return NULL;
    }

    res = (PyCOMPS_Sequence*) Py_TYPE(self)->tp_new(Py_TYPE(self), NULL, NULL);
    PyCOMPSEnvs_init(res, NULL, NULL);
    res_list = comps_envs_union(((PyCOMPS_Sequence*)self)->list,
                                ((PyCOMPS_Sequence*)other)->list);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)res)->list);
    res->list = res_list;

    return (PyObject*)res;
}

PyMemberDef PyCOMPSEnvs_members[] = {
    {NULL}};


PyMethodDef PyCOMPSEnvs_methods[] = {
     {"append", (PyCFunction)PyCOMPSSeq_append_unique, METH_O,
     "Append item to new of the list"},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSEnvs_Nums = {
    .nb_add = PyCOMPSEnvs_union
};


PyTypeObject PyCOMPS_EnvsType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.EnvList",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    sizeof(PyCOMPS_Env),   /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0, //&PyCOMPSEnvs_cmp,          /*tp_compare*/
    0,                         /*tp_repr*/
    &PyCOMPSEnvs_Nums,         /*tp_as_number*/
    0,//&PyCOMPSEnv_sequence,       /*tp_as_sequence*/
    &PyCOMPSSeq_mapping_extra, /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSSeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Comps Env list",          /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSSeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSEnvs_methods,        /* tp_methods */
    PyCOMPSEnvs_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_SeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSEnvs_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSSeq_new,                 /* tp_new */};

