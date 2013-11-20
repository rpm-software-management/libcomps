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

#include "pycomps_23macros.h"
#include "pycomps_envs.h"
#include "libcomps/comps_set.h"

PyObject* PyCOMPSEnv_union(PyObject *self, PyObject *other) {
    COMPS_DocEnv *e;
    PyObject *res;

    if (Py_TYPE(other) != &PyCOMPS_EnvType) {
        PyErr_SetString(PyExc_TypeError, "Not Environment instance");
        return NULL;
    }
    e = comps_docenv_union(((PyCOMPS_Env*)self)->env,
                           ((PyCOMPS_Env*)other)->env);

    res = PyCOMPSEnv_new(&PyCOMPS_EnvType, NULL, NULL);
    PyCOMPSEnv_init((PyCOMPS_Env*)res, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Env*)res)->env);
    ((PyCOMPS_Env*)res)->env = e;
    return res;
}


void PyCOMPSEnv_dealloc(PyObject *self)
{
    #define _env_ ((PyCOMPS_Env*)self)
    Py_XDECREF(_env_->p_group_list);
    Py_XDECREF(_env_->p_option_list);
    Py_XDECREF(_env_->p_name_by_lang);
    Py_XDECREF(_env_->p_desc_by_lang);
    COMPS_OBJECT_DESTROY(_env_->env);
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
        self->env = (COMPS_DocEnv*)comps_object_create(&COMPS_DocEnv_ObjInfo,
                                                       NULL);
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
        comps_docenv_set_id(self->env, id, 1);
        comps_docenv_set_name(self->env, name, 1);
        comps_docenv_set_desc(self->env, desc, 1);
        if (disp_ord != -1)
            comps_docenv_set_display_order(self->env, disp_ord, false);
        return 0;
    } else {
        return -1;
    }
}

int PyCOMPSEnv_print(PyObject *self, FILE *f, int flags) {
    COMPS_ObjListIt *it;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;
    char *id, *name, *desc, *display_order;
    COMPS_Object *tmp_prop;

    #define _env_ ((PyCOMPS_Env*)self)->env
    (void)flags;

    tmp_prop = (COMPS_Object*)comps_docenv_get_id(_env_);
    id = (tmp_prop)?comps_object_tostr(tmp_prop): NULL;
    tmp_prop = (COMPS_Object*)comps_docenv_get_name(_env_);
    name = (tmp_prop)?comps_object_tostr(tmp_prop): NULL;
    tmp_prop = (COMPS_Object*)comps_docenv_get_desc(_env_);
    desc = (tmp_prop)?comps_object_tostr(tmp_prop): NULL;
    tmp_prop = (COMPS_Object*)comps_docenv_get_display_order(_env_);
    display_order = (tmp_prop)?comps_object_tostr(tmp_prop): NULL;

    fprintf(f, "<COMPS_Environment: id='%s'"
               "name='%s' description='%s' display_order=%s", id, name, desc,
                display_order);
    free(id);
    free(name);
    free(desc);
    free(display_order);

    fprintf(f, ", name_by_lang={");
    pairlist = comps_objrtree_pairs(_env_->name_by_lang);
    for (hsit = pairlist->first; hsit != pairlist->last; hsit = hsit->next) {
        name = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
        printf("'%s': '%s', ", ((COMPS_ObjRTreePair*)hsit->data)->key, name);
        free(name);
    }
    if (hsit) {
        name = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
        printf("'%s': '%s'}", ((COMPS_ObjRTreePair*)hsit->data)->key, name);
        free(name);
    } else {
        printf("}");
    }
    comps_hslist_destroy(&pairlist);

    fprintf(f, ", desc_by_lang={");
    pairlist = comps_objrtree_pairs(_env_->desc_by_lang);
    for (hsit = pairlist->first; hsit != pairlist->last; hsit = hsit->next) {
        name = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
        printf("'%s': '%s', ", ((COMPS_ObjRTreePair*)hsit->data)->key, name);
        free(name);
    }
    if (hsit) {
        name = comps_object_tostr(((COMPS_ObjRTreePair*)hsit->data)->data);
        printf("'%s': '%s'}", ((COMPS_ObjRTreePair*)hsit->data)->key, name);
        free(name);
    } else {
        printf("}");
    }
    comps_hslist_destroy(&pairlist);

    fprintf(f, ", group_list=[");
    if (_env_->group_list) {
        for (it = _env_->group_list->first;
             it != _env_->group_list->last; it = it->next){
            id = comps_object_tostr(it->comps_obj);
            fprintf(f, "'%s', ", id);
            free(id);
        }
        if (it) {
            id = comps_object_tostr(it->comps_obj);
            fprintf(f, "'%s'", id);
            free(id);
        }
    }
    fprintf(f, "]");
    fprintf(f, ", option_list=[");
    if (_env_->option_list) {
        for (it = _env_->option_list->first;
             it != _env_->option_list->last; it = it->next){
            id = comps_object_tostr(it->comps_obj);
            fprintf(f, "'%s', ", id);
            free(id);
        }
        if (it) {
            id = comps_object_tostr(it->comps_obj);
            fprintf(f, "'%s'", id);
            free(id);
        }
    }
    fprintf(f, "]>");
    return 0;
    #undef _env_
}

