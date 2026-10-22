import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class ObjRTreeCopyLenTest(unittest.TestCase):
    def test_copy_len(self):
        strdict = libcomps.StrDict()
        strdict["a"] = "1"
        strdict["b"] = "2"
        self.assertEqual(len(strdict.copy()), 2)


if __name__ == "__main__":
    unittest.main()
