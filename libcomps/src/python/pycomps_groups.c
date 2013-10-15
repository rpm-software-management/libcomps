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

#include "pycomps_groups.h"

PyObject* PyCOMPSGroup_union(PyObject *self, PyObject *other) {
    COMPS_DocGroup *g;
    PyObject *res;

    if (other->ob_type != &PyCOMPS_GroupType) {
        PyErr_SetString(PyExc_TypeError, "Not Group instance");
        return NULL;
    }
    g = comps_docgroup_union(((PyCOMPS_Group*)self)->group,
                             ((PyCOMPS_Group*)other)->group);
    res = PyCOMPSGroup_new(&PyCOMPS_GroupType, NULL, NULL);
    PyCOMPSGroup_init((PyCOMPS_Group*)res, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Group*)res)->group);
    ((PyCOMPS_Group*)res)->group = g;
    return res;
}

void PyCOMPSGroup_dealloc(PyObject *self)
{
    #define _group_ ((PyCOMPS_Group*)self)
    Py_XDECREF(_group_->p_packages);
    Py_XDECREF(_group_->p_name_by_lang);
    Py_XDECREF(_group_->p_desc_by_lang);
    COMPS_OBJECT_DESTROY(_group_->group);
    Py_TYPE(self)->tp_free((PyObject*)self);
    #undef _group_
}

PyObject* PyCOMPSGroup_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Group *self;
    (void) args;
    (void) kwds;

    self = (PyCOMPS_Group*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->group = (COMPS_DocGroup*)comps_object_create(
                                                    &COMPS_DocGroup_ObjInfo,
                                                    NULL);
        self->p_packages = NULL;
        self->p_name_by_lang = NULL;
        self->p_desc_by_lang = NULL;
    }
    return (PyObject*) self;
}

int PyCOMPSGroup_init(PyCOMPS_Group *self, PyObject *args, PyObject *kwds)
{
    #define _group_ ((PyCOMPS_Group*)self)->group
    char *name = NULL;
    char *id = NULL;
    char *desc = NULL;
    char *lang = NULL;
    unsigned def=0, uservis=0;
    int disp_order=-1;

    static char *kwlist[] = {"id", "name", "desc", "display_order", "default",
                              "uservisible", "langonly", NULL};

    if (!args && !kwds) {
        return 0;
    } else if (PyArg_ParseTupleAndKeywords(args, kwds, "|sssiiis", kwlist,
                                          &id, &name, &desc, &disp_order,
                                          &def, &uservis, &lang)) {
        //printf("%s %s %s %i %i %i %s\n",
        //       id, name, desc, def, uservis, disp_order, lang);
        comps_docgroup_set_id(_group_, id, 1);
        comps_docgroup_set_name(_group_, name, 1);
        comps_docgroup_set_desc(_group_, desc, 1);
        comps_docgroup_set_def(_group_, def);
        comps_docgroup_set_uservisible(_group_, uservis);
        if (disp_order > 0)
            comps_docgroup_set_display_order(_group_, disp_order);
        comps_docgroup_set_langonly(_group_, lang, 1);
        return 0;
    } else {
        return -1;
    }
    #undef _group_
}

PyObject* PyCOMPSGroup_str(PyObject *self) {
    char *x;
    COMPS_Object *id;
    PyObject *ret;
    id = comps_docgroup_get_id(((PyCOMPS_Group*)self)->group);
    x = comps_object_tostr(id);
    ret = PyUnicode_FromFormat("%s", x);
    free(x);
    comps_object_destroy(id);
    return ret;
}

PyObject* PyCOMPSGroup_repr(PyObject *self) {
    char *x;
    COMPS_Object *id;
    PyObject *ret;
    id = comps_docgroup_get_id(((PyCOMPS_Group*)self)->group);
    x = comps_object_tostr(id);
    ret = PyUnicode_FromFormat("<libcomps.Group object '%s' at %p>", x, self);
    free(x);
    comps_object_destroy(id);
    return ret;
}

