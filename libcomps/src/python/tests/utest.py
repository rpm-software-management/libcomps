# -*- coding: UTF-8 -*-

# libcomps - C alternative to yum.comps library
# Copyright (C) 2013 Jindrich Luza
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to  Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
# USA

import unittest
import traceback
def assertSequenceEqual(seq1, seq2):
    if len(seq1) != len(seq2):
        raise (AssertionError, "%s != %s" % (seq1, seq2))
    for x,y in zip(seq1,seq2):
        if x != y:
            raise (AssertionError, "%s != %s" % (seq1, seq2))
        

class MyResult(unittest.TestResult):
    WHITEBOLD = {"1":(1,),  "2":(0,)}
    WHITE = {"1": (38, 5, 15), "2": (38, 5, 7)}
    DRKRED = {"1": (48, 5, 9), "2": (48, 5, 0)}
    RED = {"1": (38, 5, 1), "2":(38, 5, 7)}
    GREEN = {"1": (38, 5, 10), "2":(38, 5, 7)}
    YELLOW = {"1": (38, 5, 11), "2":(38, 5, 7)}
    separator1 = "-" * 70
    separator2 = "-" * 70

    def colored(self, color, string):
        return '\x1b[%sm%s\x1b[m' % (";".join(map(str, color["1"])), string)

    def __init__(self, stream, descriptions, verbosity):
        self.stream = stream
        try:
        	super(MyResult, self).__init__(stream, descriptions, verbosity)
        except TypeError:
        	super(MyResult, self).__init__()
        self.last_cls = None
        self.fail = False

    def startTest(self, test):
        if self.last_cls != test.__class__:
            self.stream.write(
                self.colored(self.WHITEBOLD,
                             "[ Starting %s ]\n"%str(test.__class__.__name__)))
            self.last_cls = test.__class__

        self.stream.write(str(test))
        self.stream.write(" ... ")

    def startTestRun(self):
        pass

    def stopTest(self, test):
        #print "done test:", test
        pass

    def addSuccess(self, test):
        self.stream.write(self.colored(self.GREEN,"[OK]\n"))
        self.testsRun += 1

    def addFailure(self, test, error):
        self.stream.write(self.colored(self.RED,"[FAIL]\n"))
        (_type, value, _traceback) = error
        tb = traceback.format_list([traceback.extract_tb(_traceback,
                                                        limit=2)[-1]])
        excp = traceback.format_exception_only(_type, value)
        self.stream.write("-"*79)
        self.stream.write("\n")
        self.stream.write("%s"%tb[0])
        self.stream.write("%s\n"%excp[0])
        self.stream.write("."*79)
        self.stream.write("\n")
        #traceback.print_exception(_type, value, tb, limit=2, file=self.stream)
        #self.stream.write(str(error))
        self.testsRun += 1
        self.failures += [(test, "".join([tb[0], excp[0]]))]
        self.fail = True

    def addError(self, test, error):
        self.stream.write(self.colored(self.DRKRED,"[ERROR]\n"))
        (_type, value, _traceback) = error
        tb = traceback.format_list([traceback.extract_tb(_traceback,
                                                        limit=2)[-1]])
        excp = traceback.format_exception_only(_type, value)
        self.stream.write("-"*79)
        self.stream.write("\n")
        self.stream.write("%s"%tb[0])
        self.stream.write("%s\n"%excp[0])
        self.stream.write("."*79)
        self.stream.write("\n")
        #traceback.print_exception(_type, value, tb, limit=2, file=self.stream)
        #self.stream.write(str(error))
        self.testsRun += 1
        self.errors += [(test, "".join([tb[0], excp[0]]))]
        self.fail = True

    def addSkip(self, test, reason):
        self.stream.write(self.colored(self.YELLOW,"[SKIPED]\n"))
        self.stream.write(self.colored(self.YELLOW,"reason: %s\n"%reason))
        self.errors += [(test, reason)]
    
    def wasSuccessful(self):
        return not self.fail
    def printErrors(self):
        pass

class MyRunner(unittest.TextTestRunner):
    def _makeResult(self):
        return MyResult(self.stream, self.descriptions, self.verbosity)
