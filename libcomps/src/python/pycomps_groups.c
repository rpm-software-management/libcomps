#include "pycomps_groups.h"

char __compsgroup_id_cmp(void *c1, void *c2) {
    return (strcmp(((COMPS_DocGroup*)c1)->id,
                  ((COMPS_DocGroup*)c2)->id) == 0);
}

inline COMPS_DocGroup* pycomps_group_get(PyObject *pygroup) {
    return (COMPS_DocGroup*)((PyCOMPS_Group*)pygroup)->citem->data;
}
inline COMPS_DocGroup** pycomps_group_getp(PyObject *pygroup) {
    return (COMPS_DocGroup**)&((PyCOMPS_Group*)pygroup)->citem->data;
}
inline COMPS_DocGroupExtra* pycomps_group_get_extra(PyObject *pygroup) {
    return (COMPS_DocGroupExtra*)
           ((COMPS_DocGroup*)((PyCOMPS_Group*)pygroup)->citem->data)
           ->reserved;
}
inline COMPS_DocGroupExtra* comps_group_get_extra(void* group) {
    return (COMPS_DocGroupExtra*)((COMPS_DocGroup*)group)->reserved;
}
inline void pycomps_group_incref(PyObject * pygroup) {
    ctopy_citem_incref(pycomps_group_get_extra(pygroup)->name_by_lang_citem);
    ctopy_citem_incref(pycomps_group_get_extra(pygroup)->desc_by_lang_citem);
    ctopy_citem_incref(pycomps_group_get_extra(pygroup)->packages_citem);
    ctopy_citem_incref(pycomps_group_get_extra(pygroup)->citem);
}

inline void comps_group_incref(void * group) {
    ctopy_citem_incref(comps_group_get_extra(group)->name_by_lang_citem);
    ctopy_citem_incref(comps_group_get_extra(group)->desc_by_lang_citem);
    ctopy_citem_incref(comps_group_get_extra(group)->packages_citem);
    ctopy_citem_incref(comps_group_get_extra(group)->citem);
}

inline void pycomps_group_decref(void * group) {
    ctopy_citem_decref(comps_group_get_extra(group)->name_by_lang_citem);
    ctopy_citem_decref(comps_group_get_extra(group)->desc_by_lang_citem);
    ctopy_citem_decref(comps_group_get_extra(group)->packages_citem);
    ctopy_citem_decref(comps_group_get_extra(group)->citem);
}

COMPS_DocGroupExtra * comps_docgroup_extra_create() {
    COMPS_DocGroupExtra *ret;
    ret = malloc(sizeof(*ret));
    return ret;
}

char comps_group_cmp(void *g1, void *g2) {
    COMPS_ListItem *it, *it2;
    if (__pycomps_strcmp(((COMPS_DocGroup*)g1)->id,
                         ((COMPS_DocGroup*)g2)->id)) return 0;
    if (__pycomps_strcmp(((COMPS_DocGroup*)g1)->name,
                         ((COMPS_DocGroup*)g2)->name)) return 0;
    if (__pycomps_strcmp(((COMPS_DocGroup*)g1)->desc,
                         ((COMPS_DocGroup*)g2)->desc)) return 0;
    it = (((COMPS_DocGroup*)g1)->packages)
         ?((COMPS_DocGroup*)g1)->packages->first: NULL;
    it2 = (((COMPS_DocGroup*)g2)->packages)
         ?((COMPS_DocGroup*)g2)->packages->first: NULL;
    
    for (; it != NULL && it2 != NULL; it = it->next, it2 = it2->next) {
        if (__pycomps_strcmp(((COMPS_DocGroupPackage*)it->data)->name,
                             ((COMPS_DocGroupPackage*)it->data)->name))
            return 0;
        if (((COMPS_DocGroupPackage*)it->data)->type !=
            ((COMPS_DocGroupPackage*)it2->data)->type)
            return 0;
    }
    if (it != NULL || it2 != NULL)
        return 0;
    return 1;
}

PyObject* PyCOMPSGroup_convert(void *g) {
    PyObject *ret;
    ret = PyCOMPSGroup_new(&PyCOMPS_GroupType, NULL, NULL);
    PyCOMPSGroup_init((PyCOMPS_Group*)ret, NULL, NULL);
    ctopy_citem_decref(pycomps_group_get_extra(ret)->packages_citem);
    ctopy_citem_decref(pycomps_group_get_extra(ret)->name_by_lang_citem);
    ctopy_citem_decref(pycomps_group_get_extra(ret)->desc_by_lang_citem);
    ctopy_citem_decref(((PyCOMPS_Group*)ret)->citem);

    ((PyCOMPS_Group*)ret)->citem = comps_group_get_extra(g)->citem;
    pycomps_group_get_extra(ret)->packages_citem =
                                     comps_group_get_extra(g)->packages_citem;
    pycomps_group_get_extra(ret)->name_by_lang_citem =
                                     comps_group_get_extra(g)->name_by_lang_citem;
    pycomps_group_get_extra(ret)->desc_by_lang_citem =
                                     comps_group_get_extra(g)->desc_by_lang_citem;

    ctopy_citem_incref(((PyCOMPS_Group*)ret)->citem);
    ctopy_citem_incref(pycomps_group_get_extra(ret)->packages_citem);
    ctopy_citem_incref(pycomps_group_get_extra(ret)->name_by_lang_citem);
    ctopy_citem_incref(pycomps_group_get_extra(ret)->desc_by_lang_citem);
    return ret;
}

