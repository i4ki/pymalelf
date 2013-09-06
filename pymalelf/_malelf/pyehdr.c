#include <Python.h>
#include <structmember.h>

#include <malelf/ehdr.h>
#include <malelf/error.h>

#include "malelfcompat.h"
#include "malelfmodule.h"
#include "pyehdr.h"

STATESTUB

#define RAISE_MALELF_ERROR(self) do {                   \
        const char *strerror = malelf_strerror(result); \
        PyErr_Format(GETSTATE(self)->error,             \
                     "%s", strerror);                   \
        PyObject *code = PyLong_FromLong(result);       \
        PyObject_SetAttrString(GETSTATE(self)->error,      \
                        "code", code);                  \
        return NULL;                                    \
        } while(0)

#define CHECK_ERROR(call) do {                                  \
        if (MALELF_SUCCESS != (result = call)) {                \
        RAISE_MALELF_ERROR(self);                               \
        }} while(0)

static PyObject *
Ehdr_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
        Ehdr *self;

        self = (Ehdr *)type->tp_alloc(type, 0);
        if (NULL != self) {
                Py_INCREF(Py_None);
                self->ident = Py_None;

                Py_INCREF(Py_None);
                self->type = Py_None;

                Py_INCREF(Py_None);
                self->machine = Py_None;

                Py_INCREF(Py_None);
                self->version = Py_None;

                self->entry = 0;
                self->phoff = 0;
                self->shoff = 0;
                self->flags = 0;
                self->ehsize = 0;
                self->phentsize = 0;
                self->phnum = 0;
                self->shentsize = 0;
                self->shnum = 0;
                self->shstrndx = 0;
        }

        return (PyObject *)self;
}

static int
Ehdr_init(Ehdr *self, PyObject *args, PyObject *kwds)
{
        PyObject *ident = NULL, *tmp = NULL;
/*        PyObject *type = NULL;
        PyObject *machine = NULL;
        PyObject *version = NULL;*/
        _u32 entry = 0;
        _u32 phoff = 0;
        _u32 shoff = 0;
        _u32 flags = 0;
        _u32 ehsize = 0;
        _u32 phentsize = 0;
        _u32 phnum = 0;
        _u32 shentsize = 0;
        _u32 shnum = 0;
        _u32 shstrndx = 0;

        static char *kwlist[] = {"ident",
/*                                 "type",
                                 "machine",
                                 "version",*/
                                 "entry",
                                 "phoff",
                                 "shoff",
                                 "flags",
                                 "ehsize",
                                 "phentsize",
                                 "phnum",
                                 "shentsize",
                                 "shnum",
                                 "shstrndx",
                                 NULL};

        if (! PyArg_ParseTupleAndKeywords(args, kwds, "|Oiiiiiiiiii", kwlist,
                                          &ident, /*&type, &machine, &version,*/
                                          &entry, &phoff, &shoff, &flags,
                                          &ehsize, &phentsize, &phnum, &shentsize,
                                          &shnum, &shstrndx)) {
                return -1;
        }

        if (ident) {
                if (! PyUnicode_Check(ident) && ! PyString_Check(ident)) {
                        PyErr_SetString(PyExc_TypeError,
                                        "The ident attribute value must be a string");
                        return -1;
                }

                Py_ssize_t ident_sz = PyString_Size(ident);

                if (ident_sz > 0x10) {
                        PyErr_Format(PyExc_TypeError,
                                        "The ident attribute value must be a " \
                                        "string of length lower than %d " \
                                        "characters.", 0x10);
                        return -1;
                }

                tmp = self->ident;
                Py_INCREF(ident);
                self->ident = ident;
                Py_XDECREF(tmp);
        }

/*        self->type = type;
        self->machine = machine;
        self->version = version; */
        self->entry = entry;
        self->phoff = phoff;
        self->shoff = shoff;
        self->flags = flags;
        self->ehsize = ehsize;
        self->phentsize = phentsize;
        self->phnum = phnum;
        self->shentsize = shentsize;
        self->shnum = shnum;
        self->shstrndx = shstrndx;

        return 0;
}

