#include <Python.h>
#include <structmember.h> /* Python PyObject structure accessors */

#define MALELF_MODULE
#include "malelfmodule.h"
#include "binary.h"

#include <malelf/binary.h>
#include <malelf/error.h>

STATESTUB

static _u32
PyMalelf_BinaryOpen(MalelfBinary *bin, char *fname)
{
        return malelf_binary_open(bin, fname);
}

static PyObject *
malelf_binary(PyObject *self, PyObject *args)
{
        _u8 error = MALELF_SUCCESS;
        MalelfBinary bin;
        const char *fname = NULL;
        struct module_state *st = GETSTATE(self);

        if (!PyArg_ParseTuple(args, "s", &fname)) {
                return NULL;
        }

        malelf_binary_init(&bin);

        error = PyMalelf_BinaryOpen(&bin, fname);

        if (MALELF_SUCCESS != error) {
                PyErr_SetString(st->error, "Binary open failed");
                return NULL;
        }

        return PyLong_FromLong(bin.ehdr.uhdr.h32->e_entry);
}

static PyMethodDef MalelfMethods[] = {
        {"binary_open",  (PyCFunction)malelf_binary, METH_VARARGS,
         "Open a new binary"},
        {NULL, NULL, 0, NULL}        /* Sentinel */
};

#define PyMalelf_add_intconstant(module, var, value, name)      \
        do {                                                    \
                var = PyLong_FromLong(value);                   \
                if (var) {                                      \
                        PyModule_AddObject(module, name, var);  \
                } else {                                        \
                        Py_DECREF(module);                      \
                        INITERROR;                              \
                }                                               \
        } while(0)

#if defined(IS_PY3K)

static int
malelf_traverse(PyObject *self, visitproc visit, void *arg)
{
        Py_VISIT(GETSTATE(self)->error);
        return 0;
}

/**
 * Unref all members of Binary class
 */
static int
malelf_clear(PyObject *self)
{
        Py_CLEAR(GETSTATE(self)->error);
        return 0;
}

static struct PyModuleDef malelfmodule = {
        PyModuleDef_HEAD_INIT,
        "_malelf",
        NULL,
        sizeof (struct module_state),
        MalelfMethods,
        NULL,
        malelf_traverse,
        malelf_clear,
        NULL
};

#define INITERROR return NULL

PyObject *
PyInit__malelf(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_malelf(void)
#endif
{
#if defined(IS_PY3K)
        PyObject *m = PyModule_Create(&malelfmodule);
#else
        PyObject *m = Py_InitModule("_malelf", MalelfMethods);
#endif
        if (m == NULL)
                INITERROR;

        static void *PyMalelf_API[PyMalelf_API_pointers];
        PyObject *c_api_object;

        /* Initialize the C API pointer array */
        PyMalelf_API[PyMalelf_BinaryOpen_NUM] = (void *)PyMalelf_BinaryOpen;

        /* Create a Capsule containing the API pointer array's address */
        c_api_object = PyCapsule_New((void *)PyMalelf_API, "malelf._C_API", NULL);

        if (c_api_object != NULL)
                PyModule_AddObject(m, "_C_API", c_api_object);

        struct module_state *st = GETSTATE(m);

        if (PyType_Ready(&BinaryType) < 0) {
                return;
        }

        st->error = PyErr_NewException("_malelf.Error", NULL, NULL);
        if (NULL == st->error) {
                Py_DECREF(m);
                INITERROR;
        }

        Py_INCREF(st->error);
        PyModule_AddObject(m, "Error", st->error);

        PyMalelf_add_intconstant(m, st->FMT_ELF, MALELF_FMT_ELF, "FMT_ELF");
        PyMalelf_add_intconstant(m, st->FMT_FLAT, MALELF_FMT_FLAT, "FMT_FLAT");
        PyMalelf_add_intconstant(m, st->ELF, MALELF_ELF, "ELF");
        PyMalelf_add_intconstant(m, st->ELF32, MALELF_ELF32, "ELF32");
        PyMalelf_add_intconstant(m, st->ELF64, MALELF_ELF64, "ELF64");
        PyMalelf_add_intconstant(m, st->FLAT, MALELF_FLAT, "FLAT");
        PyMalelf_add_intconstant(m, st->FLAT32, MALELF_FLAT32, "FLAT32");
        PyMalelf_add_intconstant(m, st->FLAT64, MALELF_FLAT64, "FLAT64");
        PyMalelf_add_intconstant(m, st->ALLOC_NONE, MALELF_ALLOC_NONE, "ALLOC_NONE");
        PyMalelf_add_intconstant(m, st->ALLOC_MMAP, MALELF_ALLOC_MMAP, "ALLOC_MMAP");
        PyMalelf_add_intconstant(m, st->ALLOC_MALLOC, MALELF_ALLOC_MALLOC, "ALLOC_MALLOC");
        PyMalelf_add_intconstant(m, st->ORIGIN, MALELF_ORIGIN, "ORIGIN");
        PyMalelf_add_intconstant(m, st->MAGIC_BYTES, MALELF_MAGIC_BYTES, "MAGIC_BYTES");
        PyMalelf_add_intconstant(m, st->PAGE_SIZE, MALELF_PAGE_SIZE, "PAGE_SIZE");

        Py_INCREF(&BinaryType);
        PyModule_AddObject(m, "Binary", (PyObject *)&BinaryType);
#if defined(IS_PY3K)
        return m;
#endif
}
