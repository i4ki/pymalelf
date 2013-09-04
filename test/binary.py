import malelf
import unittest

class TestBinary(unittest.TestCase):
    binary = None

    def setUp(self):
        pass

    def test_constructor(self):
        try:
            binary = malelf.Binary()
            self.assertIsNotNone(binary)
            self.assertIsInstance(binary, malelf.Binary)
        except malelf.Error, e:
            self.assertTrue(1 == 1,
                            ("malelf.Binary constructor failed with "
                             "exception %s" % e))
            print e.message
        except e:
            print e
