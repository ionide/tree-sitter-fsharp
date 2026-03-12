package fsharp_signature_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	fsharp_signature "github.com/tree-sitter/tree-sitter-fsharp/bindings/go/fsharp_signature"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(fsharp_signature.Language())
	if language == nil {
		t.Errorf("Error loading Fsharp signature grammar")
	}
}
