#ifndef COMPS_DOCENV_H
#define COMPS_DOCENV_H

#include <stddef.h>
#include <assert.h>

#include "comps_utils.h"
#include "comps_obj.h"
#include "comps_objdict.h"
#include "comps_objlist.h"
#include "comps_docgroupid.h"
#include "comps_validate.h"
#include "comps_radix.h"
#include "comps_log.h"
#include "comps_default.h"

/** COMPS_Object derivate representing environment element in comps.xml file */
typedef struct {
    COMPS_Object_HEAD;
    COMPS_ObjDict *properties;
    /**< properties of group */
    COMPS_ObjDict *name_by_lang;
    /**< language localization of name attribute */
    COMPS_ObjDict *desc_by_lang;
    /**< language localization of description attribute */
    COMPS_ObjList *group_list;
    /**< list of group_ids in environment */
    COMPS_ObjList *option_list;
    /**< list of options in environment */
} COMPS_DocEnv;

//HEAD_COMPS_CREATE_u(docenv, COMPS_DocEnv)  /*comps_utils.h macro*/
//HEAD_COMPS_COPY_u(docenv, COMPS_DocEnv)  /*comps_utils.h macro*/
//HEAD_COMPS_DESTROY_u(docenv, COMPS_DocEnv)  /*comps_utils.h macro*/

/** \file comps_docenv.h
 * \brief COMPS_DocEnv header file
 * @see COMPS_DocEnv_prop_setters
 * @see COMPS_DocEnv_prop_getters
 * @see COMPS_DocEnv_list_setters
 * @see COMPS_DocEnv_list_getters
 *
 * COMPS_DocEnv object support union operation. Read more about
 * @link doc_unioning Libcomps objects unioning
 * @endlink
 */

/**
 * \defgroup COMPS_DocEnv_prop_setters COMPS_DocEnv properties setters
 * @{
 **/

/** COMPS_DocEnv id setter
 * @param obj COMPS_DocEnv object
 * @param id COMPS_Str object representing id
 * \warning setter doesn't increment reference counter of id object
 */
HEAD_COMPS_STRPROP_SETTER(env, COMPS_DocEnv, id) /*comps_utils.h macro*/

/** COMPS_DocEnv name setter
 * @param obj COMPS_DocEnv object
 * @param name COMPS_Str object representing name
 * \warning setter doesn't increment reference counter of name object
 */
HEAD_COMPS_STRPROP_SETTER(env, COMPS_DocEnv, name) /*comps_utils.h macro*/

/** COMPS_DocEnv description setter
 * @param obj COMPS_DocEnv object
 * @param desc COMPS_Str object representing description
 * \warning setter doesn't increment reference counter of desc object
 */
HEAD_COMPS_STRPROP_SETTER(env, COMPS_DocEnv, desc) /*comps_utils.h macro*/

/** COMPS_DocEnv display order setter
 * @param obj COMPS_DocEnv object
 * @param display_order COMPS_Num object representing display order
 * \warning setter doesn't increment reference counter of display_order object
 */
HEAD_COMPS_NUMPROP_SETTER(env, COMPS_DocEnv, display_order) /*comps_utils.h macro*/
/**@}*/

/**
 * \defgroup COMPS_DocEnv_prop_getters COMPS_DocEnv properties getters
 * @{
 **/

/** COMPS_DocEnv id getter
 * @param obj COMPS_DocEnv object
 * @return COMPS_Str object representing group id with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, id) /*comps_utils.h macro*/
HEAD_COMPS_PROP_GETTER_OBJ(env, id) /*comps_utils.h macro*/

/** COMPS_DocEnv name getter
 * @param obj COMPS_DocEnv object
 * @return COMPS_Str object representing group name with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, name) /*comps_utils.h macro*/

/** COMPS_DocEnv description getter
 * @param obj COMPS_DocEnv object
 * @return COMPS_Str object representing group description with incremented
 * reference count
 */
HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, desc) /*comps_utils.h macro*/

/** COMPS_DocCategory display order getter
 * @param obj COMPS_DocCategory object
 * @return COMPS_Str object representing category display order with
 * incremented reference count
 */
