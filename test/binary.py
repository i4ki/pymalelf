import malelf
import unittest as unittest
import errno


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

        except malelf.Error as e:
            self.assertTrue(1 == 0,
                            ("malelf.Binary constructor failed with "
                             "exception %s" % e))

        except Exception as e:
            print (e)

    def test_binary_open(self):
        binary = malelf.Binary()
        with self.assertRaises(TypeError):
            binary.open()
        with self.assertRaises(TypeError):
            binary.open(None)
        with self.assertRaises(TypeError):
            binary.open(0)
        with self.assertRaises(TypeError):
            binary.open([])
        with self.assertRaises(TypeError):
            binary.open({})

        with self.assertRaises(malelf.Error) as err:
            binary.open("/dfghjkertyui")

        exc = err.exception
        # ENOENT
        self.assertEqual(exc.code, errno.ENOENT)

        with self.assertRaises(malelf.Error) as err:
            binary.open("/etc/passwd")

        exc = err.exception
        self.assertEqual(exc.code, malelf.ErrorCode.NOT_ELF)

        res = binary.open("/bin/ls")
        self.assertEqual(binary.fname, "/bin/ls")
        self.assertTrue(binary.size > 0)
        self.assertIsNotNone(binary.mem)
        self.assertIsInstance(binary.mem, str)
        self.assertTrue(len(binary.mem) > 0)
        self.assertTrue(res)
        self.assertEqual(binary.alloc_type, malelf.ALLOC_MMAP)
        self.assertEqual(binary.arch, malelf.ELF32)

        res = binary.open("/bin/bash")
        self.assertEqual(binary.fname, "/bin/bash")
        self.assertTrue(binary.size > 0)
        self.assertIsNotNone(binary.mem)
        self.assertIsInstance(binary.mem, str)
        self.assertTrue(len(binary.mem) > 0)
        self.assertTrue(res)
        self.assertEqual(binary.alloc_type, malelf.ALLOC_MMAP)
        self.assertEqual(binary.arch, malelf.ELF32)

        binary.close()

    def test_binary_close(self):
        binary = malelf.Binary("/bin/ls")
        res = binary.open()
        self.assertTrue(res)
        res = binary.close()
        self.assertTrue(res)
        self.assertEqual(binary.fname, "/bin/ls")
        self.assertIsNone(binary.mem)
        self.assertEqual(binary.size, 0)

        binary = malelf.Binary("/bin/ls")
        res = binary.open()
        self.assertTrue(res)
        binary.close()
        self.assertIsNone(binary.mem)
        self.assertEqual(binary.fname, "/bin/ls")
        self.assertEqual(binary.size, 0)


if __name__ == "__main__":
    unittest.main()
