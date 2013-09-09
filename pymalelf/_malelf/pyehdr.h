#ifndef MALELF_MODULE_EHDR_H
#define MALELF_MODULE_EHDR_H

#include <Python.h>
#include "malelfcompat.h"

#include <malelf/ehdr.h>

typedef struct {
        PyObject_HEAD
        _u32 name;
        _u32 value;
        PyObject *meaning;
} EhdrTable;

typedef struct {
        PyObject_HEAD
        /* Type-specific fields go here. */
        PyObject *ident;      /* Magic number and other info */
        PyObject *type;      /* Object file type */
        PyObject *machine;   /* Architecture */
        PyObject *version;   /* Object file version */
        _u32 entry;           /* Entry point virtual address */
        _u32 phoff;           /* Program header table file offset */
        _u32 shoff;           /* Section header table file offset */
        _u32 flags;           /* Processor-specific flags */
        _u32 ehsize;          /* ELF header size in bytes */
        _u32 phentsize;       /* Program header table entry size */
        _u32 phnum;           /* Program header table entry count */
        _u32 shentsize;       /* Section header table entry size */
        _u32 shnum;           /* Section header table entry count */
        _u32 shstrndx;        /* Section header string table index */

        MalelfEhdr *_ehdr;    /* Malelficus instance of EHDR */
} Ehdr;

extern PyTypeObject EhdrType;
extern PyTypeObject EhdrTableType;
extern Ehdr* PyEhdr_create(MalelfEhdr *ehdr);

#if !defined(IS_PY3K)
extern struct module_state _state;
#endif

#endif /* MALELF_MODULE_EHDR_H */
