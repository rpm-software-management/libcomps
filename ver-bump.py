#!/bin/env python3
import json
import sys
import string

if __name__ == "__main__":
    with open("version.json", "r", encoding="utf-8") as f:
        version = json.load(f)
    try:
        if sys.argv[1] == "major":
            version["libcomps_VERSION_MAJOR"] += 1
            version["libcomps_VERSION_MINOR"] = 1
            version["libcomps_VERSION_PATCH"] = 1
            version["libcomps_RELEASE"] = 1
        elif sys.argv[1] == "minor":
            version["libcomps_VERSION_MINOR"] += 1
            version["libcomps_VERSION_PATCH"] = 1
            version["libcomps_RELEASE"] = 1
        elif sys.argv[1] == "patch":
            version["libcomps_VERSION_PATCH"] += 1
            version["libcomps_RELEASE"] = 1
    except IndexError:
        version["libcomps_VERSION_PATCH"] += 1
        version["libcomps_RELEASE"] = 1

    with open("version.json", "w", encoding="utf-8") as f:
        json.dump(version, f, indent=4)

    with open("libcomps/version.cmake.in", "r", encoding="utf-8") as f:
        version_in = f.read()

    version_out = string.Template(version_in).substitute(version)
    with open("libcomps/version.cmake", "w", encoding="utf-8") as f:
        f.write(version_out)
