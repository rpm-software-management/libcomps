#!/bin/env python3

if __name__ == "__main__":
    with open("VERSION", "r", encoding="utf-8") as f:
        version = f.read().strip()

    major, minor, release = version.split(".")
    release = str(int(release) + 1)

    with open("VERSION", "w", encoding="utf-8") as f:
        f.write(f"{major}.{minor}.{release}\n")