static int
Ehdr_traverse(Ehdr *self, visitproc visit, void *arg)
{
        Py_VISIT(self->ident);
        Py_VISIT(self->type);
        Py_VISIT(self->machine);
        Py_VISIT(self->version);
        return 0;
}

/**
 * Unref all members of Binary class
 */
static int
Ehdr_clear(Ehdr *self)
{
        Py_CLEAR(self->ident);
        Py_CLEAR(self->type);
        Py_CLEAR(self->machine);
        Py_CLEAR(self->version);
        return 0;
}

/**
 * Destructor
 * First, unref all members of Binary class and then deallocate memory
 */
static void
Ehdr_dealloc(Ehdr *self)
{
        Ehdr_clear(self);
        Py_TYPE(self)->tp_free((PyObject *)self);
}


/**
 * Ehdr member's table
 */
static PyMemberDef Ehdr_members[] = {
        {"ident", T_OBJECT_EX, offsetof(Ehdr, ident), 0, "Magic number and other info"},
        {"type", T_INT, offsetof(Ehdr, type), 0, "Object file type"},
        {"machine", T_INT, offsetof(Ehdr, machine), 0, "Architecture"},
        {"version", T_INT, offsetof(Ehdr, version), 0, "Object file version"},
        {"entry", T_INT, offsetof(Ehdr, entry), 0, " Entry point virtual address"},
        {"phoff", T_INT, offsetof(Ehdr, phoff), 0, "Program header table file offset"},
        {"shoff", T_INT, offsetof(Ehdr, shoff), 0, "Section header table file offset"},
        {"flags", T_INT, offsetof(Ehdr, flags), 0, "Processor-specific flags"},
        {"ehsize", T_INT, offsetof(Ehdr, ehsize), 0, "ELF header size in bytes"},
        {"phentsize", T_INT, offsetof(Ehdr, phentsize), 0, "Program header table entry size"},
        {"phnum", T_INT, offsetof(Ehdr, phnum), 0, "Program header table entry count"},
        {"shentsize", T_INT, offsetof(Ehdr, shentsize), 0, "Section header table entry size"},
        {"shnum", T_INT, offsetof(Ehdr, shentsize), 0, "Section header table entry count"},
        {"shstrndx", T_INT, offsetof(Ehdr, shstrndx), 0, "Section header string table index"},
        {NULL}  /* Sentinel */
};

/**
 * Ehdr method's table
 */
static PyMethodDef Ehdr_methods[] = {
        {NULL}  /* Sentinel */
};

/**
 * malelf.Binary class definition
 */
PyTypeObject EhdrType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "_malelf.Ehdr",               /*tp_name*/
        sizeof(Ehdr),                 /*tp_basicsize*/
        0,                            /*tp_itemsize*/
        (destructor)Ehdr_dealloc,     /*tp_dealloc*/
        0,                            /*tp_print*/
        0,                            /*tp_getattr*/
        0,                            /*tp_setattr*/
        0,                            /*tp_compare*/
        0,                            /*tp_repr*/
        0,                            /*tp_as_number*/
        0,                            /*tp_as_sequence*/
        0,                            /*tp_as_mapping*/
        0,                            /*tp_hash */
        0,                            /*tp_call*/
        0,                            /*tp_str*/
        0,                            /*tp_getattro*/
        0,                            /*tp_setattro*/
        0,                            /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC, /*tp_flags*/
        "Ehdr instance",              /* tp_doc */
        (traverseproc)Ehdr_traverse,  /* tp_traverse */
        (inquiry)Ehdr_clear,          /* tp_clear */
        0,		              /* tp_richcompare */
        0,		              /* tp_weaklistoffset */
        0,		              /* tp_iter */
        0,		              /* tp_iternext */
        Ehdr_methods,                 /* tp_methods */
        Ehdr_members,                 /* tp_members */
        0,                            /* tp_getset */
        0,                            /* tp_base */
        0,                            /* tp_dict */
        0,                            /* tp_descr_get */
        0,                            /* tp_descr_set */
        0,                            /* tp_dictoffset */
        (initproc)Ehdr_init,          /* tp_init */
        0,                            /* tp_alloc */
        Ehdr_new,                     /* tp_new */
};


