#!/bin/env python3
import sys

if __name__ == "__main__":
    with open("VERSION", "r", encoding="utf-8") as f:
        version = f.read().strip()

    parts = [int(x) for x in version.split(".")]
    major, minor, patch = parts[0], parts[1], parts[2]

    try:
        if sys.argv[1] == "major":
            major += 1
            minor = 0
            patch = 0
        elif sys.argv[1] == "minor":
            minor += 1
            patch = 0
        elif sys.argv[1] == "patch":
            patch += 1
    except IndexError:
        patch += 1

    new_version = f"{major}.{minor}.{patch}"
    with open("VERSION", "w", encoding="utf-8") as f:
        f.write(new_version + "\n")
