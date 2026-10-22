import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class EmptyElementsTest(unittest.TestCase):
    # https://github.com/rpm-software-management/libcomps/issues/117
    def test_empty_bool_and_display_order_elements(self):
        for elem in ("display_order", "uservisible", "biarchonly"):
            comps = libcomps.Comps()
            comps.fromxml_str('<comps><group><id>g</id><name>G</name>'
                              '<description>d</description><%s></%s>'
                              '<packagelist><packagereq>p</packagereq>'
                              '</packagelist></group></comps>' % (elem, elem))
            self.assertEqual(comps.groups[0].id, "g")


if __name__ == "__main__":
    unittest.main()