/**
 * EhdrTable class
 *
 * Will be used to store information about Ehdr fields.
 */

static PyObject *
EhdrTable_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
        EhdrTable *self;

        self = (EhdrTable *)type->tp_alloc(type, 0);
        if (NULL != self) {
                Py_INCREF(Py_None);
                self->name = Py_None;

                Py_INCREF(Py_None);
                self->value = Py_None;

                Py_INCREF(Py_None);
                self->meaning = Py_None;
        }

        return (PyObject *)self;
}

static int
EhdrTable_init(EhdrTable *self, PyObject *args, PyObject *kwds)
{
        PyObject *name = NULL;
        PyObject *value = NULL;
        PyObject *meaning = NULL;
        PyObject *tmp;

        static char *kwlist[] = {
                "name", "value", "meaning", NULL
        };

        if (! PyArg_ParseTupleAndKeywords(args, kwds, "OO|OOO", kwlist,
                                          &name, &value, &meaning)) {
                return -1;
        }

        if (!name || !PyString_Check(name)) {
                PyErr_SetString(PyExc_TypeError,
                                "The attribute name is of type string and required.");
                return -1;
        }

        if (!value || ! PyString_Check(value)) {
                PyErr_SetString(PyExc_TypeError,
                                "The attribute value is of type string and required");
                return -1;
        }

        tmp = self->name;
        Py_INCREF(name);
        self->name = name;
        Py_XDECREF(tmp);

        tmp = self->value;
        Py_INCREF(value);
        self->value = value;
        Py_XDECREF(tmp);

        if (meaning) {
                if (! PyString_Check(meaning)) {
                        PyErr_SetString(PyExc_TypeError,
                                        "The attribute meaning must be a string");
                        return -1;
                }

                tmp = self->meaning;
                Py_INCREF(meaning);
                self->meaning = meaning;
                Py_XDECREF(tmp);
        }

        return 0;
}

static int
EhdrTable_traverse(EhdrTable *self, visitproc visit, void *arg)
{
        Py_VISIT(self->name);
        Py_VISIT(self->value);
        Py_VISIT(self->meaning);

        return 0;
}

/**
 * Unref all members of Binary class
 */
static int
EhdrTable_clear(EhdrTable *self)
{
        Py_CLEAR(self->name);
        Py_CLEAR(self->value);
        Py_CLEAR(self->meaning);

        return 0;
}

/**
 * Destructor
 * First, unref all members of Binary class and then deallocate memory
 */
static void
EhdrTable_dealloc(EhdrTable *self)
{
        EhdrTable_clear(self);
        Py_TYPE(self)->tp_free((PyObject *)self);
}

/**
 * EhdrTable member's table
 */
static PyMemberDef EhdrTable_members[] = {
        {"name", T_OBJECT_EX, offsetof(EhdrTable, name), 0, "Name of field"},
        {"value", T_OBJECT_EX, offsetof(EhdrTable, value), 0, "Value of field"},
        {"meaning", T_OBJECT_EX, offsetof(EhdrTable, meaning), 0, "Meaning of field"},
        {NULL}  /* Sentinel */
};

/**
 * EhdrTable method's table
 */
static PyMethodDef EhdrTable_methods[] = {
        {NULL}  /* Sentinel */
};

/**
 * malelf.Binary class definition
 */