int PyCOMPSGroup_print(PyObject *self, FILE *f, int flags) {
    #define _group_ ((PyCOMPS_Group*)self)
    COMPS_ObjListIt *it;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;
    char *id, *name, *desc, *lang, *def, *uservis, *disp_ord;
    COMPS_Object* tmp_prop;

    (void)flags;

    tmp_prop = comps_docgroup_get_id(_group_->group);
    id = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_docgroup_get_name(_group_->group);
    name = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_docgroup_get_desc(_group_->group);
    desc = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_docgroup_get_display_order(_group_->group);
    disp_ord = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_docgroup_get_langonly(_group_->group);
    lang = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_docgroup_get_def(_group_->group);
    def = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;
    tmp_prop = comps_docgroup_get_uservisible(_group_->group);
    uservis = (tmp_prop)?comps_object_tostr(tmp_prop):NULL;


    fprintf(f, "<COMPS_Group: id='%s', name='%s', description='%s',  "
            "default='%s', uservisible='%s', lang_only='%s', display_order=%s ",
            id, name, desc, def, uservis, lang, disp_ord);
    free(id);
    free(name);
    free(desc);
    free(lang);
    free(def);
    free(uservis);
    free(disp_ord);

    fprintf(f, "name_by_lang={");
    pairlist = comps_objrtree_pairs(_group_->group->name_by_lang);
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
    pairlist = comps_objrtree_pairs(_group_->group->desc_by_lang);
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

    fprintf(f, ", packages=[");
    if (_group_->group->packages) {
        for (it = _group_->group->packages->first;
             it != _group_->group->packages->last; it = it->next){
            id = comps_object_tostr(it->comps_obj);
            fprintf(f,"%s, ", id);
            free(id);
        }
        if (it) {
            id = comps_object_tostr(it->comps_obj);
            fprintf(f,"%s", id);
            free(id);
        }
    }
    fprintf(f, "]>");
    return 0;
    #undef _group_
}

