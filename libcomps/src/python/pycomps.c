#include "pycomps_23macros.h"
#include "pycomps.h"

#if PY_MAJOR_VERSION >= 3
    #define MODINIT_RET_NONE return NULL
    #define PY_OBJ_HEAD_INIT PyVarObject_HEAD_INIT(NULL, 0)
    #define IS_PY3K
#else
    #define MODINIT_RET_NONE return
    #define PY_OBJ_HEAD_INIT PyObject_HEAD_INIT(NULL)\
                             0,
#endif

inline PyCOMPS_GetSetClosure * get_closure(void * closure) {
    return ((PyCOMPS_GetSetClosure*)closure);
}

PyObject* PyCOMPS_toxml_f(PyObject *self, PyObject *other) {
    const char *errors = NULL;
    char *tmps;
    int i;
    COMPS_ListItem *it;
    PyObject *ret, *tmp;
    PyCOMPS *self_comps = (PyCOMPS*)self;

    if (__pycomps_arg_to_char(other, &tmps)) return NULL;
    if (!self_comps->comps->encoding)
       self_comps->comps->encoding = "UTF-8";
    comps2xml_f(self_comps->comps, tmps, 0);
    free(tmps);

    ret = PyList_New(self_comps->comps->log->logger_data->len);
    for (i = 0, it = self_comps->comps->log->logger_data->first;
         it != NULL; it = it->next, i++) {
        tmps = comps_log_entry_str(it->data);
        tmp = PyUnicode_DecodeUTF8(tmps, strlen(tmps), errors);
        PyList_SetItem(ret, i, tmp);
        free(tmps);
    }
    return ret;
}

PyObject* PyCOMPS_toxml_str(PyObject *self) {
    PyObject *ret;
    const char *errors = NULL;

    char *s = comps2xml_str(((PyCOMPS*)self)->comps);
    ret = PyUnicode_DecodeUTF8(s, strlen(s), errors);
    free(s);
    return ret;
}

void pycomps_clear(PyObject *self) {
    PyCOMPS *self_comps = (PyCOMPS*)self;
    if (self_comps->cats_pobj)
        Py_XDECREF(self_comps->cats_pobj);
    if (self_comps->groups_pobj)
        Py_XDECREF(self_comps->groups_pobj);
    if (self_comps->envs_pobj)
        Py_XDECREF(self_comps->envs_pobj);

    ctopy_citem_destroy(self_comps->groups_citem);
    ctopy_citem_destroy(self_comps->cats_citem);
    ctopy_citem_destroy(self_comps->envs_citem);
    Py_XDECREF(self_comps->enc);

    self_comps->cats_pobj = NULL;
    self_comps->groups_pobj = NULL;
    self_comps->envs_pobj = NULL;
}

PyObject* PyCOMPS_clear(PyObject *self) {
    PyCOMPS *self_comps = (PyCOMPS*)self_comps;

    pycomps_clear(self);

    self_comps->comps->lobjects->data_destructor = NULL;
    comps_dict_destroy(self_comps->comps->lobjects);
    comps_dict_destroy(self_comps->comps->dobjects);
    comps_dict_destroy(self_comps->comps->mdobjects);
    comps_log_destroy(self_comps->comps->log);

    self_comps->comps->lobjects = comps_dict_create(NULL, NULL,
                                                          &comps_list_destroy_v);
    self_comps->comps->dobjects = comps_dict_create(NULL,
                                                         &comps_dict_clone_v,
                                                         &comps_dict_destroy_v);
    self_comps->comps->mdobjects = comps_dict_create(NULL,
                                                        &comps_mdict_clone_v,
                                                        &comps_mdict_destroy_v);
    self_comps->comps->log = comps_log_create(0);


    self_comps->cats_citem = ctopy_citem_create(
                                comps_doc_categories(self_comps->comps),
                                &comps_list_destroy_v);
    self_comps->groups_citem = ctopy_citem_create(
                                comps_doc_groups(self_comps->comps),
                                &comps_list_destroy_v);
    self_comps->envs_citem = ctopy_citem_create(
                                comps_doc_environments(self_comps->comps),
                                &comps_list_destroy_v);
    self_comps->enc = PyUnicode_FromString("");
    Py_RETURN_NONE;
}

