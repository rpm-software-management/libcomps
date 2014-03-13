#include "comps_default.h"
#include "comps_docpackage.h"

COMPS_DefaultsOptions COMPS_DDefaultsOptions = {
    .default_biarchonly = false,
    .default_uservisible = true,
    .default_default = false,
    .default_pkgtype = COMPS_PACKAGE_MANDATORY
};

char* comps_default_doctype_name = "comps";
char* comps_default_doctype_sysid = "comps.dtd";
char* comps_default_doctype_pubid = "-//Red Hat, Inc.//DTD Comps info//EN";

