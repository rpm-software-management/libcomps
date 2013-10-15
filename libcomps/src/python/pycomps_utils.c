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

#include "pycomps_utils.h"

inline char __pycomps_strcmp(const char *s1, const char *s2){
    if (s1 == NULL && s2 == NULL)
        return 0;
    if (s1 == NULL && s2 != NULL)
        return 1;
    if (s2 == NULL && s1 != NULL)
        return -1;
    return (char)strcmp(s1,s2);
}

inline char __pycomps_strcmp_v(void *s1, void *s2){
    return (strcmp((char*)s1, (char*)s2) == 0);
}

inline PyObject* __pycomps_str_getter(char * prop){
    if (prop == NULL) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(prop);
}

inline char __pycomps_set_nodel_check(PyObject * value){
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete attribute");
        return 0;
    }
    return 1;
}

inline PyObject* __pycomps_arg_to_unicode(PyObject *o) {
    PyObject *tmp;

    if (o == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot convert to unicode");
        return NULL;
    }

    if (o == Py_None) {
        tmp = Py_None;
        Py_INCREF(tmp);
    } else
        tmp = PyUnicode_FromObject(o);

    return tmp;
}

inline PyObject* __pycomps_arg_to_unicode2(PyObject *o) {
    PyObject *tmp;

    if (o == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot convert to unicode");
        return NULL;
    }
    tmp = PyUnicode_FromObject(o);
    return tmp;
}

inline signed char __pycomps_PyUnicode_AsString(PyObject *val, char **ret) {
    PyObject *o;
    char *tmp;

    if (val == Py_None) {
        *ret = NULL;
        return 1;
    } else {
        o = PyUnicode_AsUTF8String(val);
        if (o == NULL) {
            PyErr_SetString(PyExc_TypeError, "Cannot decode to UTF8");
            return -1;
        }
        tmp = PyBytes_AsString(o);
        if (tmp == NULL) {
            return -1;
        }
        *ret = malloc(sizeof(char) * (strlen(tmp)+1));
        memcpy(*ret, tmp, sizeof(char) * (strlen(tmp)+1));
        Py_XDECREF(o);
    }
    if (*ret == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot convert to string");
        return -2;
    }
    return 0;
}

inline signed char __pycomps_stringable_to_char(PyObject *value, char ** ret) {
    PyObject *tmpo;
    int retval;

    tmpo = __pycomps_arg_to_unicode(value);
    if (!tmpo) {
        *ret = NULL;
        return -1;
    }
    retval = __pycomps_PyUnicode_AsString(tmpo, ret);
    Py_XDECREF(tmpo);
    return retval;
}

inline PyObject *str_to_unicode(void* str) {
    return PyUnicode_FromString((const char*) str);
}

inline signed char __pycomps_arg_to_char(PyObject *value, char ** ret) {
    PyObject *tmpo;
    int retval;

    tmpo = __pycomps_arg_to_unicode2(value);
    if (!tmpo) {
        *ret = NULL;
        return -1;
    }
    retval = __pycomps_PyUnicode_AsString(tmpo, ret);
    Py_XDECREF(tmpo);
    return retval;
}

void* __pycomps_strcloner(void *str) {
    void *ret;
    ret = malloc(sizeof(char)*(strlen(str)+1));
    memcpy(ret, str, sizeof(char)*(strlen(str)+1));
    return ret;
}

PyObject* __pycomps_lang_decode(char * lang) {
    const char *errors = NULL;
    PyObject *tmp;
    PyObject *ret = NULL;
    #if PY_MAJOR_VERSION >= 3
        tmp = PyUnicode_Decode(lang, strlen(lang), "UTF-8", errors);
        ret = PyUnicode_FromObject(tmp);
    #else
        tmp = PyUnicode_Decode(lang, strlen(lang), "UTF-8", errors);
        if (!tmp) {
            //PyErr_SetString(PyExc_TypeError, "PyUnicode_Decode error");
            return NULL;
        }
        ret = PyUnicode_FromObject(tmp);
        if (!ret) {
            return NULL;
        }
        Py_XDECREF(tmp);
    #endif
    return ret;
}

PyObject* __PyCOMPS_get_ids(PyObject *self, void *closure) {
    #define _closure_ ((__PyCOMPS_ListGetSetClosure*)closure)

    PyCOMPS_Sequence *ret = (PyCOMPS_Sequence*)GET_FROM(self, _closure_->p_offset);
    COMPS_Object * c_obj;

    if (!ret) {
        ret = (PyCOMPS_Sequence*)_closure_->type->tp_new(_closure_->type,
                                                          NULL, NULL);
        _closure_->type->tp_init((PyObject*)ret, NULL, NULL);
        COMPS_OBJECT_DESTROY(ret->list);
        c_obj = (COMPS_Object*) GET_FROM(self, _closure_->c_offset);
        ret->list = (COMPS_ObjList*)
                    comps_object_incref((COMPS_Object*)_closure_->get_f(c_obj));
    } else {
        Py_INCREF(ret);
    }
    return  (PyObject*)ret;
    #undef _closure_
}

int __PyCOMPS_set_ids(PyObject *self, PyObject *value, void *closure) {
    #define _closure_ ((__PyCOMPS_ListGetSetClosure*)closure)
    PyCOMPS_Sequence *pobj;
    (void) closure;
    (void) self;
    if (!value) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete attribute option_ids");
        return -1;
    }
    if (value->ob_type != _closure_->type) {
        PyErr_Format(PyExc_TypeError, "Not %s instance",_closure_->type->tp_name);
        return -1;
    }
    COMPS_Object * c_obj;
    c_obj = (COMPS_Object*) GET_FROM(self, _closure_->c_offset);

    _closure_->set_f(c_obj, ((PyCOMPS_Sequence*)value)->list);
    pobj = (PyCOMPS_Sequence*)GET_FROM(self, _closure_->p_offset);
    Py_XDECREF(pobj);
    Py_INCREF(value);
    SET_TO(self, _closure_->p_offset, pobj);
    return 0;
    #undef _closure_
}

