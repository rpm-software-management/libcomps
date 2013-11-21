#ifndef COMPS_VALIDATE_H
#define COMPS_VALIDATE_H

#include "comps_obj.h"
#include "comps_utils.h"

#define H_COMPS_VAL_RULE_GENERIC\
    COMPS_ValErr* (*rule_check)(COMPS_ValRuleGeneric*, COMPS_Object*);\
    char *verbose_msg

#define H_COMPS_VAL_RESULT COMPS_Object_HEAD

#define COMPS_VAL_RULE_PROP .rule_check = &comps_validate_rule_prop_check
#define COMPS_VAL_RULE_LIST .rule_check = &comps_validate_rule_list_check
#define COMPS_VAL_GETF COMPS_Object*(*)(COMPS_Object*)

typedef struct COMPS_ValRuleGeneric COMPS_ValRuleGeneric;
typedef struct COMPS_ValErr COMPS_ValErr;

struct COMPS_ValRuleGeneric {
    H_COMPS_VAL_RULE_GENERIC;
};

typedef struct COMPS_ValRuleProp {
    H_COMPS_VAL_RULE_GENERIC;
    COMPS_Object* (*get_f)(COMPS_Object*);
    COMPS_ValErr* (*check_f)(COMPS_Object*, COMPS_Object*);
} COMPS_ValRuleProp;

typedef struct COMPS_ValRuleList {
    H_COMPS_VAL_RULE_GENERIC;
    size_t offset;
    COMPS_ValErr* (*check_f)(COMPS_Object*, COMPS_Object*);
} COMPS_ValRuleList;

typedef struct COMPS_ValGenResult {
    H_COMPS_VAL_RESULT;
} COMPS_ValGenResult;

typedef struct COMPS_ValOkResult {
    H_COMPS_VAL_RESULT;
} COMPS_ValOkResult;

typedef struct COMPS_ValErrResult {
    H_COMPS_VAL_RESULT;
    COMPS_ObjList *err_list;
} COMPS_ValErrResult;

struct COMPS_ValErr {
    COMPS_Object_HEAD;
    char *err_msg;
};

COMPS_ValErr* comps_validate_rule_prop_check(COMPS_ValRuleGeneric *rule,
                                           COMPS_Object *obj);
COMPS_ValErr* comps_validate_rule_list_check(COMPS_ValRuleGeneric *rule,
                                           COMPS_Object *obj);
COMPS_ValGenResult* comps_validate_execute(COMPS_Object *obj,
                                   COMPS_ValRuleGeneric *rules[]);

COMPS_ValErr* comps_id_check(COMPS_Object *obj, COMPS_Object *prop);
COMPS_ValErr* comps_objlist_unique_check(COMPS_Object *object,
                                       COMPS_Object *objlist);

extern COMPS_ObjectInfo COMPS_ValGenResult_ObjInfo;
extern COMPS_ObjectInfo COMPS_ValOkResult_ObjInfo;
extern COMPS_ObjectInfo COMPS_ValErrResult_ObjInfo;
extern COMPS_ObjectInfo COMPS_ValErr_ObjInfo;
extern COMPS_ValErr COMPS_ValNoErr;
#endif
