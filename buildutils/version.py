import struct
try:
    import ConfigParser
except ImportError:
    import configparser

def version_load(fname):
    cfp = ConfigParser.RawConfigParser()
    cfp.read(fname)
    major = cfp.getint("version", "major")
    minor = cfp.getint("version", "minor")
    patch = cfp.getint("version", "patch")
    release = cfp.getint("version", "release")
    return {"MAJOR": major, "MINOR": minor, "PATCH": patch, "RELEASE": release}

def version_store(fname, version):
    cfp = ConfigParser.RawConfigParser()
    cfp.add_section("version")
    cfp.set("version", "major", version["MAJOR"])
    cfp.set("version", "minor", version["MINOR"])
    cfp.set("version", "patch", version["PATCH"])
    cfp.set("version", "release", version["RELEASE"])
    f = open(fname, "w")
    cfp.write(f)
    f.close()

def version_str(version):
    return "%d.%d.%d-%d" % (version["MAJOR"], version["MINOR"],
                            version["PATCH"], version["RELEASE"])

def version_major_bump(version):
    version["MAJOR"] += 1

def version_minor_bump(version):
    version["MINOR"] += 1

def version_patch_bump(version):
    version["PATCH"] += 1

def version_relase_bump(version):
    version["RELEASE"] += 1
