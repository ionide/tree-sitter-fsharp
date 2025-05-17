#include <Python.h>

typedef struct TSLanguage TSLanguage;

TSLanguage *tree_sitter_fsharp(void);
TSLanguage *tree_sitter_fsharp_signature(void);

static PyObject *_binding_language_fsharp(PyObject *Py_UNUSED(self),
                                          PyObject *Py_UNUSED(args)) {
  return PyCapsule_New(tree_sitter_fsharp(), "tree_sitter.Language", NULL);
}

static PyObject *_binding_language_fsharp_signature(PyObject *Py_UNUSED(self),
                                                    PyObject *Py_UNUSED(args)) {
  return PyCapsule_New(tree_sitter_fsharp_signature(), "tree_sitter.Language",
                       NULL);
}

static struct PyModuleDef_Slot slots[] = {
#ifdef Py_GIL_DISABLED
    {Py_mod_gil, Py_MOD_GIL_NOT_USED},
#endif
    {0, NULL}};

static PyMethodDef methods[] = {
    {"language_fsharp", _binding_language_fsharp, METH_NOARGS,
     "Get the tree-sitter language for FSharp."},
    {"language_fsharp_signature", _binding_language_fsharp_signature,
     METH_NOARGS, "Get the tree-sitter language for FSharp interfaces."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_binding",
    .m_doc = NULL,
    .m_size = 0,
    .m_methods = methods,
    .m_slots = slots,
};

PyMODINIT_FUNC PyInit__binding(void) { return PyModuleDef_Init(&module); }
