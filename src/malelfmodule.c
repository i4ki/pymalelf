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
        "malelf",
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
PyInit_malelf(void)

#else
#define INITERROR return

PyMODINIT_FUNC
initmalelf(void)
#endif
{
#if defined(IS_PY3K)
        PyObject *m = PyModule_Create(&malelfmodule);
#else
        PyObject *m = Py_InitModule("malelf", MalelfMethods);
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

        st->error = PyErr_NewException("malelf.Error", NULL, NULL);
        if (NULL == st->error) {
                Py_DECREF(m);
                INITERROR;
        }

        Py_INCREF(st->error);
        PyModule_AddObject(m, "Error", st->error);

        Py_INCREF(&BinaryType);
        PyModule_AddObject(m, "Binary", (PyObject *)&BinaryType);
#if defined(IS_PY3K)
        return m;
#endif
}
