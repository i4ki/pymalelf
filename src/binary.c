#include <Python.h>
#include <structmember.h>

#include "pydefines.h"
#include "malelfcompat.h"
#include "malelfmodule.h"
#include "binary.h"

#include <malelf/binary.h>
#include <malelf/error.h>

static int
Binary_traverse(Binary *self, visitproc visit, void *arg)
{
    Py_VISIT(self->fname);
    Py_VISIT(self->mem);

    return 0;
}

/**
 * Unref all members of Binary class
 */
static int Binary_clear(Binary *self)
{
        Py_CLEAR(self->fname);
        Py_CLEAR(self->mem);

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
        malelf_binary_close(self->_bin);
        free(self->_bin);
        _PyObject_GC_UNTRACK(self);
        Py_TYPE(self)->tp_free((PyObject *)self);
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

                printf("inside Binary_new()\n");

                malelf_binary_init(self->_bin);

                self->fname = PyUnicode_FromString("");
                if (NULL == self->fname)
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
        PyObject *tmp = self->fname;
        self->fname = PyString_FromString(self->_bin->fname);
        Py_INCREF(self->fname);
        if (tmp) {
                Py_XDECREF(tmp);
        }

        self->fd = self->_bin->fd;

        tmp = self->mem;
        self->mem = PyBytes_FromStringAndSize(self->_bin->mem,
                                              self->_bin->size);
        Py_INCREF(self->mem);
        if (tmp) {
                Py_XDECREF(tmp);
        }
        self->size = self->_bin->size;
        self->alloc_type = self->_bin->alloc_type;;
        self->arch = self->_bin->class;
}

static PyObject *
Binary_open(Binary *self, PyObject *args, PyObject *kwds)
{
        PyObject *fname = NULL, *pyresult;
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

        char *asciiname = PyString_AsString(fname);

        result = malelf_binary_open(self->_bin, asciiname);

        if (MALELF_SUCCESS != result) {
                const char *strerror = malelf_strerror(result);
                const char *errformat = "Failed to open file '%s'.\nErrorCode:"
                                        " %u, Message: %s\n";
                char buffer[PYMALELF_MAX_MSG_ERROR];
                snprintf(buffer,
                         PYMALELF_MAX_MSG_ERROR,
                         errformat,
                         asciiname,
                         result,
                         strerror);
                PyErr_Format(GETSTATE(self)->error, buffer);
                return NULL;
        }

        PyMalelf_refresh_binary(self);

        pyresult = PyLong_FromLong(0);

        return pyresult;
}

static PyMemberDef Binary_members[] = {
    {"fname", T_OBJECT_EX, offsetof(Binary, fname), 0, "input file name"},
    {"fd", T_INT, offsetof(Binary, fd), 0, "File descriptor"},
    {"mem", T_OBJECT_EX, offsetof(Binary, mem), 0, "memory bytes"},
    {"size", T_INT, offsetof(Binary, size), 0, "size of binary"},
    {"ehdr", T_OBJECT_EX, offsetof(Binary, ehdr), 0, "Ehdr"},
    {"phdr", T_OBJECT_EX, offsetof(Binary, phdr), 0, "Phdr"},
    {"shdr", T_OBJECT_EX, offsetof(Binary, shdr), 0, "Shdr"},
    {"alloc_type", T_INT, offsetof(Binary, alloc_type), 0, "Allocation type"},
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
PyTypeObject BinaryType = {
  PyVarObject_HEAD_INIT(NULL, 0)
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
    (traverseproc)Binary_traverse,   /* tp_traverse */
    (inquiry)Binary_clear,           /* tp_clear */
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
