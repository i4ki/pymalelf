from distutils.core import setup
from distutils.extension import Extension

malelfmodule = Extension('malelf',
                         define_macros=[('MALELF_MAJOR_VERSION', '1'),
                                        ('MALELF_MINOR_VERSION', '0')],
                         include_dirs=['/usr/local/include'],
                         libraries=['malelf'],
                         library_dirs=['/usr/lib'],
                         sources=['src/malelfmodule.c',
                                  'src/binary.c'])

setup(name='PyMalelf',
      version='1.0',
      description='Malelficus Python Bindings',
      author='Tiago Natel de Moura',
      author_email='natel@secplus.com.br',
      url='https://github.com/SecPlus/PyMalelf',
      long_description='''
Just the Python Bindings of libmalelf
''',
      ext_modules=[malelfmodule])
