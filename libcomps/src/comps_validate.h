#ifndef COMPS_VALIDATE_H
#define COMPS_VALIDATE_H

#include "comps_obj.h"
#include "comps_utils.h"

#define H_COMPS_VAL_RULE_GENERIC\
    COMPS_ValGenResult* (*rule_check)(COMPS_ValRuleGeneric*, COMPS_Object*);\
    char *verbose_msg

#define H_COMPS_VAL_RESULT COMPS_Object_HEAD

#define COMPS_VAL_RULE_PROP .rule_check = &comps_validate_rule_prop_check
#define COMPS_VAL_RULE_LIST .rule_check = &comps_validate_rule_list_check
#define COMPS_VAL_RULE_LIST2 .rule_check = &comps_validate_rule_list_check2
#define COMPS_VAL_GETF COMPS_Object*(*)(COMPS_Object*)

typedef struct COMPS_ValRuleGeneric COMPS_ValRuleGeneric;
typedef struct COMPS_ValGenResult COMPS_ValGenResult;

struct COMPS_ValRuleGeneric {
    H_COMPS_VAL_RULE_GENERIC;
};

typedef struct COMPS_ValRuleProp {
    H_COMPS_VAL_RULE_GENERIC;
    COMPS_Object* (*get_f)(COMPS_Object*);
    COMPS_ValGenResult* (*check_f)(COMPS_Object*, COMPS_Object*);
} COMPS_ValRuleProp;

typedef struct COMPS_ValRuleList {
    H_COMPS_VAL_RULE_GENERIC;
    size_t offset;
    COMPS_ValGenResult* (*check_f)(COMPS_Object*, COMPS_Object*);
} COMPS_ValRuleList;

typedef struct COMPS_ValRuleList2 {
    H_COMPS_VAL_RULE_GENERIC;
    COMPS_Object* (*get_f)(COMPS_Object*);
    COMPS_ValGenResult* (*check_f)(COMPS_Object*, COMPS_Object*);
} COMPS_ValRuleList2;

struct COMPS_ValGenResult {
    H_COMPS_VAL_RESULT;
};
COMPS_Object_TAIL(COMPS_ValGenResult);

typedef struct COMPS_ValOkResult {
    H_COMPS_VAL_RESULT;
} COMPS_ValOkResult;
COMPS_Object_TAIL(COMPS_ValOkResult);

typedef struct COMPS_ValErrResult {
    H_COMPS_VAL_RESULT;
    COMPS_ObjList *err_list;
} COMPS_ValErrResult;
COMPS_Object_TAIL(COMPS_ValErrResult);

typedef struct COMPS_ValErr {
    COMPS_Object_HEAD;
    COMPS_Object *err_object;
    char *err_msg;
} COMPS_ValErr;
COMPS_Object_TAIL(COMPS_ValErr);


COMPS_ValGenResult* comps_validate_rule_prop_check(COMPS_ValRuleGeneric *rule,
                                                   COMPS_Object *obj);
COMPS_ValGenResult* comps_validate_rule_list_check(COMPS_ValRuleGeneric *rule,
                                                   COMPS_Object *obj);
COMPS_ValGenResult* comps_validate_rule_list_check2(COMPS_ValRuleGeneric *rule,
                                                   COMPS_Object *obj);
COMPS_ValGenResult* comps_validate_execute(COMPS_Object *obj,
                                   COMPS_ValRuleGeneric *rules[]);

COMPS_ValGenResult* comps_empty_check(COMPS_Object *obj, COMPS_Object *prop);
COMPS_ValGenResult* comps_objlist_unique_check(COMPS_Object *object,
                                               COMPS_Object *objlist);
void comps_valgenres_print(COMPS_ValGenResult *res, FILE *stream);
void comps_valgenres_concat(COMPS_ValGenResult **res1,
                                     COMPS_ValGenResult *res2);
void comps_valgenres_prefix(COMPS_ValGenResult *res, const char *prefix);

//extern COMPS_ObjectInfo COMPS_ValGenResult_ObjInfo;
//extern COMPS_ObjectInfo COMPS_ValOkResult_ObjInfo;
//extern COMPS_ObjectInfo COMPS_ValErrResult_ObjInfo;
//extern COMPS_ObjectInfo COMPS_ValErr_ObjInfo;
#endif
