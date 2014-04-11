#!/bin/env python
import subprocess
import sys
import pprint
import string
import json
from datetime import date

def is_commit_tag(commit):
    p = subprocess.Popen(['git', 'describe',
                          '--tags', "--exact-match", "%s"%commit],
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()
    if p.returncode is 0:
        return p.stdout.readline().strip()
    else:
        return None

def tag_to_commit(tag):
    p = subprocess.Popen(['git', 'rev-parse', "%s^{commit}"%tag],
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()
    if p.returncode:
        return None
    x = p.stdout.readline().strip()
    return x

def commits_date(commits):
    dates = []
    for c in commits:
        p = subprocess.Popen(['git', 'log', '-1', "--format=%at", "%s"%c],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        p.wait()
        dates.append(int(p.stdout.read()))
    return dates

def git_tags_chrono():
    p = subprocess.Popen(['git', 'tag'], stdout=subprocess.PIPE)
    p.wait()
    if p.returncode:
        return None
    tags = [x.strip() for x in p.stdout]
    dates = [commits_date([t]) for t in tags]
    return [z[0] for z in sorted(zip(tags, dates), key=lambda x: x[1])]

def git_tags():
    p = subprocess.Popen(['git', 'tag'], stdout=subprocess.PIPE)
    if p.returncode:
        return None
    tags = [x.strip() for x in p.stdout]
    return tags

def commits_for_tag(tags, tag):
    index = tags.index(tag)
    #print index
    if index == 0:
        prev = None
    else:
        prev = tags[index-1]
        prev = tag_to_commit(prev)
    tag = tag_to_commit(tag)
    #print prev

    commits = []
    p = subprocess.Popen(['git', 'rev-list', '--all'], stdout=subprocess.PIPE)
    start = False
    for x in p.stdout:
        x = x.strip()
        #print x,tag
        if not start and x == tag:
            start = True
            #print "start true"
        if start:
            commits.append(x)
        if x == prev:
            break
    return commits

def log_for_commits(commits, _format=None):
    log = []
    if not _format:
        _args = ['git', 'log', '-1']
    else:
        _args = ['git', 'log', '-1', '--format=%s'%_format]
    for x in commits:
        #print x
        p = subprocess.Popen(_args + [x], stdout=subprocess.PIPE)
        log.append([x.rstrip("\n") for x in p.stdout])
    return log

def format_chlog_msg(msg):
    msg = filter(lambda x: x != "", msg)
    for x in range(0, len(msg)):
        if not msg[x].startswith("- "):
            msg[x] = "- "+msg[x]
    return msg

def build_chlog(tags ,top='HEAD'):
    f = open("chcommits", "r")
    chcommits = set([x.strip() for x in f])
    f.close()
    
    log = []
    for tag in tags:
        head = log_for_commits([tag], _format="%ct%n%cn <%ce>%n%B")
        head = ["*"]+head[0]
        head_body = head[3:]
        head = head[:3]
        head[1] = date.fromtimestamp(int(head[1])).strftime("%a %b %d %Y")
        head.append("-".join(tag.split("-")[1:]))
        #print head

        commits = commits_for_tag(tags, tag)
        loc_chcommits = list(chcommits & set(commits))
        loc_log = log_for_commits(loc_chcommits, _format="%B")
        _log = [" ".join(head)]
        _log.append("\n".join(format_chlog_msg(head_body)))
        for x in loc_log:
            _log.append("\n".join(format_chlog_msg(x)))
        _log.append("")
        #print _log

        log.append("\n".join(_log))
    return reversed(log)

if __name__ == "__main__":
    vfp = open("version.json", "r")
    version = json.load(vfp)
    vfp.close()
    
    subs = {}
    try:
        top_commit = tag_to_commit(sys.argv[1])
        subs["GITREVLONG"] = tag_to_commit(sys.arv[1])
    except IndexError:
        top_commit = tag_to_commit("HEAD")
        subs["GITREVLONG"] = tag_to_commit(top_commit)



    subs.update(version)
    tags = git_tags_chrono()
    subs["CHANGELOG"] = "\n".join(build_chlog(tags, top_commit))

    tag = is_commit_tag(top_commit)
    if not tag:
        subs["SOURCE_URL_PATH"] = "archive/%{commit}/libcomps-%{commit}.tar.gz"
        archive_name = "libcomps-%s.tar.gz"%(top_commit,)
    else:
        subs["SOURCE_URL_PATH"] = tags[-1]+".tar.gz"
        archive_name = "%s.tar.gz"%(tag,)

    subs["VERSION"] = "%s.%s.%s" % (subs["libcomps_VERSION_MAJOR"],
                                    subs["libcomps_VERSION_MINOR"],
                                    subs["libcomps_VERSION_PATCH"])
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
