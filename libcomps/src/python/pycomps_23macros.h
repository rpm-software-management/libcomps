#ifndef PYCOMPS_23MACROS_H
#define PYCOMPS_23MACROS_H
#include <Python.h>

#if PY_MAJOR_VERSION >= 3
    #define PYINIT_FUNC PyInit__libpycomps
#else
    #define PYINIT_FUNC init_libpycomps
#endif

#if PY_MAJOR_VERSION >= 3
    #define MODINIT_RET_NONE return NULL
    #define PY_OBJ_HEAD_INIT PyVarObject_HEAD_INIT(NULL, 0)
    #define IS_PY3K
    #define PyINT_CHECK(OB) PyLong_Check(OB)
    #define PyINT_ASLONG(OB) PyLong_AsLong(OB)
    #define SLICE_CAST PyObject*
#else
    #define MODINIT_RET_NONE return
    #define PY_OBJ_HEAD_INIT PyObject_HEAD_INIT(NULL)\
                             0,
    #define PyINT_CHECK(OB) PyInt_Check(OB)
    #define PyINT_ASLONG(OB) PyInt_AsLong(OB)

    #define SLICE_CAST PySliceObject*
#endif




#define CMP_OP_EQ_NE_CHECK(OP)                                  \
    if ((OP) != Py_EQ && (OP) != Py_NE) {                       \
        PyErr_Format(PyExc_TypeError, "Unsuported operator");   \
        return Py_INCREF(Py_NotImplemented), Py_NotImplemented; \
    }

#define CMP_NONE_CHECK(OP, SELF, OTHER)                         \
    if ((OTHER) == Py_None && (SELF) == Py_None)                \
        return ((OP) == Py_EQ)?(Py_INCREF(Py_True),Py_True):    \
                             (Py_INCREF(Py_False),Py_False);    \
    else if (((OTHER) == Py_None && (SELF) != Py_None) ||       \
             ((OTHER) != Py_None && (SELF) == Py_None))         \
        return ((OP) == Py_EQ)?(Py_INCREF(Py_False),Py_False):  \
                             (Py_INCREF(Py_True),Py_True);      \

#define PYUNICODE_ASSTRING(U) PyBytes_AsString(PyUnicode_AsUTF8String(U))

#define SET_TO(STRUCT, OFFSET, P) *((size_t*)(((char*)STRUCT)+OFFSET)) = (size_t)P;
#define GET_FROM(STRUCT, OFFSET) *((char**)(((char*)STRUCT)+OFFSET))

#endif /*PYCOMPS_23MACROS_H*/
