#include <Python.h>
#include <structmember.h> /* Python PyObject structure accessors */

#define MALELF_MODULE
#include "malelfmodule.h"

#include <malelf/binary.h>
#include <malelf/error.h>

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#else
#undef IS_PY3K
#endif

typedef struct {
        PyObject_HEAD
        /* Type-specific fields go here. */
        PyObject *fname;   /* Binary filename */
        PyObject *bkpfile; /* Filename of backup'ed file in case of
                              write operations */
        _i32 fd;           /* Binary file descriptor */
        PyObject *mem;     /* Binary content */
        _u32 size;         /* Binary size */
        PyObject *ehdr;    /* ELF Header */
        PyObject *phdr;    /* Elf Program Headers */
        PyObject *shdr;    /* Elf Section Headers */
        _u8 alloc_type;    /* System function used to allocate memory */
        _u32 arch;

        /* Internal pymalelf field */
        MalelfBinary *_bin;
} Binary;

struct module_state {
        /* Malelficus Exception Handler */
        PyObject *error;
        Binary *binary;
};

/* Python3 deprecated the use of globals to store local
   variables of module.
   Remember to use GETSTATE macro to PyMalelf be compatible with
   Python >= 2.7
*/
#if defined(IS_PY3K)
#        define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#        define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static int
Binary_traverse(Binary *self, visitproc visit, void *arg)
{
    Py_VISIT(self->fname);
    Py_VISIT(self->bkpfile);
    Py_VISIT(self->fd);
    Py_VISIT(self->mem);
    Py_VISIT(self->size);
    Py_VISIT(self->ehdr);
    Py_VISIT(self->phdr);
    Py_VISIT(self->shdr);
    Py_VISIT(self->alloc_type);
    Py_VISIT(self->arch);

    return 0;
}

/**
 * Unref all members of Binary class
 */
static int Binary_clear(Binary *self)
{
        Py_CLEAR(self->fname);
        Py_CLEAR(self->bkpfile);
        Py_CLEAR(self->fd);
        Py_CLEAR(self->mem);
        Py_CLEAR(self->size);
        Py_CLEAR(self->ehdr);
        Py_CLEAR(self->phdr);
        Py_CLEAR(self->shdr);
        Py_CLEAR(self->alloc_type);
        Py_CLEAR(self->arch);

        return 0;
}

/**
 * Destructor
 * First, unref all members of Binary class and then deallocate memory
 */
static void
Binary_dealloc(Binary *self)
{
        Binary_clear(self);
        free(self->_bin);
        self->ob_type->tp_free((PyObject *) self);
}

static PyObject *
Binary_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
        Binary *self;

        self = (Binary *)type->tp_alloc(type, 0);
        if (NULL != self) {
                self->_bin = malloc(sizeof (MalelfBinary));

                if (!self->_bin) {
                        PyErr_NoMemory();
                        return NULL;
                }

                malelf_binary_init(self->_bin);

                self->fname = PyUnicode_FromString("");
                if (NULL == self->fname)
                {
                        Py_DECREF(self);
                        return NULL;
                }

                self->bkpfile = PyUnicode_FromString("");
                if (NULL == self->bkpfile)
                {
                        Py_DECREF(self);
                        return NULL;
                }

                self->mem = PyBytes_FromString("");
                if (NULL == self->mem) {
                        Py_DECREF(self);
                        return NULL;
                }

                self->fd = self->_bin->fd;
                self->size = self->_bin->size;
                self->ehdr = NULL;
                self->phdr = NULL;
                self->shdr = NULL;

                self->alloc_type = self->_bin->alloc_type;
        }

        return (PyObject *)self;
}

static int
Binary_init(Binary *self, PyObject *args, PyObject *kwds)
{
        PyObject *fname = NULL, *tmp;
        _u8 alloc_type = 0;
        _u32 arch = 0;

        static char *kwlist[] = {"fname", "alloc_type", "arch", NULL};

        if (! PyArg_ParseTupleAndKeywords(args, kwds, "|Oii", kwlist,
                                          &fname, &alloc_type, &arch)) {
                return -1;
        }

        if (fname) {
                if (! PyString_Check(fname)) {
                        PyErr_SetString(PyExc_TypeError,
                                        "The fname attribute value must be a string");
                        return -1;
                }
                tmp = self->fname;
                Py_INCREF(fname);
                self->fname = fname;
                Py_XDECREF(tmp);
        }

        return 0;
}

