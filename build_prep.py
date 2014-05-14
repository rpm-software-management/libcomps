#!/bin/env python
import subprocess
import sys
#import pprint
import string
#import json
#from datetime import date
import argparse

from buildutils.version import *
import buildutils.chlog as chlog

if __name__ == "__main__":
    version = version_load("version.cfg")

    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--stash", action="store_true",
                        help="make stash archive instead of regular commit")
    parser.add_argument("commit", type=str, default="HEAD", nargs="?",
                                  help="commit hash or tag")
    args = parser.parse_args()

    subs = {}
    if not args.stash:
        try:
            top_commit = chlog.tag_to_commit(sys.argv[1])
            subs["GITREVLONG"] = chlog.tag_to_commit(sys.argv[1])
        except IndexError:
            top_commit = chlog.tag_to_commit("HEAD")
            subs["GITREVLONG"] = chlog.tag_to_commit(top_commit)
    else:
        p = subprocess.Popen(['git', 'stash', 'create'],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        p.wait()
        stashid = p.stdout.readline().strip()
        if not stashid:
            print "No local changes. Use build_prep without --stash parameter"
            sys.exit(1)
        top_commit = stashid
        subs["GITREVLONG"] = chlog.tag_to_commit(stashid)

    subs["libcomps_RELEASE"] = version["RELEASE"]
    #subs.update(version)
    tags = chlog.git_tags_chrono()
    subs["CHANGELOG"] = "\n".join(chlog.build_chlog(tags, top_commit))

    tag = chlog.is_commit_tag(top_commit)
    if not tag:
        subs["SOURCE_URL_PATH"] = "archive/%{commit}/libcomps-%{commit}.tar.gz"
        archive_name = "libcomps-%s.tar.gz"%(top_commit,)
    else:
        subs["SOURCE_URL_PATH"] = tags[-1]+".tar.gz"
        archive_name = "%s.tar.gz"%(tag,)

    subs["VERSION"] = "%s.%s.%s" % (version["MAJOR"],
                                    version["MINOR"],
                                    version["PATCH"])
    spec = open("libcomps.spec.in", "r")
    specstr_in = spec.read()
    spec.close()
    specstr_out = string.Template(specstr_in).safe_substitute(subs)
    spec = open("libcomps.spec", "w")
    spec.write(specstr_out)
    spec.close()

    p = subprocess.Popen(['git', 'archive', top_commit, "--format=tar.gz",
                          "--prefix=libcomps-%s/"%(top_commit,), "-o",
                          archive_name])#,
                         #stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()
