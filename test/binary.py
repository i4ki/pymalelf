from malelf import Binary

if __name__ == "__main__":
    binary = Binary()
    try:
        binary.open("/bin/ls")
    except e:
        print e.errmsg