PyObject* PyCOMPSGroup_union(PyObject *self, PyObject *other) {
    COMPS_DocGroup *g;
    PyObject *res;
    COMPS_ListItem *it;

    if (other->ob_type != &PyCOMPS_GroupType) {
        PyErr_SetString(PyExc_TypeError, "Not Group instance");
        return NULL;
    }
    if (pycomps_group_get(self)->packages == NULL) {
        pycomps_group_get(self)->packages = comps_list_create();
        comps_list_init(pycomps_group_get(self)->packages);
        pycomps_group_get_extra(self)->packages_citem->data =
                                              pycomps_group_get(self)->packages;
    }
    if (pycomps_group_get(other)->packages == NULL) {
        pycomps_group_get(other)->packages = comps_list_create();
        comps_list_init(pycomps_group_get(other)->packages);
        pycomps_group_get_extra(other)->packages_citem->data =
                                             pycomps_group_get(other)->packages;
    }
    g = comps_docgroup_union(pycomps_group_get(self),
                             pycomps_group_get(other));
    for (it = g->packages->first; it != NULL; it = it->next) {
        ((COMPS_DocGroupPackage*)it->data)->reserved = ctopy_citem_create(
                                                        it->data,
                                                        &pycomps_pkg_destroy);
        it->data_destructor = &pycomps_pkg_decref;
    }
    res = PyCOMPSGroup_new(&PyCOMPS_GroupType, NULL, NULL);
    PyCOMPSGroup_init((PyCOMPS_Group*)res, NULL, NULL);
    g->reserved = pycomps_group_get_extra(res);
    comps_docgroup_destroy(pycomps_group_get(res));
    ((PyCOMPS_Group*)res)->citem->data = g;
    pycomps_group_get_extra(res)->packages_citem->data = g->packages;
    pycomps_group_get_extra(res)->name_by_lang_citem->data = g->name_by_lang;
    pycomps_group_get_extra(res)->desc_by_lang_citem->data = g->desc_by_lang;
    return res;
}

void PyCOMPSGroup_dealloc(PyObject *self)
{
    ctopy_citem_destroy(pycomps_group_get_extra(self)->name_by_lang_citem);
    ctopy_citem_destroy(pycomps_group_get_extra(self)->desc_by_lang_citem);
    ctopy_citem_destroy(pycomps_group_get_extra(self)->packages_citem);
    ctopy_citem_destroy(((PyCOMPS_Group*)self)->citem);
    if (((PyCOMPS_Group*)self)->packages_pobj)
        Py_XDECREF(((PyCOMPS_Group*)self)->packages_pobj);
    if (((PyCOMPS_Group*)self)->name_by_lang_pobj)
        Py_XDECREF(((PyCOMPS_Group*)self)->name_by_lang_pobj);
    if (((PyCOMPS_Group*)self)->desc_by_lang_pobj)
        Py_XDECREF(((PyCOMPS_Group*)self)->desc_by_lang_pobj);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

void pycomps_group_destroy(void * group) {
    free(((COMPS_DocGroup*)group)->id);
    free(((COMPS_DocGroup*)group)->name);
    free(((COMPS_DocGroup*)group)->desc);
    free(((COMPS_DocGroup*)group)->lang_only);
    free(((COMPS_DocGroup*)group)->reserved);
    free(group);
}

PyObject* PyCOMPSGroup_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Group *self;
    COMPS_DocGroup *g;

    (void)args;
    (void)kwds;
    self = (PyCOMPS_Group*) type->tp_alloc(type, 0);
    if (self != NULL) {

        g = comps_docgroup_create();
        g->reserved = (void*)comps_docgroup_extra_create();
        ((COMPS_DocGroupExtra*)(g->reserved))->citem =\
                                  ctopy_citem_create(g, &pycomps_group_destroy);
        self->citem = ((COMPS_DocGroupExtra*)g->reserved)->citem;

        ((COMPS_DocGroupExtra*)(g->reserved))->packages_citem =
                                    ctopy_citem_create(g->packages,
                                                       &comps_list_destroy_v);
        self->packages_pobj = NULL;

        ((COMPS_DocGroupExtra*)(g->reserved))->name_by_lang_citem =
                                    ctopy_citem_create(g->name_by_lang,
                                                       &comps_dict_destroy_v);
        self->name_by_lang_pobj = NULL;

        ((COMPS_DocGroupExtra*)(g->reserved))->desc_by_lang_citem =
                                    ctopy_citem_create(g->desc_by_lang,
                                                       &comps_dict_destroy_v);
        self->desc_by_lang_pobj = NULL;
    }
    return (PyObject*) self;
}