PyTypeObject EhdrTableType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "_malelf.EhdrTable",          /*tp_name*/
        sizeof(EhdrTable),            /*tp_basicsize*/
        0,                            /*tp_itemsize*/
        (destructor)EhdrTable_dealloc,/*tp_dealloc*/
        0,                            /*tp_print*/
        0,                            /*tp_getattr*/
        0,                            /*tp_setattr*/
        0,                            /*tp_compare*/
        0,                            /*tp_repr*/
        0,                            /*tp_as_number*/
        0,                            /*tp_as_sequence*/
        0,                            /*tp_as_mapping*/
        0,                            /*tp_hash */
        0,                            /*tp_call*/
        0,                            /*tp_str*/
        0,                            /*tp_getattro*/
        0,                            /*tp_setattro*/
        0,                            /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC, /*tp_flags*/
        "Ehdr instance",              /* tp_doc */
        (traverseproc)EhdrTable_traverse,  /* tp_traverse */
        (inquiry)EhdrTable_clear,     /* tp_clear */
        0,		              /* tp_richcompare */
        0,		              /* tp_weaklistoffset */
        0,		              /* tp_iter */
        0,		              /* tp_iternext */
        EhdrTable_methods,            /* tp_methods */
        EhdrTable_members,            /* tp_members */
        0,                            /* tp_getset */
        0,                            /* tp_base */
        0,                            /* tp_dict */
        0,                            /* tp_descr_get */
        0,                            /* tp_descr_set */
        0,                            /* tp_dictoffset */
        (initproc)EhdrTable_init,     /* tp_init */
        0,                            /* tp_alloc */
        NULL,                         /* tp_new */
};


/** PyMalelf Ehdr API */

PyObject* PyEhdr_create(MalelfEhdr *mehdr)
{
        MalelfEhdrTable etable;
        EhdrTable *tbl;
        PyObject *args = NULL;
        PyObject *tmp;
        _u32 result = MALELF_SUCCESS;
        Ehdr *ehdr;

        args = Py_BuildValue("");
        ehdr = (Ehdr *) PyObject_CallObject((PyObject *) &EhdrType, args);

        if (! ehdr) {
                PyErr_NoMemory();
                return NULL;
        }

        ehdr->ident = PyString_FromStringAndSize((const char *) MALELF_ELF_FIELD(mehdr,
                                                                  e_ident,
                                                                  result),
                                                 0x10);
        if (MALELF_SUCCESS != result) {
                RAISE_MALELF_ERROR(self);
        }

        Py_INCREF(ehdr->ident);

        CHECK_ERROR(malelf_ehdr_get_type(mehdr, &etable));
        args = Py_BuildValue("sss", etable.name, etable.value, etable.meaning);
        tmp = PyObject_CallObject((PyObject *) &EhdrTableType, args);
        Py_INCREF(tmp);
        Py_XDECREF(args);
        ehdr->type = tmp;

        CHECK_ERROR(malelf_ehdr_get_machine(mehdr, &etable));
        args = Py_BuildValue("sss", etable.name, etable.value, etable.meaning);
        tmp = PyObject_CallObject((PyObject *) &EhdrTableType, args);
        Py_INCREF(tmp);
        Py_XDECREF(args);

        ehdr->machine = tmp;

        CHECK_ERROR(malelf_ehdr_get_version(mehdr, &etable));
        args = Py_BuildValue("sss", etable.name, etable.value, etable.meaning);
        tmp = PyObject_CallObject((PyObject *) &EhdrTableType, args);
        Py_INCREF(tmp);
        Py_XDECREF(args);

        ehdr->version = tmp;

        CHECK_ERROR(malelf_ehdr_get_entry(mehdr, &(ehdr->entry)));
        CHECK_ERROR(malelf_ehdr_get_phoff(mehdr, &(ehdr->phoff)));
        CHECK_ERROR(malelf_ehdr_get_phnum(mehdr, &(ehdr->phnum)));
        CHECK_ERROR(malelf_ehdr_get_phentsize(mehdr, &(ehdr->phentsize)));
        CHECK_ERROR(malelf_ehdr_get_flags(mehdr, &(ehdr->flags)));
        CHECK_ERROR(malelf_ehdr_get_ehsize(mehdr, &(ehdr->ehsize)));
        CHECK_ERROR(malelf_ehdr_get_shoff(mehdr, &(ehdr->shentsize)));
        CHECK_ERROR(malelf_ehdr_get_shnum(mehdr, &(ehdr->shnum)));
        CHECK_ERROR(malelf_ehdr_get_shentsize(mehdr, &(ehdr->shentsize)));
        CHECK_ERROR(malelf_ehdr_get_shstrndx(mehdr, &(ehdr->shstrndx)));
}
