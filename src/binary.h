#ifndef MALELF_MODULE_BINARY_H
#define MALELF_MODULE_BINARY_H

#include <malelf/binary.h>

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

extern PyTypeObject BinaryType;

#endif /* MALELF_MODULE_BINARY_H */
