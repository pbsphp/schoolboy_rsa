#include <Python.h>

#include <stdio.h>
#include <stdlib.h>

#include "schoolboy_rsa.h"


/* Encrypts python string with (python string) key */
static PyObject *schoolboy_rsa_encrypt(
    PyObject *self, PyObject *args)
{
    const char *source;
    const char *key;
    char cyphertext[KEY_STRING_SIZE];

    /* Try to parse function arguments */
    Py_ssize_t source_length;
    Py_ssize_t key_length;
    if (!PyArg_ParseTuple(args, "s#s#", &source, &source_length,
                            &key, &key_length)) {
        return NULL;
    }

    /* Throw exception if source or key is too long */
    if ((int) source_length >= MAX_SOURCE_SIZE) {
        PyErr_SetString(PyExc_ValueError, "Source is too long");
        return NULL;
    }
    if ((int) key_length >= KEY_STRING_SIZE) {
        PyErr_SetString(PyExc_ValueError, "Key is too long");
        return NULL;
    }

    /* Encrypt */
    sb_encrypt(cyphertext, source, key);

    return Py_BuildValue("s", cyphertext);
}


/* Decrypts python string with (python string) key */
static PyObject *schoolboy_rsa_decrypt(
    PyObject *self, PyObject *args)
{
    const char *cyphertext;
    const char *key;
    char source[MAX_SOURCE_SIZE];

    /* Try to parse function arguments */
    Py_ssize_t cyphertext_length;
    Py_ssize_t key_length;
    if (!PyArg_ParseTuple(  args, "s#s#", &cyphertext, &cyphertext_length,
                            &key, &key_length)) {
        return NULL;
    }

    /* Throw exception if ciphertext or key is too long */
    /* FIXME: how long cyphertext allowed? */
    if ((int) key_length >= KEY_STRING_SIZE) {
        PyErr_SetString(PyExc_ValueError, "Key is too long");
        return NULL;
    }

    /* Encrypt */
    sb_decrypt(source, cyphertext, key);

    return Py_BuildValue("s", source);
}


/* Generates and returns key pair (public, private) */
static PyObject *schoolboy_rsa_generate_keys(
    PyObject *self, PyObject *args)
{
    char pubkey[KEY_STRING_SIZE];
    char prvkey[KEY_STRING_SIZE];

    sb_generate_keys(pubkey, prvkey);

    return Py_BuildValue("ss", pubkey, prvkey);
}


/* Module's method table */
static PyMethodDef schoolboy_rsa_methods[] = {
    {"encrypt",  schoolboy_rsa_encrypt, METH_VARARGS,
        "Encrypts string with public key"},
    {"decrypt",  schoolboy_rsa_decrypt, METH_VARARGS,
        "Decrypts string with private key"},
    {"generate_keys",  schoolboy_rsa_generate_keys, METH_VARARGS,
        "Generates key pair: (public, private)"},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initschoolboy_rsa()
{
    Py_InitModule("schoolboy_rsa", schoolboy_rsa_methods);
}


int main(int argc, char *argv[])
{
    Py_SetProgramName(argv[0]);
    Py_Initialize();
    initschoolboy_rsa();
    return 0;
}