void pycomps_ctopy_comps_init(PyObject *self) {
    COMPS_ListItem *it, *it2;
    PyCOMPS *self_comps = (PyCOMPS*)self;

    it =(comps_doc_categories(self_comps->comps))?
        comps_doc_categories(self_comps->comps)->first:NULL;
    for (; it != NULL; it = it->next) {
        ((COMPS_DocCategory*)it->data)->reserved = comps_doccategory_extra_create();
        comps_cat_get_extra(it->data)->citem = ctopy_citem_create(it->data,
                                                          &pycomps_cat_destroy);
        comps_cat_get_extra(it->data)->group_ids_citem = ctopy_citem_create(
                                    ((COMPS_DocCategory*)it->data)->group_ids,
                                    &comps_list_destroy_v);
        comps_cat_get_extra(it->data)->name_by_lang_citem = ctopy_citem_create(
                                   ((COMPS_DocCategory*)it->data)->name_by_lang,
                                   &comps_dict_destroy_v);
        comps_cat_get_extra(it->data)->desc_by_lang_citem = ctopy_citem_create(
                                   ((COMPS_DocCategory*)it->data)->desc_by_lang,
                                   &comps_dict_destroy_v);
        it->data_destructor = &pycomps_cat_decref;
    }
    it =(comps_doc_groups(self_comps->comps))?
        comps_doc_groups(self_comps->comps)->first:NULL;
    for (; it != NULL; it = it->next) {
        ((COMPS_DocGroup*)it->data)->reserved = (void*)comps_docgroup_extra_create();
        comps_group_get_extra(it->data)->citem = ctopy_citem_create(it->data,
                                                            &pycomps_group_destroy);
        comps_group_get_extra(it->data)->packages_citem = ctopy_citem_create(
                                    ((COMPS_DocGroup*)it->data)->packages,
                                    &comps_list_destroy_v);
        comps_group_get_extra(it->data)->name_by_lang_citem = ctopy_citem_create(
                                   ((COMPS_DocGroup*)it->data)->name_by_lang,
                                   &comps_dict_destroy_v);
        comps_group_get_extra(it->data)->desc_by_lang_citem = ctopy_citem_create(
                                   ((COMPS_DocGroup*)it->data)->desc_by_lang,
                                   &comps_dict_destroy_v);
        it->data_destructor = &pycomps_group_decref;

        it2 =(((COMPS_DocGroup*)it->data)->packages)?
                ((COMPS_DocGroup*)it->data)->packages->first:NULL;
        for (; it2 != NULL; it2 = it2->next) {
            ((COMPS_DocGroupPackage*)it2->data)->reserved = ctopy_citem_create(
                                                           it2->data,
                                                           &pycomps_pkg_destroy);
            it2->data_destructor = &pycomps_pkg_decref;
        }
    }
    it = (comps_doc_environments(self_comps->comps))?
        comps_doc_environments(self_comps->comps)->first:NULL;
    for (; it != NULL; it = it->next) {
        ((COMPS_DocEnv*)it->data)->reserved = comps_docenv_extra_create();
        comps_env_get_extra(it->data)->citem = ctopy_citem_create(it->data,
                                                          &pycomps_env_destroy);
        comps_env_get_extra(it->data)->group_list_citem = ctopy_citem_create(
                                    ((COMPS_DocEnv*)it->data)->group_list,
                                    &comps_list_destroy_v);
        comps_env_get_extra(it->data)->option_list_citem = ctopy_citem_create(
                                    ((COMPS_DocEnv*)it->data)->option_list,
                                    &comps_list_destroy_v);
        comps_env_get_extra(it->data)->name_by_lang_citem = ctopy_citem_create(
                                   ((COMPS_DocEnv*)it->data)->name_by_lang,
                                   &comps_dict_destroy_v);
        comps_env_get_extra(it->data)->desc_by_lang_citem = ctopy_citem_create(
                                   ((COMPS_DocEnv*)it->data)->desc_by_lang,
                                   &comps_dict_destroy_v);
        it->data_destructor = &pycomps_env_decref;
    }

    self_comps->cats_citem = ctopy_citem_create(
                                comps_doc_categories(self_comps->comps),
                                &comps_list_destroy_v);
    self_comps->groups_citem = ctopy_citem_create(
                                comps_doc_groups(self_comps->comps),
                                &comps_list_destroy_v);
    self_comps->envs_citem = ctopy_citem_create(
                                comps_doc_environments(self_comps->comps),
                                &comps_list_destroy_v);
}

