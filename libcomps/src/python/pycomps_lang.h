#ifndef PYCOMPS_LANG_H
#define PYCOMPS_LANG_H

#include "pycomps_dict.h"
#include "pycomps_ctopy.h"

PyObject* pycomps_lang_get_dict(PyCOMPS_CtoPy_CItem *citem,
                                PyObject **pobj);
int pycomps_lang_set_dict(PyCOMPS_CtoPy_CItem **citem,
                          PyObject **pobj,
                          PyObject *value,
                          void **data);

PyObject* comps_lang_str(COMPS_Dict * lang);

#endif
