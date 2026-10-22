import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class MDictCopyTest(unittest.TestCase):
    def test_copy_then_delete(self):
        mdict = libcomps.MDict()
        mdict["a"] = ["x"]
        mdict["ab"] = ["y"]
        copy = mdict.copy()
        del copy
        self.assertEqual(list(mdict["ab"]), ["y"])


if __name__ == "__main__":
    unittest.main()
