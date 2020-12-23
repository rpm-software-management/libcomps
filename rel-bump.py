#!/bin/env python3
import json
import sys
import string

if __name__ == "__main__":
    f = open("version.json", "r")
    version = json.load(f)
    f.close()
    version["libcomps_RELEASE"] += 1

    f = open("version.json", "w")
    json.dump(version, f, indent=4)
    f.close()

    f = open("libcomps/version.cmake.in", "r")
    version_in = f.read()
    f.close()

    version_out = string.Template(version_in).substitute(version)
    f = open("libcomps/version.cmake", "w")
    f.write(version_out)
    f.close()