int PyCOMPSGroup_init(PyCOMPS_Group *self, PyObject *args, PyObject *kwds)
{
    char *name = NULL;
    char *id = NULL;
    char *desc = NULL;
    char *lang = NULL;
    unsigned def=0, uservis=0;
    
    (void) kwds;

    name = NULL;
    id = NULL;
    desc = NULL;
    def = 0;
    uservis = 0;
    lang = NULL;
    if (args && PyArg_ParseTuple(args, "|sssiis", &id,
                                          &name,
                                          &desc,
                                          &def,
                                          &uservis,
                                          &lang)) {
        comps_docgroup_set_id(pycomps_group_get((PyObject*)self), id, 1);
        comps_docgroup_set_name(pycomps_group_get((PyObject*)self), name, 1);
        comps_docgroup_set_desc(pycomps_group_get((PyObject*)self), desc, 1);
        comps_docgroup_set_default(pycomps_group_get((PyObject*)self), def);
        comps_docgroup_set_uservisible(pycomps_group_get((PyObject*)self), uservis);
        comps_docgroup_set_langonly(pycomps_group_get((PyObject*)self), lang, 1);
        return 0;
    } else {
        return -1;
    }
}

PyObject* comps_group_str(void * group) {
    PyObject *ret, *tmp, *tmp2, *emptytmp;
    const char *id, *name, *desc, *lang;
    char *empty;
    COMPS_ListItem *it;
    unsigned def, uservis;

    emptytmp = Py_TYPE(Py_None)->tp_str(Py_None);
    if (PyUnicode_Check(emptytmp)) {
        __pycomps_PyUnicode_AsString(emptytmp, &empty);
    } else {
        empty = PyBytes_AsString(emptytmp);
    }

    id = (((COMPS_DocGroup*)group)->id)?((COMPS_DocGroup*)group)->id:empty;
    name = (((COMPS_DocGroup*)group)->name)?((COMPS_DocGroup*)group)->name:empty;
    desc = (((COMPS_DocGroup*)group)->desc)?((COMPS_DocGroup*)group)->desc:empty;
    lang = (((COMPS_DocGroup*)group)->lang_only)
           ?((COMPS_DocGroup*)group)->lang_only:empty;
    uservis = ((COMPS_DocGroup*)group)->uservisible;
    def = ((COMPS_DocGroup*)group)->def;

    ret = PyUnicode_FromFormat("<COMPS_Group: id='%s', name='%s', description='%s',"
                              "default='%d', uservisible='%d', lang_only='%s', "
                              "name_by_lang=",
                              /*, description_by_lang=%U, %U>",*/
                              id, name, desc, def, uservis, lang);
    if (PyUnicode_Check(emptytmp)) {
        free(empty);
    }
    Py_XDECREF(emptytmp);

    tmp2 = comps_lang_str(((COMPS_DocGroup*)group)->name_by_lang);
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp2);
    Py_XDECREF(ret);
    ret = tmp;

    tmp2 = PyUnicode_FromString(", desc_by_lang=");
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp2);
    Py_XDECREF(ret);
    ret = tmp;

    tmp2 = comps_lang_str(((COMPS_DocGroup*)group)->desc_by_lang);
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_XDECREF(tmp2);
    Py_XDECREF(ret);
    ret = tmp;

    tmp2 = PyUnicode_FromString(", packages=[");
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_DECREF(ret);
    Py_DECREF(tmp2);
    ret = tmp;
    it = (((COMPS_DocGroup*)group)->packages)?
         ((COMPS_DocGroup*)group)->packages->first:NULL;
    for (; it != NULL && it != ((COMPS_DocGroup*)group)->packages->last;
         it = it->next){
        tmp2 =PyUnicode_FromFormat("<COMPS_Package name='%s' type=%s>",
                               ((COMPS_DocGroupPackage*)it->data)->name,
          comps_docpackage_type_str(((COMPS_DocGroupPackage*)it->data)->type));
        tmp = PyUnicode_Concat(ret,tmp2);
        Py_DECREF(ret);
        Py_DECREF(tmp2);
        ret = tmp;
    }
    if (it) {
        tmp2 = PyUnicode_FromFormat("<COMPS_Package name='%s' type=%s>]",
                               ((COMPS_DocGroupPackage*)it->data)->name,
          comps_docpackage_type_str(((COMPS_DocGroupPackage*)it->data)->type));
    } else {
        tmp2 = PyUnicode_FromString("]");
    }
    tmp = PyUnicode_Concat(ret,tmp2);
    Py_DECREF(tmp2);
    Py_DECREF(ret);
    ret = tmp;

    tmp2 = PyUnicode_FromString(">");
    tmp = PyUnicode_Concat(ret, tmp2);
    Py_DECREF(tmp2);
    Py_XDECREF(ret);
    ret = tmp;
    return ret;
}

