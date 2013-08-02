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

import _libpycomps as libcomps
import unittest
import tempfile
import os
import traceback
import inspect

if hasattr(dict, "iteritems"):
    def _iteritems(_dict):
        return _dict.iteritems()
else:
    def _iteritems(_dict):
        return _dict.items()

class MyResult(unittest.TestResult):
    WHITEBOLD = {"1":(1,),  "2":(0,)}
    WHITE = {"1": (38, 5, 15), "2": (38, 5, 7)}
    DRKRED = {"1": (48, 5, 9), "2": (48, 5, 0)}
    RED = {"1": (38, 5, 1), "2":(38, 5, 7)}
    GREEN = {"1": (38, 5, 10), "2":(38, 5, 7)}
    YELLOW = {"1": (38, 5, 11), "2":(38, 5, 7)}
    def colored(self, color, string):
        return '\x1b[%sm%s\x1b[%sm' % (";".join(map(str, color["1"])), string,
                                       ";".join(map(str, color["2"])))

    def __init__(self, stream, descriptions, verbosity):
        self.stream = stream
        super(MyResult, self).__init__(stream, descriptions, verbosity)
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

class MyRunner(unittest.TextTestRunner):
    def _makeResult(self):
        return MyResult(self.stream, self.descriptions, self.verbosity)

class BaseObjTestClass(object):
    obj_data = []
    obj_getset = {}
    obj_type = None

    dict1 = libcomps.Dict()
    dict1["foo"] = "bar"
    dict1["Tom"] = "Jerry"
    dict1["linux"] = "rolls!"

    idlist1 = libcomps.IdList()
    idlist1.append("id1")
    idlist1.append("007")
    idlist1.append("x")

    obj_types = {str: ["some string 1", "test string", "another str",
                       "short string", "ss", "long string string string",
                       "ls"],
                 int: [-1000,2,0,1000],
                 libcomps.Dict: [dict1],
                 libcomps.IdList: [idlist1],
                 bool: [True, False]}
    def obj_constructor(self, *args, **kwargs):
        raise NotImplemented
    def setup(self):
        raise NotImplemented

    def test_create(self):
        obj = self.obj_constructor(**self.obj_data[0])
        obj = self.obj_type()

    #@unittest.skip("")
    def test_getset(self):
        obj = self.obj_constructor(**self.obj_data[0])
        data_desc = []
        for x in inspect.getmembers(obj.__class__):
            if (inspect.isdatadescriptor(x[1]) or\
                inspect.isgetsetdescriptor(x[1]) or\
               inspect.ismemberdescriptor(x[1])) and not x[0].startswith("__"):
                data_desc.append(x)
        print(data_desc)
        for attr in data_desc:
            z = getattr(obj, x[0])
            attr_types = self.obj_getset[attr[0]]
            for _type, vals in _iteritems(self.obj_types):
                if _type not in attr_types:
                    print (attr[0], vals[0])
                    with self.assertRaises(TypeError):
                        setattr(obj, attr[0], vals[0])
                else:
                    for val in vals:
                        setattr(obj, attr[0], val)

            with self.assertRaises(TypeError):
                self.assertTrue(delattr(obj, x[0]), x[0])

    def test_dictmembers(self):
        obj = self.obj_constructor(**self.obj_data[0])
        for member in self.obj_dict_members:
            _dict = getattr(obj, member)
            _zipped = list(zip(self.obj_types[str][0::2],
                          self.obj_types[str][1::2]))+\
                      list(zip(self.obj_types[str][1::2],
                          self.obj_types[str][0::2]))
            for x,y in _zipped:
                _dict[x] = y
            for x,y in _zipped:
                self.assertTrue(_dict[x] == y)

    def test_listmembers(self):
        obj = self.obj_constructor(**self.obj_data[0])
        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj, member)
            for x in v:
                _list.append(x)
            for pos, val in zip(range(0, len(v)), v):
                self.assertTrue(_list[pos] == val)

    def __union_prep1(self):
        obj1 = self.obj_constructor(**self.obj_data[0])
        for member in self.obj_dict_members:
            _dict = getattr(obj1, member)
            _zipped = list(zip(self.obj_types[str][0:4:2],
                          self.obj_types[str][1:5:2]))+\
                      list(zip(self.obj_types[str][1:5:2],
                          self.obj_types[str][0:4:2]))
            for x,y in _zipped:
                _dict[x] = y

        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj1, member)
            for x in v[0:4]:
                _list.append(x)
        return obj1

    def __union_prep2(self):
        obj1 = self.obj_constructor(**self.obj_data[1])
        for member in self.obj_dict_members:
            _dict = getattr(obj1, member)
            _zipped = list(zip(self.obj_types[str][2::2],
                          self.obj_types[str][3::2]))+\
                      list(zip(self.obj_types[str][3::2],
                          self.obj_types[str][2::2]))
            for x,y in _zipped:
                _dict[x] = y
        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj1, member)
            for x in v[4:]:
                _list.append(x)
        return obj1


    def test_union1(self):
        obj1 = self.__union_prep1()
        obj2 = self.obj_constructor(**self.obj_data[1])
        obj = obj1 + obj2

        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj, member)
            self.assertTrue(len(_list) == 4)

        for member, v in _iteritems(self.obj_dict_members):
            _d1 = {k: v for k,v in _iteritems(getattr(obj1, member))}
            _d2 = {k: v for k,v in _iteritems(getattr(obj2, member))}
            _d1.update(_d2)
            _dict = getattr(obj, member)
            _d = {k:v for k,v in _iteritems(_dict)}
            self.assertTrue(_d == _d1)

    def test_union2(self):
        obj1 = self.__union_prep1()
        obj2 = self.obj_constructor(**self.obj_data[1])
        obj = obj1 + obj2

        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj, member)
            self.assertTrue(len(_list) == 4)

        for member, v in _iteritems(self.obj_dict_members):
            _d1 = {k: v for k,v in _iteritems(getattr(obj1, member))}
            _d2 = {k: v for k,v in _iteritems(getattr(obj2, member))}
            _d1.update(_d2)
            _dict = getattr(obj, member)
            _d = {k:v for k,v in _iteritems(_dict)}
            self.assertTrue(_d == _d1)

    def test_union3(self):
        obj1 = self.__union_prep1()
        obj2 = self.__union_prep2()
        obj = obj1 + obj2

        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj, member)
            self.assertTrue(len(_list) == len(v))

        for member, v in _iteritems(self.obj_dict_members):
            _d1 = {k: v for k,v in _iteritems(getattr(obj1, member))}
            _d2 = {k: v for k,v in _iteritems(getattr(obj2, member))}
            _d1.update(_d2)
            _dict = getattr(obj, member)
            _d = {k:v for k,v in _iteritems(_dict)}
            self.assertTrue(_d == _d1)

