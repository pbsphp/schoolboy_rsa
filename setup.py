from distutils.core import setup, Extension

module = Extension('schoolboy_rsa',
                    sources = ['schoolboy_rsa.c', 'python_extension.c'],
                    libraries = ['m', 'gmp'])

setup(  name='schoolboy_rsa',
        version='1.0',
        description='Yet another stupid RSA implementation.',
        ext_modules=[module])