PyObject* PyCOMPSGroup_str(PyObject *self) {
    return comps_group_str(pycomps_group_get(self));
}

inline void comps_group_print(FILE *f, void *g) {
    COMPS_ListItem *it;
    COMPS_HSList *pairlist;
    COMPS_HSListItem *hsit;

    fprintf(f, "<COMPS_Group: id='%s', name='%s', description='%s',  "
            "default='%d', uservisible='%d', lang_only='%s', ",
            ((COMPS_DocGroup*)g)->id,
            ((COMPS_DocGroup*)g)->name,
            ((COMPS_DocGroup*)g)->desc,
            ((COMPS_DocGroup*)g)->def,
            ((COMPS_DocGroup*)g)->uservisible,
            ((COMPS_DocGroup*)g)->lang_only);
    fprintf(f, "name_by_lang={");
    pairlist = comps_rtree_pairs(((COMPS_DocGroup*)g)->name_by_lang);
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
    pairlist = comps_rtree_pairs(((COMPS_DocGroup*)g)->desc_by_lang);
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

    fprintf(f, ", packages=[");
    if (((COMPS_DocGroup*)g)->packages) {
        for (it = (((COMPS_DocGroup*)g)->packages)->first;
             it != ((COMPS_DocGroup*)g)->packages->last; it = it->next){
            comps_pkg_print(f, it->data);
            fprintf(f,", ");
        }
        if (it)
            comps_pkg_print(f, it->data);
    }
    fprintf(f, "]>");
}

int PyCOMPSGroup_print(PyObject *self, FILE *f, int flags) {
    (void) flags;
    comps_group_print(f, pycomps_group_get(self));
    return 0;
}

PyObject* PyCOMPSGroup_cmp(PyObject *self, PyObject *other, int op) {
    COMPS_ListItem *it;
    COMPS_Set *set;
    PyObject *ret1;

    // Only eq and neq operators allowed
    CMP_OP_EQ_NE_CHECK(op)

    if (other == NULL || ( Py_TYPE(other) != Py_TYPE(self)
                           && other != Py_None)) {
        PyErr_Format(PyExc_TypeError, "Not %s instance",
                                      Py_TYPE(self)->tp_name);
        return NULL;
    }
    CMP_NONE_CHECK(op, self, other)

    ret1 = (op == Py_EQ)?Py_False:Py_True;

    if (__pycomps_strcmp(pycomps_group_get(self)->id,
                pycomps_group_get(other)->id) != 0) {
        Py_INCREF(ret1);
        return ret1;
    }
    if (__pycomps_strcmp(pycomps_group_get(self)->name,
                pycomps_group_get(other)->name) != 0) {
        Py_INCREF(ret1);
        return ret1;
    }
    if (__pycomps_strcmp(pycomps_group_get(self)->desc,
                pycomps_group_get(self)->desc) != 0) {
        Py_INCREF(ret1);
        return ret1;
    }
    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &comps_docpackage_cmp);
    it = (pycomps_group_get(self)->packages)?
         pycomps_group_get(self)->packages->first: NULL;
    for (; it!= NULL; it = it->next) {
        comps_set_add(set, it->data);
    }
    it = (pycomps_group_get(other)->packages)?
         pycomps_group_get(other)->packages->first: NULL;
    for (; it!= NULL; it = it->next) {
        if (!comps_set_in(set, it->data)) {
            if (op == Py_EQ) {
                comps_set_destroy(&set);
                Py_RETURN_FALSE;
            }
        } else {
            if (op == Py_NE) {
                comps_set_destroy(&set);
                Py_RETURN_TRUE;
            }
        }
    }
    comps_set_destroy(&set);
    Py_RETURN_TRUE;
}

PyObject* PyCOMPSGroup_get_packages(PyCOMPS_Group *self, void *closure) {
    (void) closure;
    PyObject *ret;
    if (!self->packages_pobj) {
        ret = PyCOMPSCtoPySeq_new(&PyCOMPS_PacksType, NULL, NULL);
        PyCOMPSPacks_init((PyCOMPS_CtoPySeq*)ret, NULL, NULL);
        if (pycomps_group_get((PyObject*)self)->packages == NULL) {
            pycomps_group_get((PyObject*)self)->packages = comps_list_create();
            comps_list_init(pycomps_group_get((PyObject*)self)->packages);
            pycomps_group_get_extra((PyObject*)self)->packages_citem->data =
                                   pycomps_group_get((PyObject*)self)->packages;
        }
        ctopy_citem_destroy(((PyCOMPS_CtoPySeq*)ret)->citem);
        ((PyCOMPS_CtoPySeq*)ret)->citem = pycomps_group_get_extra(
                                               (PyObject*)self)->packages_citem;
        self->packages_pobj = ret;
        ctopy_citem_incref(((PyCOMPS_CtoPySeq*)ret)->citem);
        Py_INCREF(ret);
    } else {
        ret = self->packages_pobj;
        Py_INCREF(ret);
    }
    return  ret;
}

