import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class ObjRTreeLenSetTest(unittest.TestCase):
    def test_len_inner_node(self):
        strdict = libcomps.StrDict()
        strdict["ab"] = "1"
        strdict["ac"] = "2"
        strdict["a"] = "3"
        self.assertEqual(len(strdict), 3)
        strdict["a"] = "4"
        self.assertEqual(len(strdict), 3)


if __name__ == "__main__":
    unittest.main()