PyObject* PyCOMPS_fromxml_f(PyObject *self, PyObject *other) {
    FILE *f;
    COMPS_Parsed *parsed;
    char *fname, parsed_ret;
    PyCOMPS *self_comps = (PyCOMPS*)self;

    if (__pycomps_arg_to_char(other, &fname)) return NULL;

    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 0);
    f =  fopen(fname, "r");
    if (!f) {
        PyErr_Format(PyExc_IOError, "Cannot open %s for reading", fname);
        free(fname);
        comps_parse_parsed_destroy(parsed);
        return NULL;
    }
    parsed_ret = comps_parse_file(parsed, f);
    pycomps_clear(self);
    pycomps_doc_destroy((void*)self_comps->comps);
    self_comps->comps = parsed->comps_doc;
    comps_log_destroy(self_comps->comps->log);
    self_comps->comps->log = parsed->log;
    parsed->log = NULL;

    pycomps_ctopy_comps_init(self);
    self_comps->enc = PyBytes_FromString(parsed->comps_doc->encoding);

    parsed->comps_doc = NULL;
    comps_parse_parsed_destroy(parsed);

    return PyINT_FROM_LONG((long)parsed_ret);
}

PyObject* PyCOMPS_get_last_errors(PyObject *self, void *closure)
{
    PyObject *ret;
    COMPS_ListItem *it;
    char *tmps;
    PyObject *tmp;
    const char *errors = NULL;

    (void)closure;

    ret = PyList_New(0);
    for (it = ((PyCOMPS*)self)->comps->log->logger_data->first;
         it != NULL; it = it->next) {
        if (((COMPS_LoggerEntry*)it->data)->type == COMPS_LOG_ERROR) {
            tmps = comps_log_entry_str(it->data);
            tmp = PyUnicode_DecodeUTF8(tmps, strlen(tmps), errors);
            PyList_Append(ret, tmp);
            free(tmps);
        }
    }
    return ret;
}
PyObject* PyCOMPS_get_last_log(PyObject *self, void *closure)
{
    PyObject *ret;
    COMPS_ListItem *it;
    char *tmps;
    PyObject *tmp;
    const char *errors = NULL;

    (void)closure;

    ret = PyList_New(0);
    for (it = ((PyCOMPS*)self)->comps->log->logger_data->first;
         it != NULL; it = it->next) {
        tmps = comps_log_entry_str(it->data);
        tmp = PyUnicode_DecodeUTF8(tmps, strlen(tmps), errors);
        PyList_Append(ret, tmp);
        free(tmps);
        }
    return ret;
}

PyObject* PyCOMPS_fromxml_str(PyObject *self, PyObject *other) {
    char *tmps, parsed_ret;
    PyCOMPS *self_comps = (PyCOMPS*)self;

    if (__pycomps_arg_to_char(other, &tmps)) return NULL;

    COMPS_Parsed *parsed;
    parsed = comps_parse_parsed_create();
    comps_parse_parsed_init(parsed, "UTF-8", 0);
    parsed_ret = comps_parse_str(parsed, tmps);
    free(tmps);

    pycomps_clear(self);
    pycomps_doc_destroy((void*)self_comps->comps);
    self_comps->comps = parsed->comps_doc;
    comps_log_destroy(self_comps->comps->log);
    self_comps->comps->log = parsed->log;
    parsed->log = NULL;
    pycomps_ctopy_comps_init(self);
    self_comps->enc = PyBytes_FromString(parsed->comps_doc->encoding);
    parsed->comps_doc = NULL;
    comps_parse_parsed_destroy(parsed);

    return PyINT_FROM_LONG((long)parsed_ret);
}

PyObject* PyCOMPS_get_(PyCOMPS *self, void *closure) {
    PyObject *ret;
    COMPS_List *tmpl;

    if (!(PyObject*)GET_FROM(self, get_closure(closure)->pobj_offset)) {
        ret = PyCOMPSCtoPySeq_new(get_closure(closure)->type, NULL, NULL);
        Py_TYPE(ret)->tp_init(ret, NULL, NULL);
        if (!get_closure(closure)->get_f(self->comps)){
            tmpl = comps_list_create();
            comps_list_init(tmpl);
            get_closure(closure)->set_f(self->comps, tmpl);
            ((PyCOMPS_CtoPy_CItem*)
            GET_FROM(self, get_closure(closure)->citem_offset))->data = tmpl;
        }
        ctopy_citem_decref(((PyCOMPS_CtoPySeq*)ret)->citem);
        ((PyCOMPS_CtoPySeq*)ret)->citem = (PyCOMPS_CtoPy_CItem*)GET_FROM(self,
                            get_closure(closure)->citem_offset);
        ctopy_citem_incref(((PyCOMPS_CtoPySeq*)ret)->citem);
        SET_TO(self, get_closure(closure)->pobj_offset, ret)
    } else {
        ret = (PyObject*)GET_FROM(self, get_closure(closure)->pobj_offset);
    }
    Py_INCREF(ret);
    return ret;
}

