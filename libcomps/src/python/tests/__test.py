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

try:
    import _libpycomps as libcomps
except:
    import libcomps

import sys
import unittest
import tempfile
import os
import traceback
import inspect

import utest

if sys.version_info[0] == 3:
    from functools import reduce
if hasattr(dict, "iteritems"):
    def _iteritems(_dict):
        return _dict.iteritems()
    def _itervalues(_dict):
        return _dict.itervalues()
else:
    def _iteritems(_dict):
        return _dict.items()
    def _itervalues(_dict):
        return _dict.values()

class BaseObjTestClass(object):
    obj_data = []
    obj_getset = {}
    obj_type = None

    dict1 = libcomps.StrDict()
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
                 libcomps.StrDict: [dict1],
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
               inspect.ismemberdescriptor(x[1])) and\
               not x[0].startswith("__") and\
               not inspect.ismethod(x[1]):
                data_desc.append(x)
        for attr in data_desc:
            z = getattr(obj, x[0])
            attr_types = self.obj_getset[attr[0]]
            for _type, vals in _iteritems(self.obj_types):
                if _type not in attr_types:
                    #print (attr[0], vals[0])
                    self.assertRaises(TypeError, setattr, obj, attr[0], vals[0])
                else:
                    for val in vals:
                        setattr(obj, attr[0], val)

            self.assertRaises(TypeError, delattr, obj, attr)

    #@unittest.skip("")
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

    #@unittest.skip("")
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


    #@unittest.skip("")
    def test_union1(self):
        obj1 = self.__union_prep1()
        obj2 = self.obj_constructor(**self.obj_data[1])
        obj = obj1 + obj2
        for x in inspect.getmembers(obj.__class__):
            if (inspect.isdatadescriptor(x[1]) or\
               inspect.isgetsetdescriptor(x[1]) or\
               inspect.ismemberdescriptor(x[1])) and\
               not x[0].startswith("__") and\
               not inspect.ismethod(x[1]):
                if getattr(obj2, x[0]):
                    self.assertTrue(getattr(obj, x[0]) == getattr(obj2, x[0]))
                else:
                    self.assertTrue(getattr(obj, x[0]) == getattr(obj1, x[0]))

        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj, member)
            self.assertTrue(len(_list) == 4)

        for member, v in _iteritems(self.obj_dict_members):
            _d1 = dict([(k, v) for k, v in _iteritems(getattr(obj1, member))])
            _d2 = dict([(k, v) for k, v in _iteritems(getattr(obj2, member))])
            _d1.update(_d2)
            _dict = getattr(obj, member)
            _d = dict([(k,v) for k,v in _iteritems(_dict)])
            self.assertTrue(_d == _d1)

    #@unittest.skip("")
    def test_union2(self):
        obj1 = self.__union_prep1()
        obj2 = self.obj_constructor(**self.obj_data[1])
        obj = obj1 + obj2

        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj, member)
            self.assertTrue(len(_list) == 4)

        for member, v in _iteritems(self.obj_dict_members):
            _d1 = dict([(k, v) for k,v in _iteritems(getattr(obj1, member))])
            _d2 = dict([(k, v) for k,v in _iteritems(getattr(obj2, member))])
            _d1.update(_d2)
            _dict = getattr(obj, member)
            _d = dict([(k, v) for k,v in _iteritems(_dict)])
            self.assertTrue(_d == _d1)

    #@unittest.skip("")
    def test_union3(self):
        obj1 = self.__union_prep1()
        obj2 = self.__union_prep2()
        obj = obj1 + obj2

        for member, v in _iteritems(self.obj_list_members):
            _list = getattr(obj, member)
            self.assertTrue(len(_list) == len(v))

        for member, v in _iteritems(self.obj_dict_members):
            _d1 = dict([(k, v) for k,v in _iteritems(getattr(obj1, member))])
            _d2 = dict([(k, v) for k,v in _iteritems(getattr(obj2, member))])
            _d1.update(_d2)
            _dict = getattr(obj, member)
            _d = dict([(k,v) for k,v in _iteritems(_dict)])
            self.assertTrue(_d == _d1)

    def test_hash(self):
        s = set()
        for x in range(6):
            s.add(self.obj_constructor(**self.obj_data[int(x/2)]))
        self.assertTrue(len(s) == 3)
        self.assertTrue(hash(self.obj_constructor(**self.obj_data[0])) ==\
                        hash(self.obj_constructor(**self.obj_data[0])))

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
                  "name_by_lang": [libcomps.StrDict],
                  "desc_by_lang": [libcomps.StrDict]}
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
                  "biarchonly": [bool],
                  "default": [bool],
                  "lang_only": [str],
                  "display_order": [int, bool],
                  "packages": [libcomps.PackageList],
                  "name_by_lang": [libcomps.StrDict],
                  "desc_by_lang": [libcomps.StrDict]
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
                  "name_by_lang": [libcomps.StrDict],
                  "desc_by_lang": [libcomps.StrDict]
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

    dict1 = libcomps.StrDict()
    dict1["foo"] = "bar"
    dict1["Tom"] = "Jerry"
    dict1["linux"] = "rolls!"

    dict2 = libcomps.StrDict()
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
        self.assertRaises(IndexError, listobj.__getitem__, 0)
        listobj.append(item)
        self.assertTrue(listobj[0] == item)
        self.assertTrue(len(listobj) == 1)
        self.assertRaises(IndexError, listobj.__getitem__, 1)

    def test_append(self):
        listobj = self.list_type()
        for x in self.items_data:
            listobj.append(self.item_type(**x))
        for x in range(0, len(self.items_data)):
            self.assertTrue(listobj[x] == self.item_type(**self.items_data[x]))

    def test_insert(self):
        listobj = self.list_type()
        for x in self.items_data[:4]:
            listobj.insert(0, self.item_type(**x))
        for x in range(0, 4):
            self.assertTrue(listobj[x] == self.item_type(**self.items_data[3-x]))
        listobj.insert(-100, self.item_type(**self.items_data[0]))
        self.assertTrue(listobj[0] == self.item_type(**self.items_data[0]))

        listobj.insert(100, self.item_type(**self.items_data[-1]))
        self.assertTrue(listobj[-1] == self.item_type(**self.items_data[-1]))

        listobj.insert(-4, self.item_type(**self.items_data[-1]))
        self.assertTrue(listobj[-5] == self.item_type(**self.items_data[-1]))

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
        self.assertRaises(IndexError, listobj.__getitem__, 0)
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

        listobj2 = self.list_type()
        for x in self.items_data[3:]:
            listobj2.append(self.item_type(**x))
            for k,vals in _iteritems(self.items_extra_data):
                for val in vals[3:]:
                    self.items_extra_data_setter[k](getattr(listobj2[-1], k),
                                                    val)

        listobj = listobj1 + listobj2
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

    def test_get_by_id(self):
        listobj1 = self.list_type()
        for x in self.items_data[0:4]:
            listobj1.append(self.item_type(**x))
            for k,vals in _iteritems(self.items_extra_data):
                for val in vals[0:3]:
                    self.items_extra_data_setter[k](getattr(listobj1[-1], k),
                                                    val)
        index = 0
        if hasattr(self.item_type, "id"):
            attr = "id"
        else:
            attr = "name"
        for x in self.items_data[0:4]:
            item = listobj1[x[attr]]
            self.assertTrue(item == listobj1[index])
            index += 1
        self.assertRaises(KeyError, listobj1.__getitem__, "notid")

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