int PyCOMPSGroup_set_packages(PyCOMPS_Group *self,
                                  PyObject *value, void *closure) {
    (void) closure;

    if (value->ob_type != &PyCOMPS_PacksType) {
        PyErr_SetString(PyExc_TypeError, "Not COMPS_Packages instance");
        return -1;
    }
    ctopy_citem_decref(pycomps_group_get_extra((PyObject*)self)->packages_citem);
    if (self->packages_pobj) {
        Py_XDECREF(self->packages_pobj);
    }
    pycomps_group_get_extra((PyObject*)self)->packages_citem =
                                              ((PyCOMPS_CtoPySeq*)value)->citem;
    ctopy_citem_incref(((PyCOMPS_CtoPySeq*)value)->citem);
    pycomps_group_get((PyObject*)self)->packages =
        (COMPS_List*)((PyCOMPS_CtoPySeq*)value)->citem->data;
    self->packages_pobj = value;
    Py_INCREF(value);
    
    return 0;
}

inline PyObject* PyCOMPSGroup_get_name_by_lang(PyCOMPS_Group *self, void *closure) {
    (void) closure;
    return pycomps_lang_get_dict(
                   pycomps_group_get_extra((PyObject*)self)->name_by_lang_citem,
                   &self->name_by_lang_pobj);
}

inline int PyCOMPSGroup_set_name_by_lang(PyCOMPS_Group *self,
                                  PyObject *value, void *closure) {
    (void) closure;
    return pycomps_lang_set_dict(
                  &pycomps_group_get_extra((PyObject*)self)->name_by_lang_citem,
                  &self->name_by_lang_pobj,
                  value,
                  (void**)&pycomps_group_get((PyObject*)self)->name_by_lang);
}

inline PyObject* PyCOMPSGroup_get_desc_by_lang(PyCOMPS_Group *self, void *closure) {
    (void) closure;
    return pycomps_lang_get_dict(
                   pycomps_group_get_extra((PyObject*)self)->desc_by_lang_citem,
                   &self->desc_by_lang_pobj);
}

inline int PyCOMPSGroup_set_desc_by_lang(PyCOMPS_Group *self,
                                  PyObject *value, void *closure) {
    (void) closure;
    return pycomps_lang_set_dict(
                  &pycomps_group_get_extra((PyObject*)self)->desc_by_lang_citem,
                  &self->desc_by_lang_pobj,
                  value,
                  (void**)&pycomps_group_get((PyObject*)self)->desc_by_lang);
}

int pycomps_group_strattr_setter(PyObject *self, PyObject *val, void *closure) {
    char *tmp;
    if (__pycomps_stringable_to_char(val, &tmp) < 0) {
        return -1;
    }
    __comps_doc_char_setter((void**)&GET_FROM(pycomps_group_get(self), (size_t)closure),
                                      tmp, 0);
    return 0;
}

PyObject* pycomps_group_strattr_getter(PyObject *self, void *closure) {

    char *tmp;
    tmp = GET_FROM(pycomps_group_get(self), (size_t)closure);
    if (tmp)
        return PyUnicode_FromString(tmp);
    else
        Py_RETURN_NONE;
}

PyMemberDef PyCOMPSGroup_members[] = {
    {NULL}};

PyMethodDef PyCOMPSGroup_methods[] = {
    {NULL}  /* Sentinel */
};

PyGetSetDef PyCOMPSGroup_getset[] = {
    {"id",
     (getter)pycomps_group_strattr_getter, (setter)pycomps_group_strattr_setter,
     "Group id",
     (void*)offsetof(COMPS_DocGroup, id)},
    {"name",
     (getter)pycomps_group_strattr_getter, (setter)pycomps_group_strattr_setter,
     "Group name",
     (void*)offsetof(COMPS_DocGroup, name)},
    {"desc",
     (getter)pycomps_group_strattr_getter, (setter)pycomps_group_strattr_setter,
     "Group description",
     (void*)offsetof(COMPS_DocGroup, desc)},
    {"packages",
    (getter)PyCOMPSGroup_get_packages, (setter)PyCOMPSGroup_set_packages,
     "Group packages",
     NULL},
    {"name_by_lang",
    (getter)PyCOMPSGroup_get_name_by_lang, (setter)PyCOMPSGroup_set_name_by_lang,
     "Group name locales", NULL},
    {"desc_by_lang",
    (getter)PyCOMPSGroup_get_desc_by_lang, (setter)PyCOMPSGroup_set_desc_by_lang,
     "Group description locales", NULL},
    {NULL}  /* Sentinel */
};

PyNumberMethods PyCOMPSGroup_Nums = {
    .nb_add = PyCOMPSGroup_union
};

