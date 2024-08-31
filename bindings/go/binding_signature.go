package tree_sitter_fsharp

// #cgo CFLAGS: -I../../fsharp_signature/src -std=c11 -fPIC
// #include "../../fsharp_signature/src/parser.c"
// #include "../../fsharp_signature/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for FSharp signature.
func FSharpSignature() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp_signature())
}
