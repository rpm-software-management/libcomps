import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class NestedCompsTest(unittest.TestCase):
    def test_nested_comps_element(self):
        comps = libcomps.Comps()
        ret = comps.fromxml_str('<comps><group><id>g</id><comps/></group></comps>')
        self.assertNotEqual(ret, 0)
        self.assertEqual(comps.groups[0].id, "g")


if __name__ == "__main__":
    unittest.main()
