import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class MRTreeSplitValuesTest(unittest.TestCase):
    def test_split_keeps_values(self):
        mdict = libcomps.MDict()
        mdict["ab"] = ["1"]
        mdict["ac"] = ["2"]
        self.assertEqual(list(mdict["ab"]), ["1"])
        self.assertEqual(list(mdict["ac"]), ["2"])


if __name__ == "__main__":
    unittest.main()
