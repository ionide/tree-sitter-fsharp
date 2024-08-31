package tree_sitter_fsharp

// #cgo CFLAGS: -I../../grammars/fsharp/src -std=c11 -fPIC
// #include "../../grammars/fsharp/src/parser.c"
// #include "../../grammars/fsharp/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for FSharp.
func FSharp() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp())
}