PyObject* PyCOMPSGroup_cmp(PyObject *self, PyObject *other, int op) {
    char ret;

    // Only eq and neq operators allowed
    CMP_OP_EQ_NE_CHECK(op)
    //printf("pygroup cmp\n");

    if (other == NULL || ( Py_TYPE(other) != Py_TYPE(self)
                           && other != Py_None)) {
        PyErr_Format(PyExc_TypeError, "Not %s instance",
                                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    CMP_NONE_CHECK(op, self, other)

    ret = COMPS_OBJECT_CMP(((PyCOMPS_Group*)self)->group,
                           ((PyCOMPS_Group*)other)->group);
    //printf("pygroup cmp 2\n");
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

int pycomps_group_boolattr_setter(PyObject *self, PyObject *val, void *closure) {
    long tmp;
    COMPS_Object *tmp_prop;
    if (!val) {
        PyErr_Format(PyExc_TypeError, "Can't delete %s", (char*)closure);
        return -1;
    }
    if (val && !PyBool_Check(val)) {
        PyErr_SetString(PyExc_TypeError, "Not bool object");
        return -1;
    }
    tmp = (val == Py_True)?1:0;
    tmp_prop = (COMPS_Object*) comps_num(tmp);
    comps_objdict_set_x(((PyCOMPS_Group*)self)->group->properties,
                   (char*)closure, tmp_prop);
    return 0;
}

PyObject* pycomps_group_boolattr_getter(PyObject *self, void *closure) {

    COMPS_Object *tmp_prop;
    PyObject *ret;
    tmp_prop = comps_objdict_get(((PyCOMPS_Group*)self)->group->properties,
                                 (char*)closure);
    if (tmp_prop) {
        ret = PyBool_FromLong(((COMPS_Num*)tmp_prop)->val);
        COMPS_OBJECT_DESTROY(tmp_prop);
        return ret;
    } else
        Py_RETURN_NONE;
}

PyMemberDef PyCOMPSGroup_members[] = {
    {NULL}};

PyMethodDef PyCOMPSGroup_methods[] = {
    {NULL}  /* Sentinel */
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_IdClosure = {
    .get_f = &comps_docgroup_get_id,
    .set_f = &comps_docgroup_set_id,
    .c_offset = offsetof(PyCOMPS_Group, group)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_NameClosure = {
    .get_f = &comps_docgroup_get_name,
    .set_f = &comps_docgroup_set_name,
    .c_offset = offsetof(PyCOMPS_Group, group)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_DescClosure = {
    .get_f = &comps_docgroup_get_desc,
    .set_f = &comps_docgroup_set_desc,
    .c_offset = offsetof(PyCOMPS_Group, group)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_LangClosure = {
    .get_f = &comps_docgroup_get_langonly,
    .set_f = &comps_docgroup_set_langonly,
    .c_offset = offsetof(PyCOMPS_Group, group)
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_DispOrdClosure = {
    .get_f = &comps_docgroup_get_display_order,
    .set_f = &comps_docgroup_set_display_order,
    .c_offset = offsetof(PyCOMPS_Group, group)
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_NameByLangClosure = {
    .c_offset = offsetof(PyCOMPS_Group, group),
    .p_offset = offsetof(PyCOMPS_Group, p_name_by_lang),
    .dict_offset = offsetof(COMPS_DocGroup, name_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_DescByLangClosure = {
    .c_offset = offsetof(PyCOMPS_Group, group),
    .p_offset = offsetof(PyCOMPS_Group, p_desc_by_lang),
    .dict_offset = offsetof(COMPS_DocGroup, desc_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_LIST_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_Packages = {
    .get_f = &comps_docgroup_packages,
    .set_f = &comps_docgroup_set_packages,
    .p_offset = offsetof(PyCOMPS_Group, p_packages),
    .c_offset = offsetof(PyCOMPS_Group, group),
    .type = &PyCOMPS_PacksType
};

PyGetSetDef PyCOMPSGroup_getset[] = {
    {"id",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Group id", (void*)&DocGroup_IdClosure},
    {"name",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Group name", (void*)&DocGroup_NameClosure},
    {"desc",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Group description", (void*)&DocGroup_DescClosure},
    {"lang_only",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Group langonly", (void*)&DocGroup_LangClosure},
    {"display_order",
     (getter)__PyCOMPS_get_numattr, (setter)__PyCOMPS_set_numattr,
     "Group display order attribute", (void*)&DocGroup_DispOrdClosure},
    {"uservisible",
     (getter)pycomps_group_boolattr_getter, (setter)pycomps_group_boolattr_setter,
     "Group uservisible attribute", "uservisible"},
    {"default",
     (getter)pycomps_group_boolattr_getter, (setter)pycomps_group_boolattr_setter,
     "Group default attribute", "def"},
    {"packages",
    (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     "Group packages",
     (void*)&DocGroup_Packages},
    {"name_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "Group name locales", &DocGroup_NameByLangClosure},
    {"desc_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "Group description locales", &DocGroup_DescByLangClosure},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSGroup_Nums = {
    .nb_add = PyCOMPSGroup_union
};


PyTypeObject PyCOMPS_GroupType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Group",   /*tp_name*/
    sizeof(PyCOMPS_Group), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSGroup_dealloc, /*tp_dealloc*/
    PyCOMPSGroup_print,        /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSGroup_cmp,            /*tp_compare*/
    &PyCOMPSGroup_repr,                         /*tp_repr*/
    &PyCOMPSGroup_Nums,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    PyCOMPS_hash,              /*tp_hash */
    0,                         /*tp_call*/
    &PyCOMPSGroup_str,            /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Group",          /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    PyCOMPSGroup_cmp,    /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSGroup_methods,        /* tp_methods */
    PyCOMPSGroup_members,        /* tp_members */
    PyCOMPSGroup_getset,         /* tp_getset */
    0,//&PyCOMPS_SeqItemType,        /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSGroup_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSGroup_new,                 /* tp_new */};

COMPS_Object* comps_groups_in(PyObject *item) {
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Group*)item)->group);
}

PyObject* comps_groups_out(COMPS_Object *cobj) {
    PyCOMPS_Group *ret;
    ret = (PyCOMPS_Group*)PyCOMPSGroup_new(&PyCOMPS_GroupType, NULL, NULL);
    PyCOMPSGroup_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->group);
    ret->group = (COMPS_DocGroup*)cobj;
    return (PyObject*)ret;
}

PyCOMPS_ItemInfo PyCOMPS_GroupsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_GroupType},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&comps_groups_in},
    .out_convert_func = &comps_groups_out,
    .item_types_len = 1,
    .props_offset = offsetof(COMPS_DocGroup, properties)
};

int PyCOMPSGroups_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds)
{
    (void) args;
    (void) kwds;
    self->it_info = &PyCOMPS_GroupsInfo;
    return 0;
}

COMPS_ObjList* comps_groups_union(COMPS_ObjList *groups1,
                                  COMPS_ObjList *groups2) {
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_ObjList *ret;
    COMPS_DocGroup *tmpgroup;
    COMPS_ObjListIt *it;
    void *tmpdata;

    ret = (COMPS_ObjList*)comps_object_create(&COMPS_ObjList_ObjInfo, NULL);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__comps_docgroup_idcmp);

    for (it = groups1 ? groups1->first : NULL; it != NULL; it = it->next) {
        comps_set_add(set, comps_object_copy(it->comps_obj));
    }
    for (it = groups2 ? groups2->first : NULL; it != NULL; it = it->next) {
        if (comps_set_in(set, it->comps_obj)) {
            tmpgroup = comps_docgroup_union(
                                (COMPS_DocGroup*)it->comps_obj,
                                (COMPS_DocGroup*)comps_set_data_at(set,
                                                                   it->comps_obj));
            tmpdata = comps_set_data_at(set, it->comps_obj);
            comps_set_remove(set, it->comps_obj);
            comps_object_destroy((COMPS_Object*)tmpdata);
            comps_set_add(set, tmpgroup);
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

PyObject* PyCOMPSGroups_union(PyObject *self, PyObject *other) {
    PyCOMPS_Sequence *res;
    COMPS_ObjList *res_list;

    if (other == NULL || Py_TYPE(other) != &PyCOMPS_GroupsType) {
        PyErr_SetString(PyExc_TypeError, "Not GroupList instance");
        return NULL;
    }

    res = (PyCOMPS_Sequence*) Py_TYPE(self)->tp_new(Py_TYPE(self), NULL, NULL);
    PyCOMPSGroups_init(res, NULL, NULL);

    res_list = comps_groups_union(((PyCOMPS_Sequence*)self)->list,
                                ((PyCOMPS_Sequence*)other)->list);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)res)->list);
    res->list = res_list;
    return (PyObject*)res;
}

PyObject* PyCOMPSGroups_get(PyObject *self, PyObject *key) {
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

PyMemberDef PyCOMPSGroups_members[] = {
    {NULL}};

PyMethodDef PyCOMPSGroups_methods[] = {
     {"append", (PyCFunction)PyCOMPSSeq_append_unique, METH_O,
     "Append item to new of the list"},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSGroups_Nums = {
    .nb_add = PyCOMPSGroups_union
};

PyTypeObject PyCOMPS_GroupsType = {
    PY_OBJ_HEAD_INIT
    "libcomps.GroupList",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    sizeof(PyCOMPS_Group),   /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//&PyCOMPSGroups_cmp,          /*tp_compare*/
    0,                         /*tp_repr*/
    &PyCOMPSGroups_Nums,         /*tp_as_number*/
    0,//&PyCOMPSGroup_sequence,       /*tp_as_sequence*/
    &PyCOMPSSeq_mapping_extra, /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSSeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Group list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSSeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSGroups_methods,        /* tp_methods */
    PyCOMPSGroups_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_SeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSGroups_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSSeq_new,                 /* tp_new */};

COMPS_Object* comps_pkgs_in(PyObject *item) {
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Package*)item)->package);
}

PyObject* comps_pkgs_out(COMPS_Object *cobj) {
    PyCOMPS_Package *ret;
    ret = (PyCOMPS_Package*)PyCOMPSPack_new(&PyCOMPS_PackType, NULL, NULL);
    PyCOMPSPack_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->package);
    ret->package = (COMPS_DocGroupPackage*)cobj;
    return (PyObject*)ret;
}

PyCOMPS_ItemInfo PyCOMPS_PkgsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_PackType},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&comps_pkgs_in},
    .out_convert_func = &comps_pkgs_out,
    .item_types_len = 1
};

int PyCOMPSPacks_init(PyCOMPS_Sequence *self, PyObject *args, PyObject *kwds)
{
    (void) args;
    (void) kwds;
    self->it_info = &PyCOMPS_PkgsInfo;
    return 0;
}

PyMemberDef PyCOMPSPacks_members[] = {
    {NULL}};

PyMethodDef PyCOMPSPacks_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_PacksType = {
    PY_OBJ_HEAD_INIT
    "libcomps.PackageList",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    0,                        /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,//&PyCOMPSPack_sequence,       /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSSeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Group Package list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSSeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSPacks_methods,        /* tp_methods */
    PyCOMPSPacks_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_SeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSPacks_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSSeq_new,                 /* tp_new */};


void PyCOMPSPack_dealloc(PyCOMPS_Package *self)
{
    COMPS_OBJECT_DESTROY(self->package);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSPack_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Package *self;

   (void) args;
   (void) kwds;

    self = (PyCOMPS_Package*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->package = (COMPS_DocGroupPackage*)
                        comps_object_create(&COMPS_DocGroupPackage_ObjInfo,
                                            NULL);
    }
    return (PyObject*) self;
}

int PyCOMPSPack_init(PyCOMPS_Package *self, PyObject *args, PyObject *kwds)
{
    char *name = NULL;
    int type = COMPS_PACKAGE_UNKNOWN;

   (void) kwds;

    if (args!=NULL && PyArg_ParseTuple(args, "|si", &name, &type)) {
        comps_docpackage_set_name(((PyCOMPS_Package*)self)->package, name, 1);
        comps_docpackage_set_type(((PyCOMPS_Package*)self)->package, type);
        return 0;
    } else {
        ((PyCOMPS_Package*)self)->package->name = NULL;
        ((PyCOMPS_Package*)self)->package->type = COMPS_PACKAGE_UNKNOWN;
        return 0;
    }
    return 1;
}

PyObject* PyCOMPSPack_strget_(PyCOMPS_Package *self, void *closure) {
    char *tmp = GET_FROM(self->package, (size_t)closure);
    return PyUnicode_FromString(tmp);
}

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) DocGroupPkg_NameClosure = {
    .get_f = &comps_docpackage_get_name,
    .set_f = &comps_docpackage_set_name,
    .c_offset = offsetof(PyCOMPS_Package, package)
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) DocGroupPkg_RequiresClosure = {
    .get_f = &comps_docpackage_get_requires,
    .set_f = &comps_docpackage_set_requires,
    .c_offset = offsetof(PyCOMPS_Package, package)
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) DocGroupPkg_TypeClosure = {
    .get_f = &comps_docpackage_get_type,
    .set_f = &comps_docpackage_set_type_i,
    .c_offset = offsetof(PyCOMPS_Package, package)
};

PyGetSetDef pack_getset[] = {
    {"name",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Package name",
     (void*)&DocGroupPkg_NameClosure},
    {"requires",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "Package requires",
     (void*)&DocGroupPkg_RequiresClosure},
    {"type",
     (getter)__PyCOMPS_get_numattr, (setter)__PyCOMPS_set_numattr,
     "Package type",
     (void*)&DocGroupPkg_TypeClosure},
    {NULL}  /* Sentinel */
};

PyObject* PyCOMPSPack_str(PyObject *self) {
    char *x;
    COMPS_Object *name;
    PyObject *ret;
    name = comps_docpackage_get_name(((PyCOMPS_Package*)self)->package);
    x = comps_object_tostr(name);
    ret = PyUnicode_FromFormat("%s", x);
    free(x);
    comps_object_destroy(name);
    return ret;
}

PyObject* PyCOMPSPack_repr(PyObject *self) {
    char *x;
    COMPS_Object *name;
    PyObject *ret;
    name = comps_docpackage_get_name(((PyCOMPS_Package*)self)->package);
    x = comps_object_tostr(name);
    ret = PyUnicode_FromFormat("<libcomps.Package object '%s' at %p>", x, self);
    free(x);
    comps_object_destroy(name);
    return ret;
}

int PyCOMPSPack_print(PyObject *self, FILE *f, int flags) {
    (void) flags;
    char *name, *requires;
    const char *type;
    type = comps_docpackage_type_str(((PyCOMPS_Package*)self)->package->type);
    name = comps_object_tostr((COMPS_Object*)
                              ((PyCOMPS_Package*)self)->package->name);
    if (((PyCOMPS_Package*)self)->package->requires) {
        requires = comps_object_tostr((COMPS_Object*)
                                  ((PyCOMPS_Package*)self)->package->requires);
        fprintf(f, "<COPMS_Package name='%s' type='%s' requires='%s'>",
                name, type, requires);
        free(requires);
    } else
        fprintf(f, "<COPMS_Package name='%s' type='%s'>", name, type);
    free(name);
    return 0;
}

PyObject* PyCOMPSPack_cmp(PyObject *self, PyObject *other, int op) {
    char ret;
    CMP_OP_EQ_NE_CHECK(op)

    if (other == NULL || ( Py_TYPE(other) != &PyCOMPS_PackType
                           && other != Py_None)) {
        PyErr_Format(PyExc_TypeError, "Not %s instance",
                        Py_TYPE(self)->tp_name);
        return NULL;
    }
    CMP_NONE_CHECK(op, self, other)
    ret = COMPS_OBJECT_CMP((COMPS_Object*)((PyCOMPS_Package*)self)->package,
                           (COMPS_Object*)((PyCOMPS_Package*)other)->package);
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

PyMemberDef PyCOMPSPack_members[] = {
    {NULL}};

PyMethodDef PyCOMPSPack_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_PackType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Package",   /*tp_name*/
    sizeof(PyCOMPS_Package), /*tp_basicsize*/
    0,                        /*tp_itemsize*/
    (destructor)PyCOMPSPack_dealloc, /*tp_dealloc*/
    PyCOMPSPack_print,         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSPack_cmp,           /*tp_compare*/
    PyCOMPSPack_repr,          /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSPack_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Comps Group Package",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    PyCOMPSPack_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSPack_methods,        /* tp_methods */
    PyCOMPSPack_members,        /* tp_members */
    pack_getset,               /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSPack_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSPack_new,                 /* tp_new */};

