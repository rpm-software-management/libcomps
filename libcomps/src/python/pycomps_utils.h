#ifndef PYCOMPS_UTILS_H
#define PYCOMPS_UTILS_H

#include <Python.h>
#include "structmember.h"
#include "pycomps_23macros.h"
#include "crc32.h"


PyObject* __pycomps_str_getter(char * prop);
char __pycomps_strcmp(const char *s1, const char *s2);
char __pycomps_strcmp_v(void *c1, void *c2);
char __pycomps_set_nodel_check(PyObject * value);
PyObject* __pycomps_arg_to_unicode(PyObject *o);
PyObject* __pycomps_arg_to_unicode2(PyObject *o);
char __pycomps_arg_to_char(PyObject *value, char ** ret);
char __pycomps_stringable_to_char(PyObject *value, char ** ret);
char __pycomps_PyUnicode_AsString(PyObject *val, char **ret);
void* __pycomps_strcloner(void *str);
PyObject* __pycomps_lang_decode(char * lang);
PyObject *str_to_unicode(void* str);

long PyCOMPS_hash(PyObject *self);

#endif
