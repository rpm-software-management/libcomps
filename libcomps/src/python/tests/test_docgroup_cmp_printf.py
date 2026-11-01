import unittest

try:
    import _libpycomps as libcomps
except ImportError:
    import libcomps


import subprocess
import sys


class GroupCmpOutputTest(unittest.TestCase):
    def test_no_stdout(self):
        # run in a subprocess: the C library writes to its own stdout buffer
        code = ("try:\n"
                "    import _libpycomps as libcomps\n"
                "except ImportError:\n"
                "    import libcomps\n"
                "libcomps.Group('a', 'A', 'd') == libcomps.Group('b', 'B', 'd')\n")
        out = subprocess.run([sys.executable, "-c", code],
                             stdout=subprocess.PIPE, check=True).stdout
        self.assertEqual(out, b"")


if __name__ == "__main__":
    unittest.main()
