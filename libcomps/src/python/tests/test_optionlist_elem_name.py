import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class OptionListNameTest(unittest.TestCase):
    def test_unknown_attr_warning(self):
        comps = libcomps.Comps()
        comps.fromxml_str('<comps><environment><id>e</id><name>E</name>'
                          '<description>d</description><grouplist>'
                          '<groupid>g</groupid></grouplist>'
                          '<optionlist foo="bar"><groupid>o</groupid>'
                          '</optionlist></environment></comps>')
        log = "".join(comps.get_last_log())
        self.assertIn("in element 'optionlist'", log)


if __name__ == "__main__":
    unittest.main()
