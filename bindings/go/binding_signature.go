package tree_sitter_fsharp

// #cgo CFLAGS: -std=c11 -fPIC -I${SRCDIR}/../../fsharp_signature/src
// #include "../../fsharp_signature/src/parser.c"
// #include "../../fsharp_signature/src/scanner.c"
import "C"

import "unsafe"

// Get the tree-sitter Language for F# signature files (.fsi).
func LanguageSignature() unsafe.Pointer {
	return unsafe.Pointer(C.tree_sitter_fsharp_signature())
}
