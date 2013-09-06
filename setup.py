from distutils.core import setup
from distutils.extension import Extension

malelfmodule = Extension('_malelf',
                         define_macros=[('MALELF_MAJOR_VERSION', '1'),
                                        ('MALELF_MINOR_VERSION', '0')],
                         include_dirs=['/usr/local/include'],
                         libraries=['malelf'],
                         library_dirs=['/usr/lib'],
                         sources=['pymalelf/_malelf/malelfmodule.c',
                                  'pymalelf/_malelf/pyehdr.c',
                                  'pymalelf/_malelf/pybinary.c'])

setup(name='PyMalelf',
      version='1.0',
      description='Malelficus Python Bindings',
      author='Tiago Natel de Moura',
      author_email='natel@secplus.com.br',
      url='https://github.com/SecPlus/pymalelf',
      scripts=['bin/binary.py'],
      long_description='''
Just the Python Bindings of libmalelf
''',
      ext_modules=[malelfmodule],
      package_dir={'': 'pymalelf/malelf'},
      py_modules=['malelf'])