#@unittest.skip(" ")
class Category_Test(BaseObjTestClass, unittest.TestCase):
    obj_type = libcomps.Category
    obj_data = [{"id":"cat1", "name": "category1", "desc": "cat desc 1",
                 "display_order": 1},
                {"id":"cat2", "name": "category2", "desc": "cat desc 2",
                 "display_order": 2},
                {"id":"cat3", "name": "category3", "desc": "cat desc 3",
                 "display_order": 3},
                {"id":"cat4", "name": "category4", "desc": "cat desc 4",
                 "display_order": 4}]
    obj_getset = {"id": [str],
                  "name": [str],
                  "desc": [str],
                  "display_order": [int, bool],
                  "group_ids": [libcomps.IdList],
                  "name_by_lang": [libcomps.Dict],
                  "desc_by_lang": [libcomps.Dict]}
    obj_dict_members = {"name_by_lang":{"cs": "administrace",
                                        "en": "administration",
                                        "de": "Verwaltung",
                                        "it": "Amministrazione",
                                        "fr": "administration",
                                        "fi": "hallinto",
                                        "no": "administrasjon"},
                        "desc_by_lang":{"cs": "administrace systemu",
                                        "en": "system administration",
                                        "de": "Systemadministration",
                                        "it": "amministrazione del sistema",
                                        "fr": "administration système",
                                        "fi": "-järjestelmä hallinto",
                                        "no": "systemadministrasjon"}}
    obj_list_members = {"group_ids":["g1", "g2", "g3", "g4", "g5", "g6"]}

    def obj_constructor(self, *args, **kwargs):
        return libcomps.Category(*args, **kwargs)

