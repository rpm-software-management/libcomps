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
#include "libcomps/comps_set.h"

PyObject* PyCOMPSGroup_union(PyObject *self, PyObject *other) {
    COMPS_DocGroup *g;
    PyObject *res;

    if (other->ob_type != &PyCOMPS_GroupType) {
        PyErr_SetString(PyExc_TypeError, "Not Group instance");
        return NULL;
    }
    g = comps_docgroup_union(((PyCOMPS_Group*)self)->c_obj,
                             ((PyCOMPS_Group*)other)->c_obj);
    res = PyCOMPSGroup_new(&PyCOMPS_GroupType, NULL, NULL);
    PyCOMPSGroup_init((PyCOMPS_Group*)res, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Group*)res)->c_obj);
    ((PyCOMPS_Group*)res)->c_obj = g;
    return res;
}

void PyCOMPSGroup_dealloc(PyObject *self)
{
    #define _group_ ((PyCOMPS_Group*)self)
    Py_XDECREF(_group_->p_packages);
    Py_XDECREF(_group_->p_name_by_lang);
    Py_XDECREF(_group_->p_desc_by_lang);
    COMPS_OBJECT_DESTROY(_group_->c_obj);
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
        self->c_obj = COMPS_OBJECT_CREATE(COMPS_DocGroup, NULL);
        self->p_packages = NULL;
        self->p_name_by_lang = NULL;
        self->p_desc_by_lang = NULL;
    }
    return (PyObject*) self;
}

int PyCOMPSGroup_init(PyCOMPS_Group *self, PyObject *args, PyObject *kwds)
{
    #define _group_ ((PyCOMPS_Group*)self)->c_obj
    char *name = NULL;
    char *id = NULL;
    char *desc = NULL;
    char *lang = NULL;
    unsigned def=0, uservis=1;
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
        comps_docgroup_set_def(_group_, def, false);
        comps_docgroup_set_uservisible(_group_, uservis, false);
        if (disp_order > 0)
            comps_docgroup_set_display_order(_group_, disp_order, false);
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
    id = comps_docgroup_get_id(((PyCOMPS_Group*)self)->c_obj);
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
    id = comps_docgroup_get_id(((PyCOMPS_Group*)self)->c_obj);
    x = comps_object_tostr(id);
    ret = PyUnicode_FromFormat("<libcomps.Group object '%s' at %p>", x, self);
    free(x);
    comps_object_destroy(id);
    return ret;
}

