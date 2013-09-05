#!/usr/bin/env python

import malelf

if __name__ == "__main__":
    try:
        b = malelf.Binary()
        b.open("/798798798")
        print b.fname
    except malelf.Error as e:
        print "Error code: %d" % e.code