#@unittest.skip(" ")
class Group_Test(BaseObjTestClass, unittest.TestCase):
    obj_type = libcomps.Group
    obj_data = [{"id":"g1", "name": "group1", "desc": "group desc 1",
                 "default": False, "uservisible": True, "display_order": 0,
                 "langonly": "en"},
                {"id":"g2", "name": "group2", "desc": "group desc 2",
                 "default": False, "uservisible": True, "display_order": 0,
                 "langonly": "en"},
                {"id":"g3", "name": "group3", "desc": "group desc 3",
                 "default": False, "uservisible": True, "display_order": 0,
                 "langonly": "en"},
                {"id":"g4", "name": "group4", "desc": "group desc 4",
                 "default": False, "uservisible": True, "display_order": 0,
                 "langonly": "en"}]
    obj_getset = {"id": [str],
                  "name": [str],
                  "desc": [str],
                  "uservisible": [bool],
                  "default": [bool],
                  "lang_only": [str],
                  "display_order": [int, bool],
                  "packages": [libcomps.PackageList],
                  "name_by_lang": [libcomps.Dict],
                  "desc_by_lang": [libcomps.Dict]
                  }
    obj_dict_members = {"name_by_lang":{"cs": "administrace",
                                        "en": "administration",
                                        "de": "Verwaltung",
                                        "it": "Amministrazione",
                                        "fr": "administration",
                                        "fi": "hallinto",
                                        "no": "administrasjon"},
                        "desc_by_lang":{"cs": "administrace systemu",
                                        "en": "system administration",
                                        "de": "Systemadministration",
                                        "it": "amministrazione del sistema",
                                        "fr": "administration système",
                                        "fi": "-järjestelmä hallinto",
                                        "no": "systemadministrasjon"}}
    obj_list_members = {"packages":[
                        libcomps.Package("oss", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("alsa", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("pulse", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("port", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("jack", libcomps.PACKAGE_TYPE_DEFAULT)\
                        ]}

    def obj_constructor(self, *args, **kwargs):
        return libcomps.Group(*args, **kwargs)

#@unittest.skip(" ")
class Env_Test(BaseObjTestClass, unittest.TestCase):
    obj_type = libcomps.Environment
    obj_data = [{"id":"e1", "name": "env1", "desc": "env desc 1",
                 "display_order": 0},
                {"id":"e2", "name": "env2", "desc": "env desc 2",
                 "display_order": 1},
                {"id":"e3", "name": "env3", "desc": "env desc 3",
                 "display_order": 2},
                {"id":"e4", "name": "env4", "desc": "env desc 4",
                 "display_order": 3}]
    obj_getset = {"id": [str],
                  "name": [str],
                  "desc": [str],
                  "display_order": [int, bool],
                  "option_ids": [libcomps.IdList],
                  "group_ids": [libcomps.IdList],
                  "name_by_lang": [libcomps.Dict],
                  "desc_by_lang": [libcomps.Dict]
                  }
    obj_dict_members = {"name_by_lang":{"cs": "administrace",
                                        "en": "administration",
                                        "de": "Verwaltung",
                                        "it": "Amministrazione",
                                        "fr": "administration",
                                        "fi": "hallinto",
                                        "no": "administrasjon"},
                        "desc_by_lang":{"cs": "administrace systemu",
                                        "en": "system administration",
                                        "de": "Systemadministration",
                                        "it": "amministrazione del sistema",
                                        "fr": "administration système",
                                        "fi": "-järjestelmä hallinto",
                                        "no": "systemadministrasjon"}}
    obj_list_members = {"group_ids":["g1", "g2", "g3", "g4", "g5", "g6"],
                        "option_ids":["g1", "g2", "g3", "g4", "g5", "g6"]}

    def obj_constructor(self, *args, **kwargs):
        return libcomps.Environment(*args, **kwargs)

class BaseListTestClass(object):
    list_type = None
    item_type = None
    items_data = []
    items_extra_data = []

    dict1 = libcomps.Dict()
    dict1["foo"] = "bar"
    dict1["Tom"] = "Jerry"
    dict1["linux"] = "rolls!"

    dict2 = libcomps.Dict()
    dict2["red"] = "black"
    dict2["proton"] = "electron"
    dict2["fire"] = "water"

    def test_basic(self):
        listobj = self.list_type()
        item = self.item_type(**self.items_data[0])
        listobj.append(item)
        self.assertTrue(listobj[0] == item)
        self.assertTrue(len(listobj) == 1)
        del listobj[0]
        self.assertTrue(len(listobj) == 0)
        with self.assertRaises(IndexError):
            listobj[0]
        listobj.append(item)
        self.assertTrue(listobj[0] == item)
        self.assertTrue(len(listobj) == 1)
        with self.assertRaises(IndexError):
            listobj[1]

    def test_append(self):
        listobj = self.list_type()
        for x in self.items_data:
            listobj.append(self.item_type(**x))
        for x in range(0, len(self.items_data)):
            self.assertTrue(listobj[x] == self.item_type(**self.items_data[x]))

    def test_slice(self):
        listobj = self.list_type()
        for x in self.items_data:
            listobj.append(self.item_type(**x))

        sublist = listobj[0:3]
        self.assertTrue(len(sublist) == 3)
        for x,y in zip(sublist, listobj):
            self.assertTrue(x == y)
        sublist = listobj[3:6]
        self.assertTrue(len(sublist) == 3)
        for x,y in zip(range(0,3),range(3,6)):
            self.assertTrue(sublist[x] == listobj[y])
        sublist = listobj[len(self.items_data): len(self.items_data)+2]
        self.assertTrue(len(sublist) == 0)
        sublist = listobj[len(self.items_data)-2: len(self.items_data)+2]
        self.assertTrue(len(sublist) == 2)

        sublist = listobj[0:6:2]
        self.assertTrue(len(sublist) == 3)
        for x,y in zip(range(0,3), range(0,6,2)):
            self.assertTrue(sublist[x] == listobj[y])

    def test_clear(self):
        listobj = self.list_type()
        for x in self.items_data:
            listobj.append(self.item_type(**x))
        listobj.clear()
        self.assertTrue(len(listobj) == 0)
        with self.assertRaises(IndexError):
            listobj[0]
        for x in self.items_data:
            listobj.append(self.item_type(**x))
        self.assertTrue(len(listobj) == len(self.items_data))

    def test_concat(self):
        listobj1 = self.list_type()
        for x in self.items_data[0:4]:
            listobj1.append(self.item_type(**x))
            for k,vals in _iteritems(self.items_extra_data):
                for val in vals[0:3]:
                    self.items_extra_data_setter[k](getattr(listobj1[-1], k),
                                                    val)
        listobj2 = self.list_type()
        listobj = listobj1 + listobj2
        self.assertTrue(listobj == listobj1)
        listobj = listobj2 + listobj1
        self.assertTrue(listobj == listobj1)

        return

        listobj2 = self.list_type()
        for x in self.items_data[3:]:
            listobj2.append(self.item_type(**x))
            for k,vals in _iteritems(self.items_extra_data):
                for val in vals[3:]:
                    self.items_extra_data_setter[k](getattr(listobj2[-1], k),
                                                    val)
        listobj = listobj1 + listobj2
        #print listobj
        self.assertTrue(len(listobj) == len(self.items_data))
        item = listobj[3]
        for k, vals in _iteritems(self.items_extra_data):
            tmpobj = self.item_type()
            for val in vals:
                self.items_extra_data_setter[k](getattr(tmpobj,k), val)
            if type(getattr(item,k)) == libcomps.IdList:
                self.assertTrue(self.items_extra_data_cmp[k](
                                    getattr(item, k),
                                    getattr(tmpobj, k)))

#@unittest.skip(" ")
class CategoryList_Test(unittest.TestCase, BaseListTestClass):
    list_type = libcomps.CategoryList
    item_type = libcomps.Category
    items_data = [{"id":"cat1", "name": "category1", "desc": "cat desc 1",
                 "display_order": 1},
                {"id":"cat2", "name": "category2", "desc": "cat desc 2",
                 "display_order": 2},
                {"id":"cat3", "name": "category3", "desc": "cat desc 3",
                 "display_order": 3},
                {"id":"cat4", "name": "category4", "desc": "cat desc 4",
                 "display_order": 4},
                {"id":"cat5", "name": "category5", "desc": "cat desc 5",
                 "display_order": 5},
                {"id":"cat6", "name": "category6", "desc": "cat desc 6",
                 "display_order": 6},
                {"id":"cat7", "name": "category7", "desc": "cat desc 7",
                 "display_order": 7}]
    items_extra_data_cmp = {"group_ids": lambda x,y: set(x) == set(y),
                            "name_by_lang": lambda x,y: x == y}
    items_extra_data_setter = {"group_ids": libcomps.IdList.append,
                               "name_by_lang": lambda x,y: x.__setitem__(y[0], y[1])}
    items_extra_data = {"group_ids": ["g1", "g2", "g3", "g4", "g5", "g6"],
                        "name_by_lang": [("foo", "bar"), ("red", "black"),
                                         ("fire", "water")]}

#@unittest.skip(" ")
class GroupList_Test(unittest.TestCase, BaseListTestClass):
    list_type = libcomps.GroupList
    item_type = libcomps.Group
    items_data = [{"id":"g1", "name": "group1", "desc": "group desc 1",
                 "display_order": 1, "default": False},
                {"id":"g2", "name": "group2", "desc": "group desc 2",
                 "display_order": 2, "default": False},
                {"id":"g3", "name": "group3", "desc": "group desc 3",
                 "display_order": 2, "default": False},
                {"id":"g4", "name": "group4", "desc": "group desc 4",
                 "display_order": 2, "default": False},
                {"id":"g5", "name": "group5", "desc": "group desc 5",
                 "display_order": 2, "default": False},
                {"id":"g6", "name": "group6", "desc": "group desc 6",
                 "display_order": 2, "default": False},
                {"id":"g7", "name": "group7", "desc": "group desc 7",
                 "display_order": 2, "default": False}]

    items_extra_data_cmp = {"packages": lambda x,y: set(x) == set(y),
                            "name_by_lang": lambda x,y: x == y}
    items_extra_data_setter = {"packages": libcomps.PackageList.append,
                               "name_by_lang": lambda x,y: x.__setitem__(y[0], y[1])}
    items_extra_data = {"packages": [
                        libcomps.Package("oss", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("alsa", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("pulse", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("port", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("jack", libcomps.PACKAGE_TYPE_DEFAULT),
                        libcomps.Package("cmix", libcomps.PACKAGE_TYPE_DEFAULT)],
                        "name_by_lang": [("foo", "bar"), ("red", "black"),
                                         ("fire", "water")]}

#@unittest.skip(" ")
class EnvList_Test(unittest.TestCase, BaseListTestClass):
    list_type = libcomps.EnvList
    item_type = libcomps.Environment
    items_data = [{"id":"e1", "name": "env1", "desc": "env desc 1",
                 "display_order": 1},
                {"id":"e2", "name": "env2", "desc": "env desc 2",
                 "display_order": 2},
                {"id":"e3", "name": "env3", "desc": "env desc 3",
                 "display_order": 3},
                {"id":"e4", "name": "env4", "desc": "env desc 4",
                 "display_order": 4},
                {"id":"e5", "name": "env5", "desc": "env desc 5",
                 "display_order": 5},
                {"id":"e6", "name": "env6", "desc": "env desc 6",
                 "display_order": 6},
                {"id":"e7", "name": "env7", "desc": "env desc 7",
                 "display_order": 7}]

    items_extra_data_cmp = {"group_ids": lambda x,y: set(x) == set(y),
                            "option_ids": lambda x,y: set(x) == set(y),
                            "name_by_lang": lambda x,y: x == y}
    items_extra_data_setter = {"group_ids": libcomps.IdList.append,
                               "option_ids": libcomps.IdList.append,
                               "name_by_lang": lambda x,y: x.__setitem__(y[0], y[1])}
    items_extra_data = {"group_ids": ["g1", "g2", "g3", "g4", "g5", "g6"],
                        "option_ids": ["g1", "g2", "g3", "g4", "g5", "g6"],
                        "name_by_lang": [("foo", "bar"), ("red", "black"),
                                         ("fire", "water")]}

#@unittest.skip("skip")
class PackageTest(unittest.TestCase):
    def test_attrs(self):
        pkg = libcomps.Package("kernel-3.2", libcomps.PACKAGE_TYPE_MANDATORY)
        self.assertEqual(pkg.name, "kernel-3.2")
        self.assertEqual(pkg.type, libcomps.PACKAGE_TYPE_MANDATORY)

#@unittest.skip("skip")
class DictTest(unittest.TestCase):
    def test_dict(self):
        _dict = libcomps.Dict()
        _dict["cs"] = "Ahoj svete"
        _dict["en"] = "Hello world"
        self.assertTrue(_dict["cs"] == "Ahoj svete")
        self.assertTrue(_dict["en"] == "Hello world")
        self.assertTrue("cs" in _dict)
        self.assertTrue("en" in _dict)
        _keys2 = ["cs", "en"]
        _keys = []
        for x in _dict:
            _keys.append(x)
        self.assertTrue(set(_keys) == set(_keys2))

        _dict2 = libcomps.Dict()
        for (k,w) in _iteritems(_dict):
            _dict2[k] = w
        self.assertTrue(_dict == _dict2)

        _values =  []
        _values2 = ["Ahoj svete", "Hello world"] 
        for v in _dict.itervalues():
            _values.append(v)
        self.assertTrue(set(_values) == set(_values2))

#@unittest.skip("skip")
class COMPSTest(unittest.TestCase):
    def setUp(self):
        self.comps = libcomps.Comps()

    #@unittest.skip("skip")
    def test_xml(self):
        self.comps.groups.append(libcomps.Group("g1", "group1", "group desc",
                                                0, 0, 0, "en"))
        self.comps.groups.append(libcomps.Group("g2", "group2", "group desc",
                                                0, 0, 0, "en"))
        self.comps.groups.append(libcomps.Group("g3", "group3", "group desc",
                                                0, 0, 0, "en"))
        g = self.comps.groups[0]
        g.packages.append(libcomps.Package("kernel", libcomps.PACKAGE_TYPE_MANDATORY))
        g = self.comps.groups[1]
        g.packages.append(libcomps.Package("glibc", libcomps.PACKAGE_TYPE_MANDATORY))
        g = self.comps.groups[2]
        g.packages.append(libcomps.Package("systemd", libcomps.PACKAGE_TYPE_MANDATORY))

        self.comps.categories.append(libcomps.Category("c1", "cat1", "cat desc", 1))
        c = self.comps.categories[0]
        c.group_ids.append("g1")
        self.comps.categories.append(libcomps.Category("c2", "cat2", "cat desc", 1))
        c = self.comps.categories[1]
        c.group_ids.append("g2")
        (h, fname) = tempfile.mkstemp()
        ret = self.comps.xml_f(fname)
        self.assertTrue(not ret)
        #f = open(fname, "r")

        comps2 = libcomps.Comps()
        ret = comps2.fromxml_f(fname)
        self.assertTrue(ret == 0, comps2.get_last_parse_errors())

        compsdoc = comps2.xml_str()
        compsdoc = compsdoc[0:-5] # make some error
        self.assertTrue(len(comps2.groups) == 3)
        self.assertTrue(len(comps2.categories) == 2)
        self.assertTrue(len(comps2.environments) == 0)

        comps3 = libcomps.Comps()
        ret = comps3.fromxml_str(compsdoc)
        #print comps3.get_last_parse_log()
        self.assertTrue(ret ==  -1, "%d %s" %(ret,
                                              comps3.get_last_parse_errors()))
        self.assertTrue(len(comps3.groups) == 3)
        self.assertTrue(len(comps3.categories) == 2)
        self.assertTrue(len(comps3.environments) == 0)
        x = self.comps.xml_str()
        y = comps2.xml_str()
        self.assertTrue(x == y)
        os.remove(fname)

    #@unittest.skip("")
    def test_fedora(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("fedora_comps.xml")
        #for x in comps.get_last_parse_log():
        #    print x
        self.assertTrue(ret != -1)
        comps.xml_f("fed2.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("fed2.xml")
        self.assertTrue(comps == comps2)

    #@unittest.skip("skip")
    def test_sample(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("sample_comps.xml")
        #print comps.get_last_parse_log()
        self.assertTrue(ret != -1)

        comps.xml_f("sample2.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("sample2.xml")
        self.assertTrue(comps == comps2)

    #@unittest.skip("skip")
    def test_main(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("main_comps.xml")
        self.assertTrue(ret != -1)
        comps.xml_f("main2.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("main2.xml")
        x = comps.xml_str()
        y = comps2.xml_str()
        self.assertTrue(comps == comps2)
        self.assertTrue(y == x)
        os.remove("main2.xml")

    #@unittest.skip("skip")
    def test_main2(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("main_comps2.xml")
        self.assertTrue(ret != -1)
        comps.xml_f("main22.xml")
        self.assertTrue(ret != -1)
        comps2 = libcomps.Comps()
        ret = comps2.fromxml_f("main22.xml")
        self.assertTrue(ret != -1)

    #@unittest.skip("skip")
    def test_main_union(self):
        c1 = libcomps.Comps()
        c2 = libcomps.Comps()
        c2.fromxml_f('main_comps.xml')
        c = c1 + c2
        return

        c1 = libcomps.Comps()
        c1.fromxml_f('main_comps.xml')

        c2 = libcomps.Comps()
        c2.fromxml_f('main_comps.xml')

        c = c1 + c2
        x = c.groups[0].packages[0].name
        self.assertTrue(x == "pepper")

    #@unittest.skip("skip")
    def test_main_loc(self):
        comps = libcomps.Comps()
        errors = comps.fromxml_f('main_comps.xml')
        g = comps.groups[0]
        #print(g.desc_by_lang['cs'])

        comps = libcomps.Comps()
        errors = comps.fromxml_f('main_comps.xml')
        g = comps.groups[0]
        g1 = comps.groups[0]
        g.desc_by_lang = g1.desc_by_lang
        dbl1 = g.desc_by_lang
        dbl2 = g1.desc_by_lang
        self.assertTrue(g1.desc_by_lang == g.desc_by_lang)

    #@unittest.skip("skip")
    def test_union(self):
        comps = libcomps.Comps()
        comps.fromxml_f("sample_comps.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("sample2.xml")
        comps3 = comps + comps2
        self.assertTrue(comps == comps3)
        comps.fromxml_f("fedora_comps.xml")
        comps3 = comps + comps2
        self.assertFalse(comps == comps3)
        #print comps3.xml_str()
        gids_set = set()
        cids_set = set()
        eids_set = set()

        gids3_set = set()
        cids3_set = set()
        eids3_set = set()

        for g in comps.groups:
            gids_set.add(g.id)
        for c in comps.categories:
            cids_set.add(c.id)
        for e in comps.environments:
            eids_set.add(e.id)

        for g in comps2.groups:
            gids_set.add(g.id)
        for c in comps2.categories:
            cids_set.add(c.id)
        for e in comps2.environments:
            eids_set.add(e.id)

        for g in comps3.groups:
            gids3_set.add(g.id)
        for c in comps3.categories:
            cids3_set.add(c.id)
        for e in comps3.environments:
            eids3_set.add(e.id)

        self.assertTrue(gids3_set == gids_set)
        self.assertTrue(cids3_set == cids_set)
        self.assertTrue(eids3_set == eids_set)

if __name__ == "__main__":
    unittest.main(testRunner = MyRunner)
    #unittest.main()
    #suite = unittest.TestLoader().loadTestsFromTestCase(CategoryTest)
    #ret = MyRunner(verbosity=2).run(suite)
    
    #unittest.TextTestRunner(verbosity=2).run(suite)
    #suite = unittest.TestLoader().loadTestsFromTestCase(GroupTest)
    #ret = MyRunner(verbosity=2).run(suite)
    #suite = unittest.TestLoader().loadTestsFromTestCase(EnvTest)
    #MyRunner(verbosity=2).run(suite)
    #suite = unittest.TestLoader().loadTestsFromTestCase(COMPSTest)
    #MyRunner(verbosity=2).run(suite)
    #suite = unittest.TestLoader().loadTestsFromTestCase(CategoryListTest)
    #MyRunner(verbosity=2).run(suite)
    #suite = unittest.TestLoader().loadTestsFromTestCase(GroupListTest)
    #MyRunner(verbosity=2).run(suite)
    #suite = unittest.TestLoader().loadTestsFromTestCase(EnvListTest)
    #MyRunner(verbosity=2).run(suite)



