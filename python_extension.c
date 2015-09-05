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
    if (!PyArg_ParseTuple(args, "ss", &source, &key))
        return NULL;

    /* TODO: throw exception for too long string */

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
    if (!PyArg_ParseTuple(args, "ss", &cyphertext, &key))
        return NULL;

    /* TODO: throw exception for too long string */

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