PyObject* PyCOMPSGroup_cmp(PyObject *self, PyObject *other, int op) {
    char ret;

    // Only eq and neq operators allowed
    CMP_OP_EQ_NE_CHECK(op)

    if (other == NULL || ( Py_TYPE(other) != Py_TYPE(self)
                           && other != Py_None)) {
        PyErr_Format(PyExc_TypeError, "Not %s instance",
                                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    CMP_NONE_CHECK(op, self, other)

    ret = COMPS_OBJECT_CMP(((PyCOMPS_Group*)self)->c_obj,
                           ((PyCOMPS_Group*)other)->c_obj);
    //printf("cmpgroup :%d\n",ret);
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
    if (!PyBool_Check(val)) {
        PyErr_SetString(PyExc_TypeError, "Not bool object");
        return -1;
    }
    tmp = (val == Py_True)?1:0;
    tmp_prop = (COMPS_Object*) comps_num(tmp);
    comps_objdict_set_x(((PyCOMPS_Group*)self)->c_obj->properties,
                   (char*)closure, tmp_prop);
    return 0;
}

PyObject* pycomps_group_boolattr_getter(PyObject *self, void *closure) {

    COMPS_Object *tmp_prop;
    PyObject *ret;
    tmp_prop = comps_objdict_get(((PyCOMPS_Group*)self)->c_obj->properties,
                                 (char*)closure);
    if (tmp_prop) {
        ret = PyBool_FromLong(((COMPS_Num*)tmp_prop)->val);
        COMPS_OBJECT_DESTROY(tmp_prop);
        return ret;
    } else
        Py_RETURN_NONE;
}

int pycomps_group_validate(COMPS_Object *obj) {
    COMPS_ValGenResult *result = comps_validate_execute(obj,
                                        COMPS_DocGroup_ValidateRules);
    int ret = __pycomps_validate_process(result);
    COMPS_OBJECT_DESTROY(result);
    return ret;
}

PyObject* PyCOMPSGroup_validate(PyCOMPS_Group *group) {
    if (pycomps_group_validate((COMPS_Object*)group->c_obj)) {
        return NULL;
    } else {
        Py_RETURN_NONE;
    }
}

PyMemberDef PyCOMPSGroup_members[] = {
    {NULL}};

PyDoc_STRVAR(PyCOMPS_group_validate__doc__,
             "Validate inner group structure. Raise exception on first"
             "occured error.\n\n"
             ":return: None if validation successful\n\n"
             ":raises ValueError: on first occured error");

PyDoc_STRVAR(PyCOMPS_group_packages_match__doc__,
             "Return list of packages matching selected criteria\n\n"
             ":return: List of packages\n\n");

PyMethodDef PyCOMPSGroup_methods[] = {
    {"validate", (PyCFunction)PyCOMPSGroup_validate, METH_NOARGS,
     PyCOMPS_group_validate__doc__},
    {"packages_match", (PyCFunction)PyCOMPSGroup_packages_match,
     METH_VARARGS | METH_KEYWORDS,
     PyCOMPS_group_packages_match__doc__},
    {NULL}  /* Sentinel */
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_IdClosure = {
    .get_f = &comps_docgroup_get_id,
    .set_f = &comps_docgroup_set_id,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_NameClosure = {
    .get_f = &comps_docgroup_get_name,
    .set_f = &comps_docgroup_set_name,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_DescClosure = {
    .get_f = &comps_docgroup_get_desc,
    .set_f = &comps_docgroup_set_desc,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_LangClosure = {
    .get_f = &comps_docgroup_get_langonly,
    .set_f = &comps_docgroup_set_langonly,
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_DispOrdClosure = {
    .get_f = &comps_docgroup_get_display_order,
    .set_f = &comps_docgroup_set_display_order,
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_NameByLangClosure = {
    .p_offset = offsetof(PyCOMPS_Group, p_name_by_lang),
    .dict_offset = offsetof(COMPS_DocGroup, name_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_DICT_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_DescByLangClosure = {
    .p_offset = offsetof(PyCOMPS_Group, p_desc_by_lang),
    .dict_offset = offsetof(COMPS_DocGroup, desc_by_lang),
    .dict_info = &PyCOMPS_StrDictInfo,
    .dict_type = &PyCOMPS_StrDictType
};

__COMPS_LIST_GETSET_CLOSURE(COMPS_DocGroup) DocGroup_Packages = {
    .get_f = &comps_docgroup_packages,
    .set_f = &comps_docgroup_set_packages,
    .p_offset = offsetof(PyCOMPS_Group, p_packages),
    .type = &PyCOMPS_PacksType
};

PyGetSetDef PyCOMPSGroup_getset[] = {
    {"id",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "group string identificator", (void*)&DocGroup_IdClosure},
    {"name",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "name of group", (void*)&DocGroup_NameClosure},
    {"desc",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "descrition of group", (void*)&DocGroup_DescClosure},
    {"lang_only",
     (getter)__PyCOMPS_get_strattr, (setter)__PyCOMPS_set_strattr,
     "group langonly attribute", (void*)&DocGroup_LangClosure},
    {"display_order",
     (getter)__PyCOMPS_get_numattr, (setter)__PyCOMPS_set_numattr,
     "Group display order attribute", (void*)&DocGroup_DispOrdClosure},
    {"uservisible",
     (getter)pycomps_group_boolattr_getter, (setter)pycomps_group_boolattr_setter,
     "Group uservisible attribute", "uservisible"},
    {"biarchonly",
     (getter)pycomps_group_boolattr_getter, (setter)pycomps_group_boolattr_setter,
     "Group uservisible attribute", "biarchonly"},
    {"default",
     (getter)pycomps_group_boolattr_getter, (setter)pycomps_group_boolattr_setter,
     "Group default attribute", "def"},
    {"packages",
    (getter)__PyCOMPS_get_ids, (setter)__PyCOMPS_set_ids,
     ":py:class:`libcomps.PackageList` of :py:class:`libcomps.Package`",
     (void*)&DocGroup_Packages},
    {"name_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "dictionary of locale group names", &DocGroup_NameByLangClosure},
    {"desc_by_lang",
    (getter)__PyCOMPS_get_dict, (setter)__PyCOMPS_set_dict,
     "dictionary of locale group descriptions", &DocGroup_DescByLangClosure},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSGroup_Nums = {
    .nb_add = PyCOMPSGroup_union
};


PyTypeObject PyCOMPS_GroupType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.Group",   /*tp_name*/
    sizeof(PyCOMPS_Group), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPSGroup_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
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
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
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
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Group*)item)->c_obj);
}

PyObject* comps_groups_out(COMPS_Object *cobj) {
    PyCOMPS_Group *ret;
    ret = (PyCOMPS_Group*)PyCOMPSGroup_new(&PyCOMPS_GroupType, NULL, NULL);
    PyCOMPSGroup_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->c_obj);
    ret->c_obj = (COMPS_DocGroup*)cobj;
    return (PyObject*)ret;
}

PyCOMPS_ItemInfo PyCOMPS_GroupsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_GroupType},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&comps_groups_in},
    .out_convert_func = &comps_groups_out,
    .item_types_len = 1,
    .props_offset = offsetof(COMPS_DocGroup, properties),
    .pre_checker = &pycomps_group_validate
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
    COMPS_Set *set;
    COMPS_ObjList *ret;
    COMPS_DocGroup *tmpgroup;
    COMPS_ObjListIt *it;
    void *data;
    int index;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, &comps_object_destroy_v,
                                    &__comps_docgroup_idcmp);
    for (it = groups1 ? groups1->first : NULL; it != NULL; it = it->next) {
        tmpgroup = (COMPS_DocGroup*) comps_object_copy(it->comps_obj);
        comps_set_add(set, tmpgroup);
        comps_objlist_append(ret, (COMPS_Object*)tmpgroup);
    }
    for (it = groups2 ? groups2->first : NULL; it != NULL; it = it->next) {
        if ((data = comps_set_data_at(set, it->comps_obj)) != NULL) {
            index = comps_objlist_index(ret, data);
            comps_objlist_remove_at(ret, index);
            tmpgroup = comps_docgroup_union((COMPS_DocGroup*)data,
                                            (COMPS_DocGroup*)it->comps_obj);

            comps_objlist_insert_at_x(ret, index, (COMPS_Object*)tmpgroup);

        } else {
            comps_objlist_append(ret, it->comps_obj);
        }
    }
    comps_set_destroy(&set);
    return ret;
}
char __pycomps_pkg_type_check(PyObject* pobj, void *cobj) {
    long cint;
    if (!PyLong_Check(pobj)) {
        cobj = NULL;
        return 0;
    }
    cint = PyLong_AsLong(pobj);
    switch(cint) {
        case COMPS_PACKAGE_DEFAULT:
        case COMPS_PACKAGE_OPTIONAL:
        case COMPS_PACKAGE_CONDITIONAL:
        case COMPS_PACKAGE_MANDATORY:
        case COMPS_PACKAGE_UNKNOWN:
            (*(int*)cobj) = cint;
            return 1;
        break;
        default:
            return 0;
        break;
    }
}

PyObject* PyCOMPSGroup_packages_match(PyObject *self, PyObject *args, PyObject *kwds) {

    PyObject *ret;
    int type=-1;
    char *name = NULL;
    char *keywords[] = {"name", "type", NULL};
    COMPS_ObjList * list;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "|sO&", keywords, &name,
                                    __pycomps_pkg_type_check, &type)){
    } else {
        return NULL;
    }
    list = comps_docgroup_get_packages(((PyCOMPS_Group*)self)->c_obj, name, type);
    ret = PyCOMPSSeq_new(&PyCOMPS_PacksType, NULL, NULL);
    Py_TYPE(ret)->tp_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)ret)->list);
    ((PyCOMPS_Sequence*)ret)->list = list;
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
    } else if (PyLong_Check(key)) {
        return list_getitem(self, PyLong_AsLong(key));
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
    PyVarObject_HEAD_INIT(NULL, 0)
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
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
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
    return comps_object_incref((COMPS_Object*)((PyCOMPS_Package*)item)->c_obj);
}

