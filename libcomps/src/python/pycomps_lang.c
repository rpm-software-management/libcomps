#include "pycomps_lang.h"

PyObject* pycomps_lang_get_dict(PyCOMPS_CtoPy_CItem *citem,
                                PyObject **pobj) {
    PyObject *ret;
    if (!*pobj) {
        ret = PyCOMPSDict_new(&PyCOMPS_DictType, NULL, NULL);
        PyCOMPSDict_init((PyCOMPS_Dict*)ret, NULL, NULL);
        ctopy_citem_destroy(((PyCOMPS_Dict*)ret)->citem);
        ((PyCOMPS_Dict*)ret)->citem = citem;
        *pobj = ret;
        ctopy_citem_incref(((PyCOMPS_Dict*)ret)->citem);
        Py_INCREF(ret);
    } else {
        ret = *pobj;
        Py_INCREF(ret);
    }
    return  ret;
}
int pycomps_lang_set_dict(PyCOMPS_CtoPy_CItem **citem,
                          PyObject **pobj,
                          PyObject *value,
                          void **data) {
    if (value->ob_type != &PyCOMPS_DictType) {
        PyErr_SetString(PyExc_TypeError, "Not GroupIds instance");
        return -1;
    }
    ctopy_citem_decref(*citem);
    *citem = ((PyCOMPS_Dict*)value)->citem;
    ctopy_citem_incref(*citem);
    *data = ((PyCOMPS_Dict*)value)->citem->data;
    *pobj = value;
    return 0;
}

PyObject* comps_lang_str(COMPS_Dict * lang) {
    PyObject *ret, *tmp, *tmp2;
    COMPS_HSList * pairlist;
    COMPS_HSListItem *hsit;

    pairlist = comps_rtree_pairs(lang);
    ret = PyUnicode_FromString("{");
    for (hsit = pairlist->first; hsit != pairlist->last; hsit = hsit->next) {
        tmp2 = PyUnicode_FromFormat("'%s': '%s', ",
                               ((COMPS_RTreePair*)hsit->data)->key,
                               (char*)((COMPS_RTreePair*)hsit->data)->data);
        tmp = PyUnicode_Concat(ret, tmp2);
        Py_DECREF(tmp2);
        Py_XDECREF(ret);
        ret = tmp;

    }
    if (hsit) {
        tmp2 = PyUnicode_FromFormat("'%s': '%s'}",
                               ((COMPS_RTreePair*)hsit->data)->key,
                               (char*)((COMPS_RTreePair*)hsit->data)->data);
        tmp = PyUnicode_Concat(ret, tmp2);
        Py_DECREF(tmp2);
        Py_DECREF(ret);
        ret = tmp;
    } else {
        tmp2 = PyUnicode_FromString("}");
        tmp = PyUnicode_Concat(ret, tmp2);
        Py_DECREF(tmp2);
        Py_DECREF(ret);
        ret = tmp;
    }
    comps_hslist_destroy(&pairlist);
    return ret;
}
