#ifndef COMPS_DEFAULT_H
#define COMPS_DEFAULT_H

#include <stdbool.h>

#include "comps_obj.h"

typedef struct COMPS_DefaultsOptions {
    bool default_uservisible;
    bool default_biarchonly;
    bool default_default;
    int default_pkgtype;
} COMPS_DefaultsOptions;

extern COMPS_DefaultsOptions COMPS_DDefaultsOptions;
extern char* comps_default_doctype_name;
extern char* comps_default_doctype_pubid;
extern char* comps_default_doctype_sysid;

#endif