#@unittest.skip(" ")
class PackageList_Test(unittest.TestCase, BaseListTestClass):
    list_type = libcomps.PackageList
    item_type = libcomps.Package
    items_data = [{"name":"p1", "type": libcomps.PACKAGE_TYPE_DEFAULT},
                  {"name":"p2", "type": libcomps.PACKAGE_TYPE_DEFAULT},
                  {"name":"p3", "type": libcomps.PACKAGE_TYPE_DEFAULT},
                  {"name":"p4", "type": libcomps.PACKAGE_TYPE_DEFAULT},
                  {"name":"p5", "type": libcomps.PACKAGE_TYPE_DEFAULT},
                  {"name":"p6", "type": libcomps.PACKAGE_TYPE_DEFAULT},
                  {"name":"p7", "type": libcomps.PACKAGE_TYPE_DEFAULT}]

    items_extra_data_cmp = {}
    items_extra_data_setter = {}
    items_extra_data = {}

#@unittest.skip("skip")
class PackageTest(unittest.TestCase):
    def test_attrs(self):
        pkg = libcomps.Package("kernel-3.2", libcomps.PACKAGE_TYPE_MANDATORY)
        self.assertEqual(pkg.name, "kernel-3.2")
        self.assertEqual(pkg.type, libcomps.PACKAGE_TYPE_MANDATORY)

    def test_hash(self):
        pkg1 = libcomps.Package("kernel-3.2", libcomps.PACKAGE_TYPE_MANDATORY)
        pkg2 = libcomps.Package("kernel-3.2", libcomps.PACKAGE_TYPE_MANDATORY)
        pkg3 = libcomps.Package("kernel-3.1", libcomps.PACKAGE_TYPE_MANDATORY)
        self.assertTrue(pkg1 == pkg2)
        self.assertTrue(hash(pkg1) == hash(pkg2))
        self.assertTrue(pkg2 != pkg3)
        self.assertTrue(hash(pkg2) != hash(pkg3))
        self.assertTrue(len(set([pkg1,pkg2,pkg3])) == 2)

    def test_mandatory_in_xml_out(self):
        self.comps = libcomps.Comps()
        self.comps.groups.append(libcomps.Group("g1", "group1", "group desc", 0, 0, 0, "en"))
        self.comps.groups[0].packages.append(libcomps.Package("kernel", libcomps.PACKAGE_TYPE_MANDATORY))

        out = self.comps.xml_str()
        self.assertTrue("<packagereq type=\"mandatory\">kernel</packagereq>" in out)