int PyCOMPS_set_(PyCOMPS *self, PyObject *val, void *closure) {

    (void)closure;

    if (Py_TYPE(val) != get_closure(closure)->type) {
        PyErr_Format(PyExc_TypeError, "Not %s instance",
                     get_closure(closure)->type->tp_name);
        return -1;
    }
    if ((PyObject*)GET_FROM(self, get_closure(closure)->pobj_offset)){
        Py_DECREF((PyObject*)GET_FROM(self, get_closure(closure)->pobj_offset));
        SET_TO(self, get_closure(closure)->pobj_offset, NULL);
    }
    ((PyCOMPS_CtoPy_CItem*)
     GET_FROM(self, get_closure(closure)->citem_offset))->data = NULL;
    ctopy_citem_decref(GET_FROM(self, get_closure(closure)->citem_offset));
    SET_TO(self, get_closure(closure)->citem_offset,
                      ((PyCOMPS_CtoPySeq*)val)->citem);
    ctopy_citem_incref(((PyCOMPS_CtoPySeq*)val)->citem);
    get_closure(closure)->set_f(self->comps,
                                ((PyCOMPS_CtoPySeq*)val)->citem->data);
    SET_TO(self, get_closure(closure)->pobj_offset, val);
    Py_INCREF(val);
    return 0;
}

PyCOMPS_GetSetClosure envs_closure = {&PyCOMPS_EnvsType,
                                      offsetof(PyCOMPS, envs_citem),
                                      offsetof(PyCOMPS, envs_pobj),
                                      &comps_doc_environments,
                                      &comps_doc_set_envs};
PyCOMPS_GetSetClosure groups_closure = {&PyCOMPS_GroupsType,
                                        offsetof(PyCOMPS, groups_citem),
                                        offsetof(PyCOMPS, groups_pobj),
                                        &comps_doc_groups,
                                        &comps_doc_set_groups};
PyCOMPS_GetSetClosure cats_closure = {&PyCOMPS_CatsType,
                                      offsetof(PyCOMPS, cats_citem),
                                      offsetof(PyCOMPS, cats_pobj),
                                      &comps_doc_categories,
                                      &comps_doc_set_cats};
PyGetSetDef PyCOMPS_getset[] = {
    {"categories",
     (getter)PyCOMPS_get_, (setter)PyCOMPS_set_,
     "COMPS list of categories", &cats_closure},
    {"groups",
     (getter)PyCOMPS_get_, (setter)PyCOMPS_set_,
     "COMPS list of groups", &groups_closure},
    {"environments",
     (getter)PyCOMPS_get_, (setter)PyCOMPS_set_,
     "COMPS list of environments", &envs_closure},
    {NULL}  /* Sentinel */
};

static PyMemberDef PyCOMPS_members[] = {
    {NULL}};

static PyMethodDef PyCOMPS_methods[] = {
    {"xml_f", (PyCFunction)PyCOMPS_toxml_f, METH_O,
    "write XML represenstation of COMPS to file"},
    {"xml_str", (PyCFunction)PyCOMPS_toxml_str, METH_NOARGS,
    "return XML represenstation of COMPS as string"},
    {"fromxml_f", (PyCFunction)PyCOMPS_fromxml_f, METH_O,
    "Load COMPS from xml file"},
    {"fromxml_str", (PyCFunction)PyCOMPS_fromxml_str, METH_O,
    "Load COMPS from xml string"},
    {"clear", (PyCFunction)PyCOMPS_clear, METH_NOARGS,
    "Clear COMPS"},
    {"get_last_parse_errors", (PyCFunction)PyCOMPS_get_last_errors,
     METH_NOARGS,"return list of messages from log of last parse action."
                 "Containg errors only"},
    {"get_last_parse_log", (PyCFunction)PyCOMPS_get_last_log,
     METH_NOARGS,"return list of messages from log of last parse action."},
    {NULL}  /* Sentinel */
};

