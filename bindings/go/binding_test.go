package tree_sitter_fsharp_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	tree_sitter_fsharp "github.com/tree-sitter/tree-sitter-fsharp/bindings/go"
)

func TestFSharpGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_fsharp.LanguageFSharp())
	if language == nil {
		t.Errorf("Error loading FSharp grammar")
	}

	sourceCode := []byte("module M = ()")
	parser := tree_sitter.NewParser()
	defer parser.Close()
	parser.SetLanguage(language)

	tree := parser.Parse(sourceCode, nil)
	if tree == nil || tree.RootNode().HasError() {
		t.Errorf("Error parsing FSharp")
	}
}

func TestFSharpSignatureGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_fsharp.LanguageFSharpSignature())
	if language == nil {
		t.Errorf("Error loading FSharpSignature grammar")
	}

	sourceCode := []byte("val x : int -> int")
	parser := tree_sitter.NewParser()
	defer parser.Close()
	parser.SetLanguage(language)

	tree := parser.Parse(sourceCode, nil)
	if tree == nil || tree.RootNode().HasError() {
		t.Errorf("Error parsing FSharpSignature")
	}
}
