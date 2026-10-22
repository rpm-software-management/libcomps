import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class BlacklistAttrsTest(unittest.TestCase):
    def test_no_empty_attrs(self):
        comps = libcomps.Comps()
        comps.fromxml_str('<comps><blacklist><package name="foo"/></blacklist>'
                          '<whiteout><ignoredep requires="bar"/></whiteout>'
                          '</comps>')
        out = comps.xml_str()
        self.assertNotIn('arch=""', out)
        self.assertNotIn('package=""', out)


if __name__ == "__main__":
    unittest.main()
