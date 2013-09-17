#ifndef COMPS_DOCENV_H
#define COMPS_DOCENV_H

#include <stddef.h>
#include <assert.h>

#include "comps_utils.h"
#include "comps_obj.h"
#include "comps_objdict.h"
#include "comps_objlist.h"
#include "comps_docgroupid.h"
#define COMPS_DOCOBJ_GETOBJLIST(OBJ, OBJTYPE, MEMBER, OBJS)\
COMPS_ObjList* CONCAT(CONCAT(CONCAT(comps_, OBJ), _), OBJS) (OBJTYPE *obj){\
    return obj->MEMBER;\
}
#define HEAD_COMPS_DOCOBJ_GETOBJLIST(OBJ, OBJTYPE, MEMBER, OBJS)\
COMPS_ObjList* CONCAT(CONCAT(CONCAT(comps_, OBJ), _), OBJS) (OBJTYPE *obj);

typedef struct {
    COMPS_Object_HEAD
    COMPS_ObjDict *properties;
    COMPS_ObjDict *name_by_lang;
    COMPS_ObjDict *desc_by_lang;
    COMPS_ObjList *group_list;
    COMPS_ObjList *option_list;
} COMPS_DocEnv;

HEAD_COMPS_CREATE_u(docenv, COMPS_DocEnv)  /*comps_utils.h macro*/
HEAD_COMPS_COPY_u(docenv, COMPS_DocEnv)  /*comps_utils.h macro*/
HEAD_COMPS_DESTROY_u(docenv, COMPS_DocEnv)  /*comps_utils.h macro*/

HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, id) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, name) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, desc) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, display_order) /*comps_utils.h macro*/

HEAD_COMPS_STRPROP_SETTER(env, COMPS_DocEnv, id) /*comps_utils.h macro*/
HEAD_COMPS_STRPROP_SETTER(env, COMPS_DocEnv, name) /*comps_utils.h macro*/
HEAD_COMPS_STRPROP_SETTER(env, COMPS_DocEnv, desc) /*comps_utils.h macro*/
HEAD_COMPS_NUMPROP_SETTER(env, COMPS_DocEnv, display_order) /*comps_utils.h macro*/

HEAD_COMPS_DOCOBJ_GETOBJLIST(docenv, COMPS_DocEnv, group_list, group_list)
HEAD_COMPS_DOCOBJ_GETOBJLIST(docenv, COMPS_DocEnv, option_list, option_list)

char __comps_docenv_idcmp(void *e1, void *e2);

void comps_docenv_add_groupid(COMPS_DocEnv *env,
                            COMPS_DocGroupId *gid);
void comps_docenv_add_optionid(COMPS_DocEnv *env,
                            COMPS_DocGroupId *gid);
COMPS_DocEnv* comps_docenv_union(COMPS_DocEnv *e1, COMPS_DocEnv *e2);
COMPS_DocEnv* comps_docenv_intersect(COMPS_DocEnv *e1, COMPS_DocEnv *e2);

void comps_docenv_xml(COMPS_DocEnv *env, xmlTextWriterPtr writer,
                        COMPS_Logger *log);

extern COMPS_ObjectInfo COMPS_DocEnv_ObjInfo;

#endif

