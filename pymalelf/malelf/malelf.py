import _malelf
import sys


class Binary(_malelf.Binary):
    def __init__(self, *kargs, **kwargs):
        super(Binary, self).__init__(*kargs, **kwargs)

# Get a reference of the internal _malelf exception handler
setattr(sys.modules[__name__], "Error", _malelf.Error)

for _attr in dir(_malelf):
    if _attr.isupper():
        val = getattr(_malelf, _attr)
        setattr(sys.modules[__name__], _attr, val)