PyObject* PyCOMPSEnv_str(PyObject *self) {
    char *x;
    COMPS_Object *id;
    PyObject *ret;
    id = comps_docenv_get_id(((PyCOMPS_Env*)self)->env);
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
    id = comps_docenv_get_id(((PyCOMPS_Env*)self)->env);
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
    ret = COMPS_OBJECT_CMP((COMPS_Object*)((PyCOMPS_Env*)self)->env,
                           (COMPS_Object*)((PyCOMPS_Env*)other)->env);
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

PyMemberDef PyCOMPSEnv_members[] = {
    {NULL}};

PyMethodDef PyCOMPSEnv_methods[] = {
    {NULL}  /* Sentinel */
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_IdClosure = {
    .get_f = &comps_docenv_get_id,
    .set_f = &comps_docenv_set_id,
    .c_offset = offsetof(PyCOMPS_Env, env)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_NameClosure = {
    .get_f = &comps_docenv_get_name,
    .set_f = &comps_docenv_set_name,
    .c_offset = offsetof(PyCOMPS_Env, env)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_DescClosure = {
    .get_f = &comps_docenv_get_desc,
    .set_f = &comps_docenv_set_desc,
    .c_offset = offsetof(PyCOMPS_Env, env)
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_DispOrdClosure = {
    .get_f = &comps_docenv_get_display_order,
    .set_f = &comps_docenv_set_display_order,
    .c_offset = offsetof(PyCOMPS_Env, env)
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_NameByLangClosure = {
    .c_offset = offsetof(PyCOMPS_Env, env),
    .p_offset = offsetof(PyCOMPS_Env, p_name_by_lang),
    .dict_offset = offsetof(COMPS_DocEnv, name_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_DescByLangClosure = {
    .c_offset = offsetof(PyCOMPS_Env, env),
    .p_offset = offsetof(PyCOMPS_Env, p_desc_by_lang),
    .dict_offset = offsetof(COMPS_DocEnv, desc_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_LIST_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_GroupIdsClosure = {
    .get_f = &comps_docenv_group_list,
    .set_f = &comps_docenv_set_group_list,
    .p_offset = offsetof(PyCOMPS_Env, p_group_list),
    .c_offset = offsetof(PyCOMPS_Env, env),
    .type = &PyCOMPS_GIDsType
};

__COMPS_LIST_GETSET_CLOSURE(COMPS_DocEnv) DocEnv_OptionIdsClosure = {
    .get_f = &comps_docenv_option_list,
    .set_f = &comps_docenv_set_option_list,
    .p_offset = offsetof(PyCOMPS_Env, p_option_list),
    .c_offset = offsetof(PyCOMPS_Env, env),
    .type = &PyCOMPS_GIDsType
};

PyGetSetDef PyCOMPSEnv_getset[] = {
    {"id",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Env id", (void*)&DocEnv_IdClosure},
    {"name",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Env name", (void*)&DocEnv_NameClosure},
    {"desc",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Env description", (void*)&DocEnv_DescClosure},
    {"display_order",
     (getter)__PyCOMPS_get_numattr, (setter)__PyCOMPS_set_numattr,
     "Env display order attribute", (void*)&DocEnv_DispOrdClosure},
    {"group_ids",
     (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     "Env group ids",
     &DocEnv_GroupIdsClosure},
    {"option_ids",
     (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     "Env group ids",
     &DocEnv_OptionIdsClosure},
    {"name_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "Env name locales", &DocEnv_NameByLangClosure},
    {"desc_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "Env description locales", &DocEnv_DescByLangClosure},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSEnv_Nums = {
    .nb_add = PyCOMPSEnv_union
};

PyTypeObject PyCOMPS_EnvType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Env",   /*tp_name*/
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
    "Comps Env",          /* tp_doc */
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
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Env*)item)->env);
}

PyObject* comps_envs_out(COMPS_Object *cobj) {
    PyCOMPS_Env *ret;
    ret = (PyCOMPS_Env*)PyCOMPSEnv_new(&PyCOMPS_EnvType, NULL, NULL);
    PyCOMPSEnv_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->env);
    ret->env = (COMPS_DocEnv*)cobj;
    return (PyObject*)ret;
}

PyCOMPS_ItemInfo PyCOMPS_EnvsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_EnvType},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&comps_envs_in},
    .out_convert_func = &comps_envs_out,
    .item_types_len = 1,
    .props_offset = offsetof(COMPS_DocEnv, properties)
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
    COMPS_HSListItem *hsit;
    COMPS_ObjList *ret;
    void *tmpdata;

    ret = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo, NULL);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docenv_idcmp);
    if (envs1) {
        for (it = envs1->first; it != NULL; it = it->next) {
            comps_set_add(set, comps_object_copy(it->comps_obj));
        }
    }
    if (envs2) {
        for (it = envs2->first; it != NULL; it = it->next) {
            if (comps_set_in(set, it->comps_obj)) {
                tmpenv = comps_docenv_union(
                                    (COMPS_DocEnv*)comps_set_data_at(set,
                                                                it->comps_obj),
                                    (COMPS_DocEnv*)it->comps_obj);
                tmpdata = comps_set_data_at(set, it->comps_obj);
                comps_set_remove(set, it->comps_obj);
                comps_object_destroy((COMPS_Object*)tmpdata);
                comps_set_add(set, tmpenv);
            } else {
                comps_set_add(set, comps_object_copy(it->comps_obj));
            }
        }
    }
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        comps_objlist_append_x(ret, (COMPS_Object*)hsit->data);
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
    PY_OBJ_HEAD_INIT
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Env list",           /* tp_doc */
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

