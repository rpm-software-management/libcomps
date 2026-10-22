import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class MDictCmpTest(unittest.TestCase):
    def test_cmp_values(self):
        bl1 = libcomps.Blacklist()
        bl2 = libcomps.Blacklist()
        bl1["foo"] = ["x86_64"]
        bl2["foo"] = ["s390x"]
        self.assertFalse(bl1 == bl2)
        bl2["foo"] = ["x86_64"]
        self.assertTrue(bl1 == bl2)


if __name__ == "__main__":
    unittest.main()