static void pycomps_doc_destroy(void *doc)
{
    ((COMPS_Doc*)doc)->lobjects->data_destructor = NULL;
    comps_dict_destroy(((COMPS_Doc*)doc)->lobjects);
    comps_dict_destroy(((COMPS_Doc*)doc)->dobjects);
    comps_dict_destroy(((COMPS_Doc*)doc)->mdobjects);
    comps_log_destroy(((COMPS_Doc*)doc)->log);
    free(doc);
}

static void PyCOMPS_dealloc(PyCOMPS* self)
{
    if (self->groups_pobj)
        Py_XDECREF(self->groups_pobj);
    if (self->cats_pobj)
        Py_XDECREF(self->cats_pobj);
    if (self->envs_pobj)
        Py_XDECREF(self->envs_pobj);

    ctopy_citem_decref(self->groups_citem);
    ctopy_citem_decref(self->cats_citem);
    ctopy_citem_decref(self->envs_citem);

    self->comps->lobjects->data_destructor = NULL;
    comps_dict_destroy(self->comps->lobjects);
    comps_dict_destroy(self->comps->dobjects);
    comps_dict_destroy(self->comps->mdobjects);
    comps_log_destroy(self->comps->log);
    free(self->comps);
    Py_XDECREF(self->enc);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* PyCOMPS_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS *self;
    char *enc = NULL; /* ignored here */
    PyObject *c_caps = NULL;
    (void)kwds;

    if (!PyArg_ParseTuple(args, "|sO!", &enc, &PyCapsule_Type, &c_caps))
        return NULL;

    self = (PyCOMPS*) type->tp_alloc(type, 0);
    if (self == NULL)
        return NULL;

    if (c_caps) {
        self->comps = PyCapsule_GetPointer(c_caps, NULL);
        assert(self->comps);
    } else
        self->comps = comps_doc_create(NULL);
    self->cats_pobj = NULL;
    self->envs_pobj = NULL;
    self->groups_pobj = NULL;
    if (!c_caps) {
        self->cats_citem = ctopy_citem_create(comps_doc_categories(self->comps),
                                              &comps_list_destroy_v);
        self->groups_citem = ctopy_citem_create(comps_doc_groups(self->comps),
                                                &comps_list_destroy_v);
        self->envs_citem = ctopy_citem_create(comps_doc_environments(self->comps),
                                              &comps_list_destroy_v);
    }
    self->enc = NULL;
    return (PyObject*) self;
}

static int PyCOMPS_init(PyCOMPS *self, PyObject *args, PyObject *kwds)
{
    char *enc = "UTF-8";
    PyObject *c_caps = NULL; /* ignored here */
    (void)kwds;

    if (!PyArg_ParseTuple(args, "|sO!", &enc, &PyCapsule_Type, &c_caps))
        return -1;
    self->enc = PyBytes_FromString(enc);
    return 0;
}

static PyObject* PyCOMPS_union(PyObject *self, PyObject *other) {
    PyCOMPS *res;

    if (Py_TYPE(other) != &PyCOMPS_Type) {
        PyErr_SetString(PyExc_TypeError, "Not COMPS instance");
        return NULL;
    }

    PyCOMPS *self_t = (PyCOMPS *)self;
    PyCOMPS *other_t = (PyCOMPS *)other;

    COMPS_Doc *un_comps = comps_doc_union(self_t->comps, other_t->comps);
    PyObject *c_caps = PyCapsule_New(un_comps, NULL, NULL);
    if (c_caps == NULL) {
        comps_doc_destroy(&un_comps);
        return NULL;
    }


    PyObject *arglist = Py_BuildValue("OO", self_t->enc, c_caps);
    res = (PyCOMPS*)PyObject_CallObject((PyObject*)&PyCOMPS_Type, arglist);
    Py_DECREF(arglist);
    Py_DECREF(c_caps);
    if (res == NULL)
        comps_doc_destroy(&un_comps);
    else
        pycomps_ctopy_comps_init((PyObject*)res);
    return (PyObject*)res;
}