PyCOMPS_CtoPySeqItemMan PyCOMPSGroup_ItemMan = {
    .item_type = &PyCOMPS_GroupType,
    .ctopy_convert = PyCOMPSGroup_convert,
    .data_decref = pycomps_group_decref,
    .data_incref = comps_group_incref,
    .data_cmp = comps_group_cmp,
    .fprint_f = comps_group_print,
    .str_f = comps_group_str
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
    0,                         /*tp_repr*/
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


int PyCOMPSGroups_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds)
{
    (void) args;
    (void) kwds;
    self->item_man = &PyCOMPSGroup_ItemMan;
    return 0;
}

COMPS_List* comps_groups_union(COMPS_List *groups1, COMPS_List *groups2) {
    COMPS_HSListItem *hsit;
    COMPS_Set *set;
    COMPS_List *res;
    COMPS_DocGroup *tmpgroup, *tmpgroup2, *unigroup;
    COMPS_ListItem *it, *newit;

    res = comps_list_create();
    comps_list_init(res);

    set = comps_set_create();
    comps_set_init(set, NULL, NULL, NULL, &__compsgroup_id_cmp);

    for (it = groups1->first; it != NULL; it = it->next) {
        if (((COMPS_DocGroup*)it->data)->packages == NULL) {
            ((COMPS_DocGroup*)it->data)->packages = comps_list_create();
            comps_list_init(((COMPS_DocGroup*)it->data)->packages);
            comps_group_get_extra(it->data)->packages_citem->data =
                                      ((COMPS_DocGroup*)it->data)->packages;
        }
        comps_group_incref(it->data);
        comps_set_add(set, it->data);
    }
    for (it = groups2->first; it != NULL; it = it->next) {
        tmpgroup = it->data;
        if (tmpgroup->packages == NULL) {
            tmpgroup->packages = comps_list_create();
            comps_list_init(tmpgroup->packages);
            comps_group_get_extra(tmpgroup)->packages_citem->data =
                                                         tmpgroup->packages;
        }
        if (comps_set_in(set, tmpgroup)) {
            tmpgroup2 = (COMPS_DocGroup*)comps_set_data_at(set, tmpgroup);
            unigroup = comps_docgroup_union(tmpgroup, tmpgroup2);
            unigroup->reserved = comps_docgroup_extra_create();
            ((COMPS_DocGroupExtra*)(unigroup->reserved))->citem =
                             ctopy_citem_create(unigroup, &pycomps_group_destroy);
            ((COMPS_DocGroupExtra*)(unigroup->reserved))->packages_citem =
                                        ctopy_citem_create(unigroup->packages,
                                                           &comps_list_destroy_v);
            ((COMPS_DocGroupExtra*)(unigroup->reserved))->name_by_lang_citem =
                                        ctopy_citem_create(unigroup->name_by_lang,
                                                           &comps_dict_destroy_v);
            ((COMPS_DocGroupExtra*)(unigroup->reserved))->desc_by_lang_citem =
                                        ctopy_citem_create(unigroup->desc_by_lang,
                                                           &comps_dict_destroy_v);
            pycomps_group_decref(tmpgroup2);
            comps_set_remove(set, tmpgroup2);
            comps_set_add(set, unigroup);
        } else {
            comps_group_incref((void*)tmpgroup);
            comps_set_add(set, tmpgroup);
        }
    }
    for (hsit = set->data->first; hsit != NULL; hsit = hsit->next) {
        newit = comps_list_item_create(hsit->data, NULL, &pycomps_group_decref);
        comps_list_append(res, newit);
    }
    comps_set_destroy(&set);
    return res;
}

PyObject* PyCOMPSGroups_union(PyObject *self, PyObject *other) {
    PyCOMPS_CtoPySeq *res;
    COMPS_List *res_list;

    if (other == NULL || Py_TYPE(other) != &PyCOMPS_GroupsType) {
        PyErr_SetString(PyExc_TypeError, "Not GroupList instance");
        return NULL;
    }

    res = (PyCOMPS_CtoPySeq*)Py_TYPE(self)->tp_new(Py_TYPE(self), NULL, NULL);
    PyCOMPSGroups_init(res, NULL, NULL);

    res_list = comps_groups_union(((PyCOMPS_CtoPySeq*)self)->citem->data,
                                  ((PyCOMPS_CtoPySeq*)other)->citem->data);
    comps_list_destroy((COMPS_List**)&res->citem->data);
    res->citem->data = res_list;
    return (PyObject*)res;
}

PyMemberDef PyCOMPSGroups_members[] = {
    {NULL}};

PyMethodDef PyCOMPSGroups_methods[] = {
//    {"append", (PyCFunction)PyCOMPSGroups_append, METH_O,
//    "Append item to new of the list"},
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
    (destructor)PyCOMPSCtoPySeq_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//&PyCOMPSGroups_cmp,          /*tp_compare*/
    0,                         /*tp_repr*/
    &PyCOMPSGroups_Nums,         /*tp_as_number*/
    0,//&PyCOMPSGroup_sequence,       /*tp_as_sequence*/
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
    PyCOMPSGroups_methods,        /* tp_methods */
    PyCOMPSGroups_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_CtoPySeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSGroups_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSCtoPySeq_new,                 /* tp_new */};