static void
PyMalelf_refresh_binary(Binary *self)
{
        self->fname = PyString_FromString(self->_bin->fname);
        self->bkpfile = PyString_FromString(self->_bin->bkpfile);
        self->fd = self->_bin->fd;
        self->mem = PyBytes_FromString(self->_bin->mem);
        self->size = self->_bin->size;
        self->alloc_type = self->_bin->alloc_type;;
        self->arch = self->_bin->class;
}

static PyObject *
Binary_open(Binary *self, PyObject *args, PyObject *kwds)
{
        PyObject *fname = NULL, *pyresult;
        const char *format = "%s %s";
        _u32 result;

        static char *kwlist[] = {"fname", NULL};

        if (! PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist,
                                          &fname)) {
                return NULL;
        }

        if (!fname) {
                return NULL;
        }

        if (! PyString_Check(fname)) {
                PyErr_SetString(PyExc_TypeError,
                                "The fname attribute value must be a string");
                return NULL;
        }

        if (!self->_bin) {
                self->_bin = malloc(sizeof (MalelfBinary));
        }

        malelf_binary_init(self->_bin);

        result = malelf_binary_open(self->_bin, PyString_AsString(fname));

        if (MALELF_SUCCESS != result) {
                PyErr_SetString(PyExc_TypeError,
                                "Failed to open binary file");
                return NULL;
        }

        PyMalelf_refresh_binary(self);

        pyresult = PyLong_FromLong(0);

        return pyresult;
}

static PyMemberDef Binary_members[] = {
    {"fname", T_OBJECT_EX, offsetof(Binary, fname), 0,
     "input file name"},
    {"bkpfile", T_OBJECT_EX, offsetof(Binary, bkpfile), 0,
     "backup file for write operations"},
    {"mem", T_OBJECT_EX, offsetof(Binary, mem), 0, "memory bytes"},
    {"size", T_INT, offsetof(Binary, size), 0, "size of binary"},
    {"ehdr", T_OBJECT_EX, offsetof(Binary, ehdr), 0, "Ehdr"},
    {"phdr", T_OBJECT_EX, offsetof(Binary, phdr), 0, "Phdr"},
    {"shdr", T_OBJECT_EX, offsetof(Binary, shdr), 0, "Shdr"},
    {"alloc_type", T_INT, offsetof(Binary, alloc_type), 0,
     "Allocation type"},
    {"arch", T_INT, offsetof(Binary, arch), 0, "Binary class"},
    {NULL}  /* Sentinel */
};

static PyMethodDef Binary_methods[] = {
    {"open", (PyCFunction)Binary_open, METH_VARARGS|METH_KEYWORDS,
     "Opens a new binary"
     },
    {NULL}  /* Sentinel */
};

/**
 * malelf.Binary class definition
 */
static PyTypeObject BinaryType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "malelf.Binary",             /*tp_name*/
    sizeof(Binary),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Binary_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC, /*tp_flags*/
    "Binary objects",           /* tp_doc */
    0,   /* tp_traverse */
    0,           /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Binary_methods,             /* tp_methods */
    Binary_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Binary_init,      /* tp_init */
    0,                         /* tp_alloc */
    Binary_new,                 /* tp_new */
};

static int malelf_clear(PyObject *m)
{
        Py_CLEAR(GETSTATE(m)->error);
        return 0;
}

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

static struct PyModuleDef malelfmodule = {
        PyModuleDef_HEAD_INIT,
        "malelf",
        NULL,
        sizeof(struct module_state),
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

        BinaryType.tp_new = PyType_GenericNew;
        if (PyType_Ready(&BinaryType) < 0) {
                return;
        }

        st->error = PyErr_NewException("malelf.error", NULL, NULL);
        if (NULL == st->error) {
                Py_DECREF(m);
                INITERROR;
        }

        Py_INCREF(st->error);
        PyModule_AddObject(m, "error", st->error);

        Py_INCREF(&BinaryType);
        PyModule_AddObject(m, "Binary", (PyObject *)&BinaryType);
#if defined(IS_PY3K)
        return m;
#endif
}

#if 0
int
main(int argc, char *argv[])
{
        /* Add a built-in module, before Py_Initialize */
        PyImport_AppendInittab("malelf", PyInit_malelf);

        /* Pass argv[0] to the Python interpreter */
        Py_SetProgramName(argv[0]);

        /* Initialize the Python interpreter.  Required. */
        Py_Initialize();

        /* Optionally import the module; alternatively,
           import can be deferred until the embedded script
           imports it. */
        PyImport_ImportModule("malelf");

        return 0;
}
#endif
