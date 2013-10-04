#include "comps_validate.h"

inline signed char comps_validate_prop_exists(COMPS_Object * prop) {
    if (!prop)
        return 0;
    else return 1;
}

inline signed char comps_validate_strprop_notempty(COMPS_Object * prop) {
    if (!strlen(((COMPS_Str*)prop)->val))
        return 0;
    else return 1;
}