int PyCOMPSPacks_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds)
{
   (void) args;
   (void) kwds;
    self->item_man = &PyCOMPSPack_ItemMan;
    return 0;
}

PyMemberDef PyCOMPSPacks_members[] = {
    {NULL}};

PyMethodDef PyCOMPSPacks_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyCOMPS_PacksType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Category.PackageList",   /*tp_name*/
    sizeof(PyCOMPS_CtoPySeq), /*tp_basicsize*/
    0,                        /*tp_itemsize*/
    (destructor)PyCOMPSCtoPySeq_dealloc, /*tp_dealloc*/
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
    PyCOMPSCtoPySeq_str,           /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS,        /*tp_flags*/
    "Comps Group Package list",           /* tp_doc */
    0,                    /* tp_traverse */
    0,                     /* tp_clear */
    &PyCOMPSCtoPySeq_cmp,       /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    PyCOMPSPacks_methods,        /* tp_methods */
    PyCOMPSPacks_members,        /* tp_members */
    0,                         /* tp_getset */
    &PyCOMPS_CtoPySeqType,           /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyCOMPSPacks_init,      /* tp_init */
    0,                               /* tp_alloc */
    PyCOMPSCtoPySeq_new,                 /* tp_new */};

inline COMPS_DocGroupPackage* pycomps_pkg_get(PyObject *pypkg) {
    return (COMPS_DocGroupPackage*)((PyCOMPS_Package*)pypkg)->citem->data;
}

inline void pycomps_pkg_destroy(void * pkg) {
    free(((COMPS_DocGroupPackage*)pkg)->name);
    free(pkg);
}

inline void pycomps_pkg_incref(void * pkg) {
    ctopy_citem_incref(
                (PyCOMPS_CtoPy_CItem*)((COMPS_DocGroupPackage*)pkg)->reserved);
}

inline void pycomps_pkg_decref(void * pkg) {
    ctopy_citem_destroy(((COMPS_DocGroupPackage*)pkg)->reserved);
}

PyObject* PyCOMPSPack_convert(void *p) {
    PyObject *ret;
    ret = PyCOMPSPack_new(&PyCOMPS_PackType, NULL, NULL);
    PyCOMPSPack_init((PyCOMPS_Package*)ret, NULL, NULL);
    ctopy_citem_decref(((PyCOMPS_Package*)ret)->citem);
    ((PyCOMPS_Package*)ret)->citem = ((COMPS_DocGroupPackage*)p)->reserved;
    ctopy_citem_incref(((PyCOMPS_Package*)ret)->citem);
    return ret;
}

void PyCOMPSPack_dealloc(PyCOMPS_Package *self)
{
    ctopy_citem_decref(self->citem);
    Py_TYPE(self)->tp_free((PyObject*)self);
    self = NULL;
}

PyObject* PyCOMPSPack_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCOMPS_Package *self;
    COMPS_DocGroupPackage *p;

   (void) args;
   (void) kwds;

    self = (PyCOMPS_Package*) type->tp_alloc(type, 0);
    if (self != NULL) {
        p = comps_docpackage_create();
        p->reserved = ctopy_citem_create(p, &pycomps_pkg_destroy);
        self->citem = p->reserved;
    }
    return (PyObject*) self;
}

int PyCOMPSPack_init(PyCOMPS_Package *self, PyObject *args, PyObject *kwds)
{
    char *name = NULL;
    long type = COMPS_PACKAGE_UNKNOWN;

   (void) kwds;

    if (args!=NULL && PyArg_ParseTuple(args, "|si", &name, &type)) {
        comps_docpackage_set_name(pycomps_pkg_get((PyObject*)self), name, 1);
        comps_docpackage_set_type(pycomps_pkg_get((PyObject*)self), type);
        return 0;
    } else {
        pycomps_pkg_get((PyObject*)self)->name = NULL;
        pycomps_pkg_get((PyObject*)self)->type = COMPS_PACKAGE_UNKNOWN;
        return 0;
    }
    return 1;
}

PyObject* PyCOMPSPack_get_name(PyCOMPS_Package *self, void *closure) {
   (void) closure;
    return PyUnicode_FromString(pycomps_pkg_get((PyObject*)self)->name);
}

int PyCOMPSPack_set_name(PyCOMPS_Package *self, PyObject *value, void *closure) {
    char *tmp;
    (void) closure;
    if (__pycomps_arg_to_char(value, &tmp)) {
        return -1;
    }
    comps_docpackage_set_name(pycomps_pkg_get((PyObject*)self), tmp, 0);
    return 0;
}

