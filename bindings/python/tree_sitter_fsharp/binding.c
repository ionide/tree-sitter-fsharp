#include <Python.h>

typedef struct TSLanguage TSLanguage;

TSLanguage *tree_sitter_fsharp(void);
TSLanguage *tree_sitter_fsharp_signature(void);

static PyObject *_binding_language_fsharp(PyObject *Py_UNUSED(self),
                                          PyObject *Py_UNUSED(args)) {
  return PyLong_FromVoidPtr(tree_sitter_fsharp());
}

static PyObject *_binding_language_fsharp_signature(PyObject *Py_UNUSED(self),
                                                    PyObject *Py_UNUSED(args)) {
  return PyLong_FromVoidPtr(tree_sitter_fsharp_signature());
}

static PyMethodDef methods[] = {
    {"fsharp", _binding_language_fsharp, METH_NOARGS,
     "Get the tree-sitter language for FSharp."},
    {"signature", _binding_language_fsharp_signature, METH_NOARGS,
     "Get the tree-sitter language for FSharp signature."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {.m_base = PyModuleDef_HEAD_INIT,
                                    .m_name = "_binding",
                                    .m_doc = NULL,
                                    .m_size = -1,
                                    .m_methods = methods};

PyMODINIT_FUNC PyInit__binding(void) { return PyModule_Create(&module); }
