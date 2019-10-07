#!/usr/bin/python
# -*- coding: utf-8 -*-


import unittest
import utest

try:
    import _libpycomps as libcomps
    print("local tests")
except ImportError:
    import libcomps
    print("global tests")


class TestMergeComps(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None
        self.comps = libcomps.Comps()

    def test_merge_1_1(self):
        # first identity test
        c1 = libcomps.Comps()
        c1.fromxml_f("comps/comps_part1.xml")

        self.comps += c1
        self.comps += c1

        self.assertEqual(self.comps, c1)
        self.assertEqual(self.comps.xml_str(), c1.xml_str())

    def test_merge_2_2(self):
        # second identity test
        c2 = libcomps.Comps()
        c2.fromxml_f("comps/comps_part2.xml")

        self.comps += c2
        self.comps += c2

        self.assertEqual(self.comps, c2)
        self.assertEqual(self.comps.xml_str(), c2.xml_str())

    def test_merge_1_2(self):
        # c2 overrides c1
        c1 = libcomps.Comps()
        c1.fromxml_f("comps/comps_part1.xml")

        c2 = libcomps.Comps()
        c2.fromxml_f("comps/comps_part2.xml")

        self.comps += c1
        self.comps += c2

        merged_comps = libcomps.Comps()
        merged_comps.fromxml_f("comps/merged_comps_1_2.xml")
        self.assertEqual(self.comps.xml_str(), merged_comps.xml_str())
        self.assertTrue(self.comps == merged_comps)

    def test_merge_2_1(self):
        # c1 overrides c2
        c1 = libcomps.Comps()
        c1.fromxml_f("comps/comps_part1.xml")

        c2 = libcomps.Comps()
        c2.fromxml_f("comps/comps_part2.xml")

        self.comps += c2
        self.comps += c1

        merged_comps = libcomps.Comps()
        merged_comps.fromxml_f("comps/merged_comps_2_1.xml")
        self.assertEqual(self.comps.xml_str(), merged_comps.xml_str())
        self.assertTrue(self.comps == merged_comps)


if __name__ == "__main__":
    unittest.main(testRunner = utest.MyRunner)
