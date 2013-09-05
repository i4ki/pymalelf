import malelf
import unittest2 as unittest


class TestBinary(unittest.TestCase):
    binary = None

    def setUp(self):
        pass

    def test_constructor(self):
        try:
            binary = malelf.Binary()
            self.assertIsNotNone(binary)
            self.assertIsInstance(binary, malelf.Binary)
            self.assertIsNone(binary.fname)
            self.assertIsNone(binary.mem)
            self.assertEqual(binary.size, 0)

            binary = malelf.Binary(fname="/bin/ls")
            self.assertIsNotNone(binary)
            self.assertIsInstance(binary, malelf.Binary)
            self.assertEqual(binary.fname, "/bin/ls")
            self.assertIsNone(binary.mem)
            self.assertEqual(binary.size, 0)

            binary = malelf.Binary(alloc_type=malelf.ALLOC_MALLOC)
            self.assertIsNotNone(binary)
            self.assertIsInstance(binary, malelf.Binary)
            self.assertEqual(binary.alloc_type, malelf.ALLOC_MALLOC)
            self.assertIsNone(binary.fname)
            self.assertIsNone(binary.mem)
            self.assertEqual(binary.size, 0)

            binary = malelf.Binary(arch=malelf.ELF32)
            self.assertIsNotNone(binary)
            self.assertIsInstance(binary, malelf.Binary)
            self.assertEqual(binary.arch, malelf.ELF32)
            self.assertIsNone(binary.fname)
            self.assertIsNone(binary.mem)
            self.assertEqual(binary.size, 0)

            binary = malelf.Binary(fname="/bin/ls",
                                   alloc_type=malelf.ALLOC_MALLOC,
                                   arch=malelf.ELF64)
            self.assertIsNotNone(binary)
            self.assertIsInstance(binary, malelf.Binary)
            self.assertEqual(binary.arch, malelf.ELF64)
            self.assertEqual(binary.fname, "/bin/ls")
            self.assertEqual(binary.alloc_type, malelf.ALLOC_MALLOC)

        except malelf.Error, e:
            self.assertTrue(1 == 0,
                            ("malelf.Binary constructor failed with "
                             "exception %s" % e))

            print e.message
        except Exception, e:
            print e


if __name__ == "__main__":
    unittest.main()
