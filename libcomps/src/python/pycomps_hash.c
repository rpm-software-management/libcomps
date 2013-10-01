#include "pycomps_hash.h"

long PyCOMPS_hash(PyObject *self) {
    PyObject * str;
    char *cstr = NULL;
    long crc;

    str = Py_TYPE(self)->tp_str(self);
    __pycomps_PyUnicode_AsString(str, &cstr);
    Py_DECREF(str);
    crc = crc32(0, cstr, strlen(cstr));
    free(cstr);
    return crc;
}

