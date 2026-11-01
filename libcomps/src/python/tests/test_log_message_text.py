import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class LogMessageTextTest(unittest.TestCase):
    def test_empty_group_message(self):
        comps = libcomps.Comps()
        comps.groups.append(libcomps.Group("grp1", "Grp", "d"))
        comps.xml_str()
        self.assertIn("Group with id grp1 has no packages. Skipping xml output\n",
                      comps.get_last_errors())


if __name__ == "__main__":
    unittest.main()
