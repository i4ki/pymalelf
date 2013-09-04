#ifndef MALELF_MODULE_COMPAT_H
#define MALELF_MODULE_COMPAT_H

#if PY_MAJOR_VERSION >= 3
#        define IS_PY3K
#else
#        undef IS_PY3K
#endif

#ifndef PyVarObject_HEAD_INIT
    #define PyVarObject_HEAD_INIT(type, size) \
        PyObject_HEAD_INIT(type) size,
#endif

#ifndef Py_TYPE
    #define Py_TYPE(ob) (((PyObject*)(ob))->ob_type)
#endif

/* Python3 deprecated the use of globals to store local
   variables of module.
   Remember to use GETSTATE macro to PyMalelf be compatible with
   Python >= 2.7
*/
#if defined(IS_PY3K)
#        define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#        define STATESTUB

#        define PyString_Check PyUnicode_Check
#        define PyString_FromString PyUnicode_FromString
#else
#        define GETSTATE(m) (&_state)
#        define STATESTUB struct module_state _state;
#endif

#endif /* MALELF_MODULE_COMPAT_H */
