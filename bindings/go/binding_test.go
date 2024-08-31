package tree_sitter_fsharp_test

import (
	"context"
	"testing"

	tree_sitter "github.com/smacker/go-tree-sitter"
	tree_sitter_fsharp "github.com/ionide/tree-sitter-fsharp"
)

func TestFSharpGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_fsharp.FSharp())
	if language == nil {
		t.Errorf("Error loading FSharp grammar")
	}

	sourceCode := []byte("module M = ()")

	node, err := tree_sitter.ParseCtx(context.Background(), sourceCode, language)
	if err != nil || node.HasError() {
		t.Errorf("Error parsing FSharp")
	}
}

func TestFSharpSignatureGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_fsharp.FSharpSignature())
	if language == nil {
		t.Errorf("Error loading FSharpSignature grammar")
	}

	sourceCode := []byte("val x : int -> int")

	node, err := tree_sitter.ParseCtx(context.Background(), sourceCode, language)
	if err != nil || node.HasError() {
		t.Errorf("Error parsing FSharpSignature")
	}
}