#@unittest.skip("skip")
class DictTest(unittest.TestCase):
    def test_dict(self):
        _dict = libcomps.StrDict()
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

        _dict2 = libcomps.StrDict()
        for (k,w) in _iteritems(_dict):
            _dict2[k] = w
        self.assertTrue(_dict == _dict2)

        _values =  []
        _values2 = ["Ahoj svete", "Hello world"] 
        for v in _itervalues(_dict):
            _values.append(v)
        self.assertTrue(set(_values) == set(_values2))

    def test_keyerror(self):
        _dict = libcomps.StrDict()
        self.assertTrue(_dict.get("notindict") == None)
        self.assertRaises(KeyError, _dict.__getitem__, "notindict")

#@unittest.skip("skip")
class MDictTest(unittest.TestCase):
    def test_blacklist(self):
        bl1 = libcomps.Blacklist()
        bl2 = libcomps.Blacklist()
        self._test(bl1, bl2)

    def test_whiteout(self):
        wo1 = libcomps.Whiteout()
        wo2 = libcomps.Whiteout()
        self._test(wo1, wo2)
    
    def _test(self, obj1, obj2):
        obj1["key"] = []
        self.assertTrue(obj1["key"] == libcomps.StrSeq())
        obj1["key"].append("val1")
        obj1["key"].append("val2")
        obj2["key"] = ["val1", "val2"]
        self.assertTrue(obj1["key"] == obj2["key"])
        del obj1["key"]
        self.assertTrue("key" not in obj1)
        self.assertRaises(KeyError, obj1.__getitem__, "key")
        self.assertFalse(obj1 == obj2)

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
        ret = self.comps.xml_f(fname, def_options={"default_explicit":True})
        self.assertTrue(not ret)

        comps2 = libcomps.Comps()
        ret = comps2.fromxml_f(fname)
        self.assertTrue(ret == 0, comps2.get_last_errors())

        self.assertTrue(len(comps2.groups) == 3)
        self.assertTrue(len(comps2.categories) == 2)
        self.assertTrue(len(comps2.environments) == 0)

        x = self.comps.xml_str(xml_options={})
        y = comps2.xml_str()

        self.assertTrue(x == y)
        os.remove(fname)

        compsdoc = comps2.xml_str()
        compsdoc = compsdoc[0:-5] # make some error
        comps3 = libcomps.Comps()
        self.assertRaises(libcomps.ParserError, comps3.fromxml_str, compsdoc)

        INVALID_COMPS_XML = "invalid xml"
        comps4 = libcomps.Comps()
        self.assertRaises(libcomps.ParserError, comps4.fromxml_str, str(INVALID_COMPS_XML))

        VALID_XML_NO_COMPS = "<whatever />"
        comps5 = libcomps.Comps()
        ret = comps5.fromxml_str(str(VALID_XML_NO_COMPS))
        # return code 1 is non fatal error
        self.assertTrue(ret == 1, comps5.get_last_errors())

        VALID_XML_HEADER_NO_COMPS = """<?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE variants PUBLIC "-//Red Hat, Inc.//DTD Variants info//EN" "variants.dtd">
        <variants>
        </variants>"""
        comps6 = libcomps.Comps()
        ret = comps6.fromxml_str(str(VALID_XML_HEADER_NO_COMPS))
        # return code 1 is non fatal error
        self.assertTrue(ret == 1, comps6.get_last_errors())


    #@unittest.skip("")
    def test_fedora(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/fedora_comps.xml")
        #for x in comps.get_last_parse_log():
        #    print x
        self.assertTrue(ret != -1)
        comps.xml_f("fed2.xml", xml_options={"empty_groups":True,
                                             "empty_categories":True,
                                             "empty_environments":True,
                                             "uservisible_explicit": True,
                                             "default_explicit": True})
        comps2 = libcomps.Comps()
        comps2.fromxml_f("fed2.xml")
        self.assertTrue(comps == comps2)

    #@unittest.skip("skip")
    def test_sample(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/sample_comps.xml")
        #print comps.get_last_parse_log()
        self.assertTrue(ret != -1)

        comps.xml_f("sample2.xml", xml_options={"empty_groups":True,
                                                "empty_categories":True,
                                                "empty_environments":True,
                                                "uservisible_explicit": True,
                                                "default_explicit": True})
        comps2 = libcomps.Comps()
        comps2.fromxml_f("sample2.xml")
        self.assertTrue(comps == comps2)

    #@unittest.skip("skip")
    def test_main(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/main_comps.xml")
        self.assertTrue(ret != -1)
        comps.xml_f("main2.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("main2.xml")
        x = comps.xml_str(xml_options={"empty_groups":True,
                                         "empty_categories":True,
                                         "empty_environments":True})
        y = comps2.xml_str(xml_options={"empty_groups":True,
                                         "empty_categories":True,
                                         "empty_environments":True})
        self.assertTrue(comps == comps2)
        self.assertTrue(y == x)
        os.remove("main2.xml")

    #@unittest.skip("skip")
    def test_main2(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/main_comps2.xml")
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
        c2.fromxml_f('comps/main_comps.xml')
        c = c1 + c2

        c1 = libcomps.Comps()
        c1.fromxml_f('comps/main_comps.xml')

        c2 = libcomps.Comps()
        c2.fromxml_f('comps/main_comps.xml')

        c = c1 + c2
        x = c.groups[0].packages[0].name
        self.assertTrue(x == "pepper")

    #@unittest.skip("skip")
    def test_main_loc(self):
        comps = libcomps.Comps()
        errors = comps.fromxml_f('comps/main_comps.xml')
        g = comps.groups[0]
        #print(g.desc_by_lang['cs'])

        comps = libcomps.Comps()
        errors = comps.fromxml_f('comps/main_comps.xml')
        g = comps.groups[0]
        g1 = comps.groups[0]
        g.desc_by_lang = g1.desc_by_lang
        dbl1 = g.desc_by_lang
        dbl2 = g1.desc_by_lang
        self.assertTrue(g1.desc_by_lang == g.desc_by_lang)

    #@unittest.skip("skip")
    def test_union(self):
        comps = libcomps.Comps()
        comps.fromxml_f("comps/sample_comps.xml")
        comps2 = libcomps.Comps()
        comps2.fromxml_f("sample2.xml")
        comps3 = comps + comps2
        self.assertTrue(comps == comps3)
        comps.fromxml_f("comps/fedora_comps.xml")
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

    #@unittest.skip("")
    def test_gz(self):
        comps = libcomps.Comps()
        self.assertRaises(IOError, comps.fromxml_f, "comps/sample_comp2.xml.gz")

    #@unittest.skip("")
    def test_a_inoptid(self):
        c = libcomps.Comps()
        ret = c.fromxml_f("comps/sample_comps2.xml")
        self.assertTrue(ret != -1)
        groups = c.groups

        envs = [e for e in c.environments if "gnome" in e.id]
        env = envs[0]
        #for grp in groups:
        #    id_ = grp.id
        ret = reduce(lambda x,y : y and (x or env.option_ids), groups, False)
        self.assertTrue(ret)

    #@unittest.skip("")
    def test_gid(self):
        gid1 = libcomps.GroupId("gid1")
        gid2 = libcomps.GroupId("gid2", default=False)
        gid3 = libcomps.GroupId("gid3", default=True)
        self.assertTrue(gid1 != None)
        self.assertRaises(TypeError, gid1.__eq__, 1)
        self.assertTrue(gid1 == gid1)
        self.assertTrue(gid1 != "gid2")
        self.assertTrue(gid1 != gid2)
        self.assertTrue(gid1 != gid3)
        self.assertRaises(NotImplementedError, gid1.__lt__, gid2)

    #@unittest.skip("")
    def test_default(self):
        e = libcomps.Environment("e1", "enviroment1", "env desc")
        e.group_ids.append("groupid1")
        e.group_ids.append("groupid2")
        gid = libcomps.GroupId("groupid3", default=True)
        e.group_ids.append(gid)

        gid = libcomps.GroupId("groupid4")
        e.group_ids.append(gid)

        gid = libcomps.GroupId("groupid4", default=False)
        e.group_ids.append(gid)

        self.assertRaises(TypeError, libcomps.GroupId.__init__)
        #    gid = libcomps.GroupId()

        self.assertFalse(e.group_ids[0].default)
        self.assertFalse(e.group_ids[1].default)
        self.assertTrue(e.group_ids[2].default)
        self.assertFalse(e.group_ids[3].default)
        self.assertFalse(e.group_ids[4].default)

    #@unittest.skip("")
    def test21(self):
        c1 = libcomps.Comps()
        c1.fromxml_f("comps/f21-rawhide-comps.xml")
        c2 = libcomps.Comps()
        c2.fromxml_f("comps/sample_comps.xml")
        c = c1 + c2
        c.xml_f("f21_united.xml")

    #@unittest.skip("")
    def test_f21_in(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/comps-f21.xml.in",
                              options={"default_biarchonly": False})

        for i in comps.groups:
            for j in i.packages:
                x = j
        self.assertTrue(ret != -1)
        comps.xml_f("f21-2.xml")#, def_options={"default_biarchonly": False},
                                # xml_options={"biarchonly_explicit": False})
        comps2 = libcomps.Comps()
        comps2.fromxml_f("f21-2.xml", options={"default_biarchonly": False})
        self.assertTrue(comps.groups[39] == comps2.groups[39])
        self.assertTrue(comps == comps2)

    #@unittest.skip("")
    def test_envs(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/f21-rawhide-comps.xml")
        env = comps.environments[0]
        self.assertEqual(env.name_by_lang['cs'], u'Pracovní prostředí GNOME')
        self.assertEqual(env.desc_by_lang['de'],
                         u'GNOME ist eine hoch-intuitive und benutzerfreundliche Benutzeroberfläche')
        group_ids = ("base-x", "standard", "core", "dial-up",
                     "fonts", "input-methods", "multimedia",
                     "hardware-support", "printing", "firefox",
                     "guest-desktop-agents", "gnome-desktop")

        if hasattr(self, "assertItemsEqual"):
            _f = self.assertItemsEqual
        elif hasattr(self, "assertItemsEqual"):
            _f = self.assertSequenceEqual
        else:
           _f = utest.assertSequenceEqual

        _f([id_.name for id_ in env.group_ids],
                              group_ids)
        _f([id_.default for id_ in env.group_ids],
                              (False, False, False, False,
                              False, False, False, False,
                              False, False, False, False))
        option_ids = ("libreoffice", "gnome-games", "epiphany", "3d-printing")

        _f([x.name for x in env.option_ids], option_ids)

    #@unittest.skip("")
    def test_clear_for_COMPS_ObjRTree_such_as_group_or_category_namy_by_lang(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/f21-rawhide-comps.xml")

        env = comps.categories[0].name_by_lang.clear()
        env = comps.groups[0].name_by_lang.clear()

        self.assertEqual(str(comps.categories[0].name_by_lang), u'{}')
        self.assertEqual(str(comps.groups[0].name_by_lang), u'{}')

    #@unittest.skip("")
    def test_xml_options(self):
        comps = libcomps.Comps()
        g = libcomps.Group()
        g.id = "g1"
        comps.groups.append(g)
        c = libcomps.Category()
        c.id = "c1"
        comps.categories.append(c)
        e = libcomps.Environment()
        e.id = "e1"
        comps.environments.append(e)
        s = comps.toxml_str()

        comps2 = libcomps.Comps()
        comps2.fromxml_str(s)
        self.assertEqual(len(comps2.groups), 0)
        self.assertEqual(len(comps2.categories), 0)
        self.assertEqual(len(comps2.environments), 0)

        s = comps.toxml_str(xml_options={"empty_groups": True,
                                     "empty_categories": True,
                                     "empty_environments": True})
        comps2 = libcomps.Comps()
        comps2.fromxml_str(s)
        self.assertEqual(len(comps2.groups), 1)
        self.assertEqual(len(comps2.categories), 1)
        self.assertEqual(len(comps2.environments), 1)

        s = comps.toxml_str(xml_options={"empty_groups": True,
                                     "empty_categories": True,
                                     "empty_environments": True,
                                     "uservisible_explicit": True,
                                     "default_explicit":True})
        self.assertTrue("</default>" in s)
        self.assertTrue("</uservisible>" in s)

        g.packages.append(libcomps.Package("pkg"))
        s = comps.toxml_str(xml_options={"empty_groups": True,
                                     "empty_categories": True,
                                     "empty_environments": True,
                                     "bao_explicit": True})
        self.assertTrue("basearchonly=" in s)

    #@unittest.skip("")
    def test_arches(self):
        comps = libcomps.Comps()
        comps.fromxml_f("comps/main_arches.xml")
        filtered = comps.arch_filter(["x86"])
        g1_pkgs = ["pkg3", "pkg4", "pkg5", "pkg6", "pkg7"]
        c1_gids = ["g1", "g3", "g4", "g6", "g7"]
        e1_gids = ["g1", "g3", "g4", "g6"]
        e1_oids = ["o1", "o3", "o4", "o6"]
        x = 0
        for pkg in filtered.groups[0].packages:
            self.assertTrue(g1_pkgs[x] == pkg.name)
            x +=1
        x = 0
        for gid in filtered.categories[0].group_ids:
            self.assertTrue(c1_gids[x] == gid.name)
            x +=1
        x = 0
        for gid in filtered.environments[0].group_ids:
            self.assertTrue(e1_gids[x] == gid.name)
            x +=1
        x = 0
        for oid in filtered.environments[0].option_ids:
            self.assertTrue(e1_oids[x] == oid.name)
            x +=1
        test_comps = libcomps.Comps()
        env1 = libcomps.Environment(id='env4', name='env4')
        env1.option_ids.append("o5")
        group1 = libcomps.Group(id='group4', name='group4')
        group1.biarchonly = False
        group_id = libcomps.GroupId(name='g1')
        c1 = libcomps.Category(id='cat4', name='cat4')
        c1.group_ids.append(group_id)
        test_comps.environments.append(env1)
        test_comps.groups.append(group1)
        test_comps.categories.append(c1)
        comps3 = comps.arch_filter(["unknown"])
        self.assertTrue(test_comps == comps3)

        comps4 = comps.arch_filter(["x86", "x86_64", "s390"])
        self.assertTrue(comps == comps4)

        s = comps.toxml_str(xml_options = {"arch_output":True})
        comps5 = libcomps.Comps()
        comps5.fromxml_str(s)
        self.assertTrue(comps == comps5)

    def test_arches_invalid_input(self):
        INVALID_UTF8_CHAR = '\udcfd'
        c = libcomps.Comps()
        self.assertRaises(TypeError, c.arch_filter, [INVALID_UTF8_CHAR])

    #@unittest.skip("")
    def test_validate(self):
        c = libcomps.Comps()
        c.fromxml_f("comps/f21-rawhide-comps.xml")
        self.assertRaises(ValueError, c.validate)
        c2 = libcomps.Comps()
        cat1 = libcomps.Category()
        self.assertRaises(ValueError, c2.categories.append, cat1)
        group1 = libcomps.Group()
        self.assertRaises(ValueError, c2.groups.append, group1)
        env1 = libcomps.Environment()
        self.assertRaises(ValueError, c2.environments.append, env1)

        self.assertRaises(ValueError, cat1.group_ids.append, "")
        self.assertRaises(ValueError, env1.group_ids.append, "")
        self.assertRaises(ValueError, env1.option_ids.append, "")
        self.assertRaises(ValueError, group1.packages.append,
                                      libcomps.Package(""))

    #@unittest.skip("")
    def test_defaults(self):
        c = libcomps.Comps()
        c.fromxml_f("comps/main_def.xml",
                    options={"default_uservisible": True,
                             "default_default": True,
                             "default_pkgtype":
                             libcomps.PACKAGE_TYPE_DEFAULT})

        self.assertTrue(c.groups[0].default == True)
        self.assertTrue(c.groups[0].uservisible == True)
        self.assertTrue(c.groups[0].packages[0].type == \
                        libcomps.PACKAGE_TYPE_DEFAULT)

        c.fromxml_f("comps/main_def.xml",
                    options={"default_uservisible": False,
                             "default_default": False,
                             "default_pkgtype":
                             libcomps.PACKAGE_TYPE_MANDATORY})

        self.assertTrue(c.groups[0].default == False)
        self.assertTrue(c.groups[0].uservisible == False)

        self.assertTrue(c.groups[0].packages[0].type == \
                        libcomps.PACKAGE_TYPE_MANDATORY)

    def test_fobj(self):
        c = libcomps.Comps()
        self.assertRaises(TypeError, c.fromxml_f, open("comps/main_def.xml"))

    #biarchonly in rhel-4
    def test_biarchonly(self):
        ORIG = 'comps/biarchonly.xml'
        comps = libcomps.Comps()
        comps.fromxml_f(ORIG, options={"default_biarchonly": True})
        self.assertTrue(comps.groups[0].biarchonly == False)
        self.assertTrue(comps.groups[1].biarchonly == True)
        self.assertTrue(comps.groups[2].biarchonly == True)
        xmlstr = comps.toxml_str(xml_options={"biarchonly_explicit": True})
        comps2 = libcomps.Comps()
        comps2.fromxml_str(xmlstr, options={"default_biarchonly": True})
        self.assertTrue(comps == comps2)

    def test_match(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/comps-rawhide.xml")
        self.assertTrue(ret != -1)
        self.assertTrue(len(comps.groups_match(id="base")) == 0)
        self.assertTrue(len(comps.groups_match(id="base-x")) == 1)
        self.assertTrue(len(comps.groups_match(name="base-x")) == 1)
        self.assertTrue(len(comps.groups_match(desc="Local X.org display server")) == 1)
        self.assertTrue(len(comps.groups_match(id="base-x", name="base-x")) == 1)
        self.assertTrue(len(comps.groups_match(id="base-x",
                                              name="base-x",
                                              desc="Local X.org display server")) == 1)
        self.assertTrue(len(comps.groups_match(id="base-x",
                                              name="base-x",
                                              desc="Lokaler X.org-Displayserver")) == 0)
        self.assertTrue(len(comps.groups_match(id="base-x",
                                              name="base-x",
                                              desc="Lokaler X.org-Displayserver",
                                              lang="de")) == 1)
        self.assertTrue(len(comps.groups_match(id="base-x",
                                              name="base-x",
                                              desc="Lokaler X.org-Displayserver",
                                              lang="es")) == 0)

        self.assertTrue(len(comps.categories_match(id="gnome-desktop")) == 0)
        self.assertTrue(len(comps.categories_match(id="gnome-desktop-environment")) == 1)
        self.assertTrue(len(comps.categories_match(name="GNOME Desktop")) == 1)
        self.assertTrue(len(comps.categories_match(desc="GNOME is a highly intuitive and user friendly desktop environment.")) == 1)
        self.assertTrue(len(comps.categories_match(id="gnome-desktop-environment", name="GNOME Desktop")) == 1)
        self.assertTrue(len(comps.categories_match(id="gnome-desktop-environment",
                                              name="GNOME Desktop",
                                              desc="GNOME is a highly intuitive and user friendly desktop environment.")) == 1)
        self.assertTrue(len(comps.categories_match(id="gnome-desktop-environment",
                                              name="GNOME Desktop",
                                              desc="GNOME es un entorno de escritorio muy intuitivo y amigable")) == 0)
        self.assertTrue(len(comps.categories_match(id="gnome-desktop-environment",
                                              name="Escritorio GNOME",
                                              desc="GNOME es un entorno de escritorio muy intuitivo y amigable",
                                              lang="es")) == 1)
        self.assertTrue(len(comps.categories_match(id="gnome-desktop-environment",
                                              name="GNOME Desktop",
                                              desc="Lokaler X.org-Displayserver",
                                              lang="ja")) == 0)

        self.assertTrue(len(comps.environments_match(id="minimal")) == 0)
        self.assertTrue(len(comps.environments_match(id="minimal-environment")) == 1)
        self.assertTrue(len(comps.environments_match(name="Minimal Install")) == 1)
        self.assertTrue(len(comps.environments_match(desc="Basic functionality.")) == 1)
        self.assertTrue(len(comps.environments_match(id="minimal-environment", name="Minimal Install")) == 1)
        self.assertTrue(len(comps.environments_match(id="minimal-environment",
                                              name="Minimal Install",
                                              desc="Basic functionality.")) == 1)
        self.assertTrue(len(comps.environments_match(id="minimal-environment",
                                              name="Minimal Install",
                                              desc="Basis functionaliteit.")) == 0)
        self.assertTrue(len(comps.environments_match(id="minimal-environment",
                                              name="Minimale installatie",
                                              desc="Basis functionaliteit.",
                                              lang="nl")) == 1)
        self.assertTrue(len(comps.environments_match(id="minimal-environment",
                                              name="Minimale installatie",
                                              desc="Basis functionaliteit.",
                                              lang="es")) == 0)

    def test_group_packages_match(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_f("comps/comps-rawhide.xml")
        group = comps.groups_match(id="assamese-support")[0]
        default_packages = group.packages_match(type=libcomps.PACKAGE_TYPE_DEFAULT)
        mandatory_packages = group.packages_match(type=libcomps.PACKAGE_TYPE_MANDATORY)
        conditional_packages = group.packages_match(type=libcomps.PACKAGE_TYPE_CONDITIONAL)
        self.assertTrue(len(default_packages) == 1)
        for x in default_packages:
            self.assertTrue(x.type == libcomps.PACKAGE_TYPE_DEFAULT)
        self.assertTrue(len(mandatory_packages) == 3)
        for x in mandatory_packages:
            self.assertTrue(x.type == libcomps.PACKAGE_TYPE_MANDATORY)
        self.assertTrue(len(conditional_packages) == 1)
        for x in conditional_packages:
            self.assertTrue(x.type == libcomps.PACKAGE_TYPE_CONDITIONAL)

if __name__ == "__main__":
    if len(sys.argv)>1:
        suite = unittest.TestSuite()
        for test in sys.argv[1:]:
            if "." in test:
                cls, test_func = test.split(".")
                suite.addTest(globals()[cls](test_func))
            else:
                for attr in dir(globals()[test]):
                    if attr.startswith("test") and\
                       hasattr(getattr(globals()[test], attr), "__call__"):
                        suite.addTest(globals()[test](attr))
            utest.MyRunner().run(suite)
    else:
        unittest.main(testRunner = utest.MyRunner)
    #unittest.main()
    #suite = unittest.TestLoader().loadTestsFromTestCase(Category_Test)
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