PyObject* comps_pkgs_out(COMPS_Object *cobj) {
    PyCOMPS_Package *ret;
    ret = (PyCOMPS_Package*)PyCOMPSPack_new(&PyCOMPS_PackType, NULL, NULL);
    PyCOMPSPack_init(ret, NULL, NULL);
    COMPS_OBJECT_DESTROY(ret->c_obj);
    ret->c_obj = (COMPS_DocGroupPackage*)cobj;
    return (PyObject*)ret;
}

int pycomps_package_validate(COMPS_Object *obj) {
    COMPS_ValGenResult *result = comps_validate_execute(obj,
                                        COMPS_DocGroupPackage_ValidateRules);
    int ret = __pycomps_validate_process(result);
    COMPS_OBJECT_DESTROY(result);
    return ret;
}

PyObject* PyCOMPSPackage_validate(PyCOMPS_Package *pkg) {
    if (pycomps_package_validate((COMPS_Object*)pkg->c_obj))
        return NULL;
    else
        Py_RETURN_NONE;
}

COMPS_ObjList* comps_pkgs_union(COMPS_ObjList *pkgs1, COMPS_ObjList *pkgs2) {
    COMPS_ObjListIt *it;
    COMPS_Set *set;
    COMPS_DocGroupPackage *tmppkg;
    COMPS_ObjList *ret;
    void *data;
    //int index;

    ret = COMPS_OBJECT_CREATE(COMPS_ObjList, NULL);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, &comps_object_destroy_v,
                                    &__comps_docpackage_idcmp);
    for (it = pkgs1 ? pkgs1->first : NULL; it != NULL; it = it->next) {
        tmppkg = (COMPS_DocGroupPackage*)comps_object_copy(it->comps_obj);
        comps_set_add(set, tmppkg);
        comps_objlist_append(ret, (COMPS_Object*)tmppkg);
    }
    for (it = pkgs2 ? pkgs2->first : NULL; it != NULL; it = it->next) {
        if ((data = comps_set_data_at(set, it->comps_obj)) != NULL) {
            ((COMPS_DocGroupPackage*)data)->type =
                    ((COMPS_DocGroupPackage*)it->comps_obj)->type;
        } else {
            comps_objlist_append(ret, it->comps_obj);
        }
    }
    comps_set_destroy(&set);
    return ret;
}

