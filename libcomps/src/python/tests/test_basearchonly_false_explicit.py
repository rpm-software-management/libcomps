import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


class BasearchonlyTest(unittest.TestCase):
    def test_explicit_xml_out(self):
        comps = libcomps.Comps()
        comps.groups.append(libcomps.Group("g1", "group1", "group desc"))
        for name, bao in (("p1", True), ("p2", False), ("p3", None)):
            pkg = libcomps.Package(name, libcomps.PACKAGE_TYPE_MANDATORY)
            if bao is not None:
                pkg.basearchonly = bao
            comps.groups[0].packages.append(pkg)
        out = comps.xml_str(xml_options={"bao_explicit": True})
        self.assertIn('basearchonly="true">p1<', out)
        self.assertIn('basearchonly="false">p2<', out)
        self.assertIn('basearchonly="false">p3<', out)


if __name__ == "__main__":
    unittest.main()