HEAD_COMPS_PROP_GETTER(env, COMPS_DocEnv, display_order) /*comps_utils.h macro*/
/**@}*/

/**
 * \defgroup COMPS_DocEnv_list_getters COMPS_DocEnv list getters
 * @{
 **/

/** COMPS_DocEnv group_ids list getter
 * @param obj COMPS_DocEnv object
 * @return COMPS_ObjList with packages in group. Reference of list isn't
 * incremented
 */
HEAD_COMPS_DOCOBJ_GETOBJLIST(docenv, COMPS_DocEnv, group_list, group_list)

/** COMPS_DocEnv option_ids list getter
 * @param obj COMPS_DocEnv object
 * @return COMPS_ObjList with packages in group. Reference of list isn't
 * incremented
 */
HEAD_COMPS_DOCOBJ_GETOBJLIST(docenv, COMPS_DocEnv, option_list, option_list)
/**@}*/

/**
 * \defgroup COMPS_DocEnv_list_setters COMPS_DocEnv list setters
 * @{
 **/

/** COMPS_DocEnv group_ids list setter
 * @param obj COMPS_DocEnv object
 * @param list COMPS_ObjList object with group_ids items
 * \warning existing group_ids list object reference count will be decremented.
 * Setter doesn't provides any additional items type checking
 */
HEAD_COMPS_DOCOBJ_SETOBJLIST(docenv, COMPS_DocEnv, group_list, group_list)

/** COMPS_DocEnv option_ids list setter
 * @param obj COMPS_DocEnv object
 * @param list COMPS_ObjList object with group_ids items
 * \warning existing option_ids list object reference count will be decremented.
 * Setter doesn't provides any additional items type checking
 */
HEAD_COMPS_DOCOBJ_SETOBJLIST(docenv, COMPS_DocEnv, option_list, option_list)
/**@}*/

HEAD_COMPS_DOCOBJ_GETARCHES(docenv, COMPS_DocEnv)
HEAD_COMPS_DOCOBJ_SETARCHES(docenv, COMPS_DocEnv)

char __comps_docenv_idcmp(void *e1, void *e2);

/** add group_id to group_ids list in environment
 * @param env COMPS_DocEnv object
 * @param gid COMPS_DocGroupId object
 * \warning COMPS_DocGroupId reference counter isn't incremented
 */
void comps_docenv_add_groupid(COMPS_DocEnv *env,
                            COMPS_DocGroupId *gid);

/** add group_id to option list in environment
 * @param env COMPS_DocEnv object
 * @param gid COMPS_DocGroupId object
 * \warning COMPS_DocGroupId reference counter isn't incremented
 */
void comps_docenv_add_optionid(COMPS_DocEnv *env,
                            COMPS_DocGroupId *gid);

/** union two environments into one and return new COMPS_DocEnv object
 * @param e1 COMPS_DocEnv object
 * @param e2 COMPS_DocEnv object
 * @return new COMPS_DocEnv object
 */
COMPS_DocEnv* comps_docenv_union(COMPS_DocEnv *e1, COMPS_DocEnv *e2);

/** intersect two environments into one and return new COMPS_DocEnv object
 * @param e1 COMPS_DocEnv object
 * @param e2 COMPS_DocEnv object
 * @return new COMPS_DocEnv object
 */
COMPS_DocEnv* comps_docenv_intersect(COMPS_DocEnv *e1, COMPS_DocEnv *e2);

signed char comps_docenv_xml(COMPS_DocEnv *env, xmlTextWriterPtr writer,
                             COMPS_Log *log, COMPS_XMLOptions *xml_options,
                             COMPS_DefaultsOptions *def_options);
COMPS_DocEnv* comps_docenv_arch_filter(COMPS_DocEnv *source,
                                       COMPS_ObjList *arches);

extern COMPS_ObjectInfo COMPS_DocEnv_ObjInfo;
extern COMPS_ValRuleGeneric* COMPS_DocEnv_ValidateRules[];

#endif

