#!/bin/env python3
import json

import distutils.sysconfig as sysconfig
data = {"LIBS": 'python' + sysconfig.get_config_var('LDVERSION'),
        "CPPPATH": sysconfig.get_python_inc(),
        "LIBPATH": sysconfig.get_config_var('LIBDIR')}
print (json.dumps(data))