PyObject* PyCOMPSPacks_union(PyObject *self, PyObject *other) {
    PyCOMPS_Sequence *res;
    COMPS_ObjList *res_list;

    if (other == NULL || Py_TYPE(other) != &PyCOMPS_PacksType) {
        PyErr_Format(PyExc_TypeError, "Not %s instance", Py_TYPE(self)->tp_name);
        return NULL;
    }

    res = (PyCOMPS_Sequence*) Py_TYPE(self)->tp_new(Py_TYPE(self), NULL, NULL);
    PyCOMPSPacks_init(res, NULL, NULL);
    res_list = comps_pkgs_union(((PyCOMPS_Sequence*)self)->list,
                                ((PyCOMPS_Sequence*)other)->list);

    COMPS_OBJECT_DESTROY(((PyCOMPS_Sequence*)res)->list);
    res->list = res_list;
    return (PyObject*)res;
}

PyCOMPS_ItemInfo PyCOMPS_PkgsInfo = {
    .itemtypes = (PyTypeObject*[]){&PyCOMPS_PackType},
    .in_convert_funcs = (PyCOMPS_in_itemconvert[])
                        {&comps_pkgs_in},
    .out_convert_func = &comps_pkgs_out,
    .item_types_len = 1,
    .props_offset = offsetof(COMPS_DocGroupPackage, name),
    .pre_checker = &pycomps_package_validate,
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

PyNumberMethods PyCOMPSPacks_Nums = {
    .nb_add = PyCOMPSPacks_union
};

PyTypeObject PyCOMPS_PacksType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.PackageList",   /*tp_name*/
    sizeof(PyCOMPS_Sequence), /*tp_basicsize*/
    0,                        /*tp_itemsize*/
    (destructor)PyCOMPSSeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    &PyCOMPSPacks_Nums,        /*tp_as_number*/
    0,//&PyCOMPSPack_sequence,       /*tp_as_sequence*/
    &PyCOMPSSeq_mapping_extra, /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    PyCOMPSSeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
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
    COMPS_OBJECT_DESTROY(self->c_obj);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyCOMPSPack_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Package *self;

   (void) args;
   (void) kwds;

    self = (PyCOMPS_Package*) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->c_obj = COMPS_OBJECT_CREATE(COMPS_DocGroupPackage, NULL);
    }
    return (PyObject*) self;
}