PyObject* __PyCOMPS_get_dict(PyObject *self, void *closure) {
    #define _closure_ ((__PyCOMPS_DictGetSetClosure*)closure)

    PyCOMPS_Dict *ret = (PyCOMPS_Dict*)GET_FROM(self, _closure_->p_offset);
    COMPS_Object * c_obj;
    COMPS_ObjDict * dict;

    if (!ret) {
        ret = (PyCOMPS_Dict*)PyCOMPSDict_new(_closure_->dict_type, NULL, NULL);
        ret->it_info = _closure_->dict_info;
        //_closure_->dict_type->tp_init((PyObject*)ret, NULL, NULL);
        COMPS_OBJECT_DESTROY(ret->dict);
        c_obj = (COMPS_Object*) GET_FROM(self, _closure_->c_offset);
        dict = (COMPS_ObjDict*) GET_FROM(c_obj, _closure_->dict_offset);
        ret->dict = (COMPS_ObjDict*)comps_object_incref((COMPS_Object*)dict);
    } else {
        Py_INCREF(ret);
    }
    return  (PyObject*)ret;
    #undef _closure_
}

int __PyCOMPS_set_dict(PyObject *self, PyObject *value, void *closure) {
    #define _closure_ ((__PyCOMPS_DictGetSetClosure*)closure)
    PyCOMPS_Dict *pobj;
    COMPS_Object * c_obj;
    COMPS_ObjDict *dict;
    if (!value) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete attribute option_ids");
        return -1;
    }
    if (Py_TYPE(value) != _closure_->dict_type) {
        PyErr_Format(PyExc_TypeError, "%s not %s instance",
                                  Py_TYPE(value)->tp_name,
                                  _closure_->dict_type->tp_name);
        return -1;
    }
    /*} else {
        PyErr_Format(PyExc_TypeError, "1 %s not %s instance",
                                  Py_TYPE(value)->tp_name,
                                  _closure_->dict_type->tp_name);
        return -1;
    }*/

    c_obj = (COMPS_Object*) GET_FROM(self, _closure_->c_offset);
    dict = (COMPS_ObjDict*) GET_FROM(c_obj, _closure_->dict_offset);
    COMPS_OBJECT_DESTROY(dict);
    comps_object_incref((COMPS_Object*)((PyCOMPS_Dict*)value)->dict);
    SET_TO(c_obj, _closure_->dict_offset, ((PyCOMPS_Dict*)value)->dict);

    pobj = (PyCOMPS_Dict*)GET_FROM(self, _closure_->p_offset);
    Py_XDECREF(pobj);
    Py_INCREF(value);
    SET_TO(self, _closure_->p_offset, value);
    return 0;
    #undef _closure_
}

int __PyCOMPS_set_strattr(PyObject *self, PyObject *val, void *closure) {
    #define _closure_ ((__PyCOMPS_StrPropGetSetClosure*)closure)
    char *tmp;
    COMPS_Object *obj;
    if (__pycomps_stringable_to_char(val, &tmp) < 0) {
        return -1;
    }
    obj = (COMPS_Object*)GET_FROM(self, _closure_->c_offset);
    _closure_->set_f(obj, tmp, 0);
    free(tmp);
    #undef _closure_
    return 0;
}

PyObject* __PyCOMPS_get_strattr(PyObject *self, void *closure) {
    #define _closure_ ((__PyCOMPS_StrPropGetSetClosure*)closure)
    COMPS_Object* tmp_prop, *obj;
    PyObject *ret;
    char *x;

    obj = (COMPS_Object*)GET_FROM(self, _closure_->c_offset);
    tmp_prop = _closure_->get_f(obj);

    if (tmp_prop) {
        x = comps_object_tostr(tmp_prop);
        ret = PyUnicode_FromString(x);
        free(x);
        COMPS_OBJECT_DESTROY(tmp_prop);
        return ret;
    } else
        Py_RETURN_NONE;
    #undef _closure_
}

int __PyCOMPS_set_numattr(PyObject *self, PyObject *val, void *closure) {
    #define _closure_ ((__PyCOMPS_NumPropGetSetClosure*)closure)
    long tmp;
    COMPS_Object *obj;
    if (!PyINT_CHECK(val)) {
        PyErr_SetString(PyExc_TypeError, "Not int object");
        return -1;
    }
    tmp = PyINT_ASLONG(val);
    obj = (COMPS_Object*)GET_FROM(self, _closure_->c_offset);
    _closure_->set_f(obj, tmp);
    #undef _closure_
    return 0;
}

PyObject* __PyCOMPS_get_numattr(PyObject *self, void *closure) {
    #define _closure_ ((__PyCOMPS_NumPropGetSetClosure*)closure)
    COMPS_Object* tmp_prop, *obj;
    PyObject *ret;

    obj = (COMPS_Object*)GET_FROM(self, _closure_->c_offset);
    tmp_prop = _closure_->get_f(obj);
    if (tmp_prop) {
        ret = PyINT_FROM_LONG(((COMPS_Num*)tmp_prop)->val);
        COMPS_OBJECT_DESTROY(tmp_prop);
        return ret;
    } else
        Py_RETURN_NONE;
    #undef _closure_
}

