#ifndef COMPS_DEFAULT_H
#define COMPS_DEFAULT_H

#include <stdbool.h>

typedef struct COMPS_DefaultsOptions {
    bool default_uservisible;
    bool default_biarchonly;
    bool default_default;
    int default_pkgtype;
} COMPS_DefaultsOptions;

extern COMPS_DefaultsOptions COMPS_DDefaultsOptions;

#endif
