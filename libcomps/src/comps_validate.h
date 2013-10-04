#ifndef COMPS_VALIDATE_H
#define COMPS_VALIDATE_H

#include "comps_obj.h"

#define H_COMPS_VAL_RULE_GENERIC\
    signed char (*rule_check)(COMPS_ValRuleGeneric*, COMPS_Object*)

#define H_COMPS_VAL_RULE_PROP .rule_check = &comps_validate_rule_prop_check

#define COMPS_VAL_RULE_PROP

typedef struct COMPS_ValRuleGeneric COMPS_ValRuleGeneric;

struct COMPS_ValRuleGeneric {
    H_COMPS_VAL_RULE_GENERIC;
};

typedef struct COMPS_ValRuleProp {
    H_COMPS_VAL_RULE_GENERIC;
    COMPS_Object* (*get_f)(COMPS_Object);
    signed char (*check_f)(COMPS_Object*, COMPS_Object*);
} COMPS_ValRuleProp;

signed char comps_validate_exists(COMPS_Object * prop);
signed char comps_validate_strnotempty(COMPS_Object * prop);


#endif
