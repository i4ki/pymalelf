from malelf import Binary
import unittest

class TestBinary(unittest.TestCase):
    binary = None

    def setUp(self):
        pass


if __name__ == "__main__":
    binary = Binary()
    try:
        binary.open("/bin/ls")
    except e:
        print e.errmsg
