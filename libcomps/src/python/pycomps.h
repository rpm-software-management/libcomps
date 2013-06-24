#ifndef PYCOMPS_H
#define PYCOMPS_H

#include <Python.h>
#include "stddef.h"

#include "libcomps/comps.h"
#include "libcomps/comps_parse.h"
#include "libcomps/comps_dict.h"
#include "structmember.h"
#include "pycomps_categories.h"
#include "pycomps_groups.h"
#include "pycomps_envs.h"
#include "pycomps_utils.h"
#include "pycomps_dict.h"
#include "pycomps_ctopy.h"
#include "pycomps_ctopy_seq.h"
#include "pycomps_ctopy_seq_iter.h"

typedef struct {
    PyTypeObject * type;
    size_t citem_offset;
    size_t pobj_offset;
    COMPS_List *(*get_f)(COMPS_Doc*);
    void (*set_f)(COMPS_Doc*, COMPS_List*);
} PyCOMPS_GetSetClosure;

typedef struct {
    PyObject_HEAD
    COMPS_Doc * comps;
    PyCOMPS_CtoPy_CItem *cats_citem;
    PyCOMPS_CtoPy_CItem *groups_citem;
    PyCOMPS_CtoPy_CItem *envs_citem;

    PyObject *cats_pobj;
    PyObject *groups_pobj;
    PyObject *envs_pobj;
    PyObject *enc;
} PyCOMPS;

PyCOMPS_GetSetClosure * get_closure(void * closure);

static void pycomps_doc_destroy(void *doc);

extern PyTypeObject PyCOMPS_Type;

#endif
