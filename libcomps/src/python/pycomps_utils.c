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

inline char __pycomps_PyUnicode_AsString(PyObject *val, char **ret) {
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

inline char __pycomps_stringable_to_char(PyObject *value, char ** ret) {
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

inline char __pycomps_arg_to_char(PyObject *value, char ** ret) {
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
            PyErr_SetString(PyExc_TypeError, "PyUnicode_Decode error");
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

long PyCOMPS_hash(PyObject *self) {
    PyObject * str;
    char *cstr;

    str = Py_TYPE(self)->tp_str(self);
    __pycomps_PyUnicode_AsString(str, &cstr);
    Py_DECREF(str);
    return crc32(0, cstr, strlen(cstr));
}
