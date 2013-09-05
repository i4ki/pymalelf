===========
PyMalelf - Python bindings to libmalelf
===========

PyMalelf provides easy access to libmalelf C API.
You can use this to rapidly automate tasks or use the benefits of
a high level language to analyse malwares.

    #!/usr/bin/env python

    import malelf

    binary = malelf.Binary()
    binary.open("/bin/ls")
    print "size = " + binary.size
    ehdr = binary.ehdr()
    print "binary entry point: %08x" % ehdr.entry
    print "number of segments: %d" % ehdr.nphdr


Installation
=========

   $ pip install pymalelf


https://github.com/SecPlus/pymalelf
https://github.com/SecPlus/libmalelf