PyObject* PyCOMPS_cmp(PyObject *self, PyObject *other, int op) {
    char res;

    CMP_OP_EQ_NE_CHECK(op)
    res = comps_doc_cmp(((PyCOMPS*)self)->comps, ((PyCOMPS*)other)->comps);
    if (op == Py_EQ && res) {
        Py_RETURN_TRUE;
    } else if (op == Py_NE && !res){
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

PyNumberMethods PyCOMPS_Nums = {
    .nb_add = PyCOMPS_union
};


PyTypeObject PyCOMPS_Type = {
    PY_OBJ_HEAD_INIT
    "_libpycomps.Comps",             /*tp_name*/
    sizeof(PyCOMPS), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCOMPS_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    &PyCOMPS_Nums,              /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    PYCOMPS_DOCU,             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    PyCOMPS_cmp,               /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyCOMPS_methods,           /* tp_methods */
    PyCOMPS_members,           /* tp_members */
    PyCOMPS_getset,            /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPS_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyCOMPS_new,                /* tp_new */};

#if PY_MAJOR_VERSION >= 3
    static struct PyModuleDef moduledef = {
            PyModuleDef_HEAD_INIT,
            "_libpycomps",
            "libcomps module",
            -1,
            NULL, //myextension_methods,
            NULL,
            NULL, //myextension_traverse,
            NULL, //myextension_clear,
            NULL
    };
#endif

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


PyMODINIT_FUNC
PYINIT_FUNC(void) {
    PyObject *m;
    PyCOMPS_GroupType.tp_new = PyCOMPSGroup_new;
    PyCOMPS_Type.tp_new = PyCOMPS_new;
    if (PyType_Ready(&PyCOMPS_Type) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_CatType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_CatsType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_IDsType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_EnvsType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_EnvType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_GroupType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_GroupsType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_PacksType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_PackType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_DictType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_SeqIterType) < 0 ) {
        MODINIT_RET_NONE;
    }
    if (PyType_Ready(&PyCOMPS_CtoPySeqIterType) < 0 ) {
        MODINIT_RET_NONE;
    }
    #if PY_MAJOR_VERSION >= 3
        m = PyModule_Create(&moduledef);
    #else
        m = Py_InitModule("_libpycomps", NULL);
    #endif
    Py_INCREF(&PyCOMPS_Type);
    PyModule_AddObject(m, "Comps", (PyObject*) &PyCOMPS_Type);
    Py_INCREF(&PyCOMPS_CatsType);
    PyModule_AddObject(m, "CategoryList", (PyObject*) &PyCOMPS_CatsType);
    Py_INCREF(&PyCOMPS_CatType);
    PyModule_AddObject(m, "Category", (PyObject*) &PyCOMPS_CatType);
    Py_INCREF(&PyCOMPS_IDsType);
    PyModule_AddObject(m, "IdList", (PyObject*) &PyCOMPS_IDsType);
    Py_INCREF(&PyCOMPS_GroupType);
    PyModule_AddObject(m, "Group", (PyObject*) &PyCOMPS_GroupType);
    Py_INCREF(&PyCOMPS_GroupsType);
    PyModule_AddObject(m, "GroupList", (PyObject*) &PyCOMPS_GroupsType);
    Py_INCREF(&PyCOMPS_PacksType);
    PyModule_AddObject(m, "PackageList", (PyObject*) &PyCOMPS_PacksType);
    Py_INCREF(&PyCOMPS_PackType);
    PyModule_AddObject(m, "Package", (PyObject*) &PyCOMPS_PackType);
    Py_INCREF(&PyCOMPS_EnvType);
    PyModule_AddObject(m, "Environment", (PyObject*) &PyCOMPS_EnvType);
    Py_INCREF(&PyCOMPS_EnvsType);
    PyModule_AddObject(m, "EnvList", (PyObject*) &PyCOMPS_EnvsType);
    Py_INCREF(&PyCOMPS_DictType);
    PyModule_AddObject(m, "Dict", (PyObject*) &PyCOMPS_DictType);

    PyModule_AddIntConstant(m, "PACKAGE_TYPE_DEFAULT", COMPS_PACKAGE_DEFAULT);
    PyModule_AddIntConstant(m, "PACKAGE_TYPE_OPTIONAL", COMPS_PACKAGE_OPTIONAL);
    PyModule_AddIntConstant(m, "PACKAGE_TYPE_CONDITIONAL", COMPS_PACKAGE_CONDITIONAL);
    PyModule_AddIntConstant(m, "PACKAGE_TYPE_MANDATORY", COMPS_PACKAGE_MANDATORY);
    PyModule_AddIntConstant(m, "PACKAGE_TYPE_UNKNOWN", COMPS_PACKAGE_UNKNOWN);
    #if PY_MAJOR_VERSION >= 3
        return m;
    #endif
}

