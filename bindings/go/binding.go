package tree_sitter_fsharp

// #cgo CFLAGS: -std=c11 -fPIC -I../../fsharp/src
// #include "../../fsharp/src/parser.c"
// #include "../../fsharp/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for this grammar.
func Language() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp())
}
