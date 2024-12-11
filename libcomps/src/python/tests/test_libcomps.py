#!/usr/bin/python
# -*- coding: utf-8 -*-


import unittest

import os
import shutil
import tempfile

import utest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class TestLibcomps(unittest.TestCase):

    def setUp(self):
        self.comps = libcomps.Comps()
        self.comps.fromxml_f("comps/comps-f21.xml")
        self.tmp_dir = tempfile.mkdtemp()
        self.tmp_file = os.path.join(self.tmp_dir, "comps.xml")

    def tearDown(self):
        shutil.rmtree(self.tmp_dir)

    def test_add_group_missing_id(self):
        self.comps = libcomps.Comps()

        group = libcomps.Group()
        self.assertRaises(ValueError, self.comps.groups.append, group)

    def test_add_group(self):
        self.comps = libcomps.Comps()
        group = libcomps.Group()
        group.id = "core"
        self.comps.groups.append(group)

        data = self.comps.xml_str(xml_options = {"empty_groups":True,
                                                 "default_explicit":True,
                                                 "uservisible_explicit":True})
        expected_data_1 = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE comps PUBLIC "-//Red Hat, Inc.//DTD Comps info//EN" "comps.dtd">
<comps>
  <group>
    <id>core</id>
    <default>false</default>
    <uservisible>true</uservisible>
  </group>
</comps>
"""
        self.assertEqual(data, expected_data_1)

        group.display_order = 0
        expected_data_2 = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE comps PUBLIC "-//Red Hat, Inc.//DTD Comps info//EN" "comps.dtd">
<comps>
  <group>
    <id>core</id>
    <default>false</default>
    <uservisible>true</uservisible>
    <display_order>0</display_order>
  </group>
</comps>
"""
        data = self.comps.xml_str(xml_options = {"empty_groups":True,
                                                 "default_explicit":True,
                                                 "uservisible_explicit":True})
        self.assertEqual(data, expected_data_2)

        group.display_order = None
        data = self.comps.xml_str(xml_options = {"empty_groups":True,
                                                 "default_explicit":True,
                                                 "uservisible_explicit":True})
        self.assertEqual(data, expected_data_1)

    def test_add_package(self):
        package = libcomps.Package()
        package.name = "kernel"
        group = libcomps.Group()
        group.packages.append(package)
        #print self.comps.xml_str()

    def test_iter_groups(self):
        for group in self.comps.groups:
            group.id
            group.name
            group.uservisible
            self.assertEqual(str(group), group.id)
            self.assertEqual(repr(group).split(" at ")[0],
                            "<libcomps.Group object '%s'" % group.id)
        self.comps.xml_str()
        self.comps.xml_f(self.tmp_file)

    def test_iter_packages(self):
        for group in self.comps.groups:
            for package in group.packages:
                package.name
                package.type
                package.requires
                self.assertEqual(str(package), package.name)
                self.assertEqual(repr(package).split(" at ")[0],
                                 "<libcomps.Package object '%s'" % package.name)
        self.comps.xml_str()
        self.comps.xml_f(self.tmp_file)

    def test_iter_categories(self):
        for category in self.comps.categories:
            category.id
            category.name
            category.desc
            category.display_order
            self.assertRaises(KeyError, category.name_by_lang.__getitem__,
                              "does-not-exist")
            self.assertRaises(KeyError, category.desc_by_lang.__getitem__,
                              "does-not-exist")
            self.assertEqual(str(category), category.id)
            self.assertEqual(repr(category).split(" at ")[0],
                             "<libcomps.Category object '%s'" % category.id)
        self.comps.xml_str()
        self.comps.xml_f(self.tmp_file)

    def test_iter_environments(self):
        for environment in self.comps.environments:
            environment.id
            environment.name
            environment.desc
            environment.display_order
            self.assertRaises(KeyError, environment.name_by_lang.__getitem__,
                              "does-not-exist")
            self.assertRaises(KeyError, environment.desc_by_lang.__getitem__,
                              "does-not-exist")
            self.assertEqual(str(environment), environment.id)
            self.assertEqual(repr(environment).split(" at ")[0],
                             "<libcomps.Environment object '%s'" % environment.id)
        self.comps.xml_str()
        self.comps.xml_f(self.tmp_file)

    def test_iter_iter_environments(self):
        it1 = iter(self.comps.environments)
        it2 = iter(it1)
        it3 = iter(it1)
        it4 = iter(it2)
        self.assertIs(it1, it2)
        self.assertIs(it1, it3)
        self.assertIs(it1, it4)
        del it1
        next(it2)

    def test_iter_iter_langpacks(self):
        it1 = iter(self.comps.langpacks)
        it2 = iter(it1)
        it3 = iter(it1)
        it4 = iter(it2)
        self.assertIs(it1, it2)
        self.assertIs(it1, it3)
        self.assertIs(it1, it4)
        del it1
        next(it2)

    def test_duplicate_groups(self):
        self.comps = libcomps.Comps()

        group = libcomps.Group()
        group.id = "core"
        self.comps.groups.append(group)

        group = libcomps.Group()
        group.id = "core"
        #self.assertRaises(self.comps.groups.append(group))

        #print self.comps.xml_str()
        self.comps.fromxml_str(self.comps.xml_str())

    def test_empty_by_lang_tags(self):
        self.comps = libcomps.Comps()
        self.comps.fromxml_f("comps/comps_empty_by_lang_tags.xml")
        for group in self.comps.groups:
            self.assertEqual("{}", str(group.name_by_lang))
            self.assertEqual("{}", str(group.desc_by_lang))

        for category in self.comps.categories:
            self.assertEqual("{}", str(category.name_by_lang))
            self.assertEqual("{}", str(category.desc_by_lang))

if __name__ == "__main__":
    unittest.main(testRunner = utest.MyRunner)
