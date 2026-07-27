package tree_sitter_fsharp

// NOTE: the implementation and signature grammars are compiled in separate
// files (this one and binding_signature.go) because each cgo preamble is its
// own C translation unit — both scanner.c files define the same static
// helper functions and cannot share one.

// #cgo CFLAGS: -std=c11 -fPIC -I${SRCDIR}/../../fsharp/src
// #include "../../fsharp/src/parser.c"
// #include "../../fsharp/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for F# implementation files (.fs, .fsx).
func Language() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp())
}