int PyCOMPSPack_init(PyCOMPS_Package *self, PyObject *args, PyObject *kwds)
{
    char *name = NULL, *requires=NULL;
    int type = COMPS_PACKAGE_UNKNOWN;

    #define _pkg_ ((PyCOMPS_Package*)self)->c_obj

    static char *kwlist[] = {"name", "type", "requires", NULL};

    if (!args && !kwds) {
        return 0;
    } else if (PyArg_ParseTupleAndKeywords(args, kwds, "|sis", kwlist,
                                          &name, &type, &requires)) {
        comps_docpackage_set_name(_pkg_, name, 1);
        comps_docpackage_set_requires(_pkg_, requires, 1);
        comps_docpackage_set_type_i(_pkg_, type, 0);
        return 0;
    } else {
        return -1;
    }
    #undef _pkg_
}

PyObject* PyCOMPSPack_strget_(PyCOMPS_Package *self, void *closure) {
    char *tmp = GET_FROM(self->c_obj, (size_t)closure);
    return PyUnicode_FromString(tmp);
}

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) DocGroupPkg_NameClosure = {
    .get_f = &comps_docpackage_get_name,
    .set_f = &comps_docpackage_set_name,
};

__COMPS_STRPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) DocGroupPkg_RequiresClosure = {
    .get_f = &comps_docpackage_get_requires,
    .set_f = &comps_docpackage_set_requires,
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) DocGroupPkg_TypeClosure = {
    .get_f = &comps_docpackage_get_type,
    .set_f = &comps_docpackage_set_type_i,
};

__COMPS_NUMPROP_GETSET_CLOSURE(COMPS_DocGroupPackage) DocGroupPkg_BAOClosure = {
    .get_f = &comps_docpackage_get_basearchonly,
    .set_f = &comps_docpackage_set_basearchonly,
};

PyDoc_STRVAR(PyCOMPS_package_type__doc__,
             "package type which could be one of following:\n\n"
             "* :py:const:`libcomps.PACKAGE_TYPE_DEFAULT`\n"
             "* :py:const:`libcomps.PACKAGE_TYPE_OPTIONAL`\n"
             "* :py:const:`libcomps.PACKAGE_TYPE_CONDITIONAL`\n"
             "* :py:const:`libcomps.PACKAGE_TYPE_MANDATORY`\n"
             "* :py:const:`libcomps.PACKAGE_TYPE_UNKNOWN`\n");

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
     PyCOMPS_package_type__doc__,
     (void*)&DocGroupPkg_TypeClosure},
    {"basearchonly",
     (getter)__PyCOMPS_get_boolattr, (setter)__PyCOMPS_set_boolattr,
     "Package basearchonly attribute",
     (void*)&DocGroupPkg_BAOClosure},
    {NULL}  /* Sentinel */
};

PyObject* PyCOMPSPack_str(PyObject *self) {
    char *x;
    COMPS_Object *name;
    PyObject *ret;
    name = comps_docpackage_get_name(((PyCOMPS_Package*)self)->c_obj);
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
    name = comps_docpackage_get_name(((PyCOMPS_Package*)self)->c_obj);
    x = comps_object_tostr(name);
    ret = PyUnicode_FromFormat("<libcomps.Package object '%s' at %p>", x, self);
    free(x);
    comps_object_destroy(name);
    return ret;
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
    ret = COMPS_OBJECT_CMP((COMPS_Object*)((PyCOMPS_Package*)self)->c_obj,
                           (COMPS_Object*)((PyCOMPS_Package*)other)->c_obj);
    if (op == Py_EQ) {
        if (!ret) Py_RETURN_FALSE;
    } else {
        if (ret) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

PyMemberDef PyCOMPSPack_members[] = {
    {NULL}};

PyDoc_STRVAR(PyCOMPS_package_validate__doc__,
             "Validate inner package structure. Raise exception on first"
             "occured error.\n\n"
             ":return: None if validation successful\n\n"
             ":raises ValueError: on first occured error");

PyMethodDef PyCOMPSPack_methods[] = {
    {"validate", (PyCFunction)PyCOMPSPackage_validate, METH_NOARGS,
    PyCOMPS_package_validate__doc__},
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_PackType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libcomps.Package",   /*tp_name*/
    sizeof(PyCOMPS_Package), /*tp_basicsize*/
    0,                        /*tp_itemsize*/
    (destructor)PyCOMPSPack_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSPack_cmp,           /*tp_compare*/
    PyCOMPSPack_repr,          /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    PyCOMPS_hash,              /*tp_hash */
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

