package tree_sitter_fsharp

// #cgo CFLAGS: -I../../grammars/fsharp/src -std=c11 -fPIC
// #include "../../grammars/signature/src/parser.c"
// #include "../../grammars/signature/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for FSharp signature.
func FSharpSignature() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp_signature())
}
