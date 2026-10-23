import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class TranslationsTest(unittest.TestCase):
    def test_name_by_lang_keeps_all_translations(self):
        comps = libcomps.Comps()
        comps.fromxml_str('<comps><group><id>g</id><name>G</name>'
                          '<name xml:lang="de">DE</name>'
                          '<name xml:lang="pt_BR">BR</name>'
                          '<name xml:lang="pt">PT</name>'
                          '<description>d</description>'
                          '<packagelist><packagereq>p</packagereq>'
                          '</packagelist></group></comps>')
        name_by_lang = comps.groups[0].name_by_lang
        self.assertEqual(sorted(name_by_lang.keys()), ["de", "pt", "pt_BR"])
        self.assertEqual(name_by_lang["de"], "DE")


if __name__ == "__main__":
    unittest.main()
