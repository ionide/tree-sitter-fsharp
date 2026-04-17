package fsharp_signature

// #cgo CFLAGS: -std=c11 -fPIC -I../../../fsharp_signature/src
// #include "../../../fsharp_signature/src/parser.c"
// #include "../../../fsharp_signature/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for this grammar.
func Language() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp_signature())
}