PyObject* PyCOMPSPack_get_type(PyCOMPS_Package *self, void *closure) {
   (void) closure;
    switch (pycomps_pkg_get((PyObject*)self)->type) {
        case COMPS_PACKAGE_DEFAULT:
            return Py_BuildValue("%d",0);
        case COMPS_PACKAGE_OPTIONAL:
            return Py_BuildValue("%d",1);
        case COMPS_PACKAGE_MANDATORY:
            return Py_BuildValue("%d",2);
        case COMPS_PACKAGE_UNKNOWN:
            return Py_BuildValue("%d",3);
        case COMPS_PACKAGE_CONDITIONAL:
            return Py_BuildValue("%d",4);
    }
    Py_RETURN_NONE;
}

int PyCOMPSPack_set_type(PyCOMPS_Package *self, PyObject *value, void *closure){
   (void) closure;
    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "Not a integer");
        return -1;
    }
    if (PyLong_AsLong(value)>3 || PyLong_AsLong(value)<0) {
        PyErr_SetString(PyExc_ValueError, "Out of range");
        return -1;
    }
    pycomps_pkg_get((PyObject*)self)->type = PyLong_AsLong(value);
    return 0;
}

PyGetSetDef pack_getset[] = {
    {"name",
     (getter)PyCOMPSPack_get_name, (setter)PyCOMPSPack_set_name,
     "Package name",
     NULL},
    {"type",
     (getter)PyCOMPSPack_get_type, (setter)PyCOMPSPack_set_type,
     "Package type",
     NULL},
    {NULL}  /* Sentinel */
};

PyObject* PyCOMPSPack_str(PyObject *self) {
    PyObject * ret;
    char *name;
    const char *type;
    char * empty = "";
    name = (pycomps_pkg_get(self)->name)?
          pycomps_pkg_get(self)->name:empty;
    type = comps_docpackage_type_str(pycomps_pkg_get(self)->type);
    ret = PyUnicode_FromFormat("<COMPS_Package: name='%s', type=%s>",
                               name, type);
    return ret;
}

inline void comps_pkg_print(FILE *f, void *p) {
    const char *type;
    type = comps_docpackage_type_str(((COMPS_DocGroupPackage*)p)->type);
    fprintf(f, "<COPMS_Package name='%s' type='%s'>",
            ((COMPS_DocGroupPackage*)p)->name, type);
}

int PyCOMPSPack_print(PyObject *self, FILE *f, int flags) {
    (void) flags;
    comps_pkg_print(f, (void*)pycomps_pkg_get(self));
    return 0;
}

PyObject* PyCOMPSPack_cmp(PyObject *self, PyObject *other, int op) {
    PyObject *ret0, *ret1;
    if (other == NULL || other->ob_type != &PyCOMPS_PackType) {
        PyErr_SetString(PyExc_TypeError, "Not COMPS_Pack instance");
        return NULL;
    }
    if (op != Py_EQ && op != Py_NE) {
        return Py_NotImplemented;
    }
    ret0 = (op == Py_EQ)?Py_True:Py_False;
    ret1 = (op == Py_EQ)?Py_False:Py_True;

    if (__pycomps_strcmp(pycomps_pkg_get(self)->name,
                        pycomps_pkg_get(self)->name) != 0) {
        Py_INCREF(ret1);
        return ret1;
    }
    if (pycomps_pkg_get(self)->type !=
        pycomps_pkg_get(self)->type) {
        Py_INCREF(ret1);
        return ret1;
    }
    Py_INCREF(ret0);
    return ret0;
}

char pycomps_pack_cmp(void *p1, void *p2) {
    if (__pycomps_strcmp(((COMPS_DocGroupPackage*)p1)->name,
                         ((COMPS_DocGroupPackage*)p2)->name)) return 0;
    if (((COMPS_DocGroupPackage*)p1)->type!=((COMPS_DocGroupPackage*)p2)->type)
        return 0;
    return 1;
}

PyMemberDef PyCOMPSPack_members[] = {
    {NULL}};

PyMethodDef PyCOMPSPack_methods[] = {
    {NULL}  /* Sentinel */
};

PyCOMPS_CtoPySeqItemMan PyCOMPSPack_ItemMan = {
    .item_type = &PyCOMPS_PackType,
    .ctopy_convert = PyCOMPSPack_convert,
    .data_incref = pycomps_pkg_incref,
    .data_cmp = pycomps_pack_cmp,
    .data_decref = pycomps_pkg_decref,
    .fprint_f = comps_pkg_print
};

PyTypeObject PyCOMPS_PackType = {
    PY_OBJ_HEAD_INIT
    "libcomps.Group.Package",   /*tp_name*/
    sizeof(PyCOMPS_Package), /*tp_basicsize*/
    0,                        /*tp_itemsize*/
    (destructor)PyCOMPSPack_dealloc, /*tp_dealloc*/
    PyCOMPSPack_print,         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,//PyCOMPSPack_cmp,           /*tp_compare*/
    0,                         /*tp_repr*/
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

