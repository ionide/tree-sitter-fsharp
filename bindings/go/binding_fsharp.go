package tree_sitter_fsharp

// #cgo CFLAGS: -I../../fsharp/src -std=c11 -fPIC
// #include "../../fsharp/src/parser.c"
// #include "../../fsharp/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for FSharp.
func LanguageFSharp() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp())
}
