#ifndef PYCOMPS_GROUPS_H
#define PYCOMPS_GROUPS_H

#include <Python.h>
#include "libcomps/comps.h"
#include "libcomps/comps_list.h"
#include "structmember.h"
#include "pycomps_sequence.h"
#include "pycomps_utils.h"
#include "pycomps_ids.h"
#include "pycomps_dict.h"
#include "pycomps_23macros.h"
#include "pycomps_ctopy_seq.h"
#include "pycomps_lang.h"

typedef struct {
    PyObject_HEAD
    PyCOMPS_CtoPy_PItem_HEAD
    PyObject *packages_pobj;
    PyObject *name_by_lang_pobj;
    PyObject *desc_by_lang_pobj;
} PyCOMPS_Group;

typedef struct {
    PyObject_HEAD
    PyCOMPS_CtoPy_PItem_HEAD
    //COMPS_DocGroupPackage * package;
} PyCOMPS_Package;

typedef struct COMPS_DocGroupExtra {
    PyCOMPS_CtoPy_CItem * citem;
    PyCOMPS_CtoPy_CItem * packages_citem;
    PyCOMPS_CtoPy_CItem * name_by_lang_citem;
    PyCOMPS_CtoPy_CItem * desc_by_lang_citem;
} COMPS_DocGroupExtra;

COMPS_DocGroup *pycomps_group_get(PyObject *pygroup);
void pycomps_group_destroy(void * group);
void pycomps_group_decref(void * cat);
COMPS_DocGroupExtra* pycomps_group_get_extra(PyObject *pygroup);
COMPS_DocGroupExtra* comps_group_get_extra(void* group);
void comps_group_incref(void * group);
PyObject* comps_group_str(void * group);
void comps_group_print(FILE *f, void *g);

COMPS_DocGroupExtra * comps_docgroup_extra_create();

//COMPS_DocGroup* docgroup_from_pycompsgroup(PyObject *self);
COMPS_List* comps_groups_union(COMPS_List *groups1, COMPS_List *groups2);
char comps_group_cmp(void *g1, void *g2);

void PyCOMPSGroup_dealloc(PyObject *self);
PyObject * PyCOMPSGroup_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSGroup_init(PyCOMPS_Group *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSGroup_get_name_by_lang(PyCOMPS_Group *self, void *closure);
int PyCOMPSGroup_set_name_by_lang(PyCOMPS_Group *self, PyObject *value,
                                                       void *closure);
PyObject* PyCOMPSGroup_get_desc_by_lang(PyCOMPS_Group *self, void *closure);
int PyCOMPSGroup_set_desc_by_lang(PyCOMPS_Group *self, PyObject *value,
                                                       void *closure);


PyObject * PyCOMPSGroups_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSGroups_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds);

void pycomps_pkg_decref(void * pkg);
void pycomps_pkg_incref(void * pkg);
void pycomps_pkg_destroy(void * pkg);
void comps_pkg_print(FILE *f, void *p);

COMPS_DocGroupPackage* pycomps_pkg_get(PyObject *pypkg);
PyObject* PyCOMPSPack_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCOMPSPack_init(PyCOMPS_Package *self, PyObject *args, PyObject *kwds);
PyObject* PyCOMPSPack_convert(void *p);

int PyCOMPSPacks_init(PyCOMPS_CtoPySeq *self, PyObject *args, PyObject *kwds);

extern PyTypeObject PyCOMPS_GroupsType;
extern PyTypeObject PyCOMPS_GroupType;
extern PyTypeObject PyCOMPS_PacksType;
extern PyTypeObject PyCOMPS_PackType;
extern PyCOMPS_CtoPySeqItemMan PyCOMPSPack_ItemMan;

#endif
