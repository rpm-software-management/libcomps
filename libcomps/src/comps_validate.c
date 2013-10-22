#include "comps_validate.h"

signed char comps_validate_rule_prop_check(COMPS_ValRuleGeneric *rule,
                                           COMPS_Object *obj) {
    #define _rule_ ((COMPS_ValRuleProp*)rule)
    COMPS_Object *prop;
    signed char ret;

    prop = _rule_->get_f(obj);
    ret = _rule_->check_f(obj, prop);
    COMPS_OBJECT_DESTROY(prop);
    return ret;

    #undef _rule_
}

signed char comps_validate_execute(COMPS_Object *obj,
                                   COMPS_ValRuleGeneric *rules[]) {
    int i;
    for (i = 0; rules[i]; i++) {
        if (!rules[i]->rule_check(rules[i], obj)) {
            return 0;
        }
    }
    return 1;
}
