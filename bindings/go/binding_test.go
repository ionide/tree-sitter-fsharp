package tree_sitter_fsharp_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	tree_sitter_fsharp "github.com/ionide/tree-sitter-fsharp/bindings/go"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_fsharp.Language())
	if language == nil {
		t.Errorf("Error loading Fsharp grammar")
	}
}

func TestCanLoadSignatureGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_fsharp.LanguageSignature())
	if language == nil {
		t.Errorf("Error loading Fsharp signature grammar")
	}
}

func TestCanParse(t *testing.T) {
	parser := tree_sitter.NewParser()
	defer parser.Close()
	if err := parser.SetLanguage(tree_sitter.NewLanguage(tree_sitter_fsharp.Language())); err != nil {
		t.Fatalf("Error setting F# language: %v", err)
	}
	tree := parser.Parse([]byte("let x = 1\n"), nil)
	defer tree.Close()
	if tree.RootNode().HasError() {
		t.Errorf("Error parsing F# source")
	}
}

func TestCanParseSignature(t *testing.T) {
	parser := tree_sitter.NewParser()
	defer parser.Close()
	if err := parser.SetLanguage(tree_sitter.NewLanguage(tree_sitter_fsharp.LanguageSignature())); err != nil {
		t.Fatalf("Error setting F# signature language: %v", err)
	}
	tree := parser.Parse([]byte("module Test\nval x : int\n"), nil)
	defer tree.Close()
	if tree.RootNode().HasError() {
		t.Errorf("Error parsing F# signature source")
	}
}
