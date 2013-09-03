#ifndef Py_MALELFMODULE_H
#define Py_MALELFMODULE_H

#include <malelf/types.h>
#include <malelf/binary.h>

#include "malelfcompat.h"
#include "binary.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Header file for malelfmodule */

/* C API functions */
#define PyMalelf_BinaryOpen_NUM 0
#define PyMalelf_BinaryOpen_RETURN _u32
#define PyMalelf_BinaryOpen_PROTO (MalelfBinary *bin, char *fname)

/* Total number of C API pointers */
#define PyMalelf_API_pointers 1

#ifdef MALELF_MODULE
/* This section is used when compiling malelfmodule.c */

static PyMalelf_BinaryOpen_RETURN PyMalelf_BinaryOpen PyMalelf_BinaryOpen_PROTO;

#else
/* This section is used in modules that use malelfmodule's API */

static void **PyMalelf_API;

#define PyMalelf_BinaryOpen \
 (*(PyMalelf_BinaryOpen_RETURN (*)PyMalelf_BinaryOpen_PROTO) PyMalelf_API[PyMalelf_BinaryOpen_NUM])

/* Return -1 on error, 0 on success.
 * PyCapsule_Import will set an exception if there's an error.
 */
static int
import_malelf(void)
{
    PyMalelf_API = (void **)PyCapsule_Import("malelf._C_API", 0);
    return (PyMalelf_API != NULL) ? 0 : -1;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined(Py_MALELFMODULE_H) */
