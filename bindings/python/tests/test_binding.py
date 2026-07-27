from unittest import TestCase

from tree_sitter import Language, Parser
import tree_sitter_fsharp


class TestLanguage(TestCase):
    def test_can_load_grammar(self):
        try:
            Parser(Language(tree_sitter_fsharp.language()))
        except Exception:
            self.fail("Error loading Fsharp grammar")

    def test_can_load_signature_grammar(self):
        try:
            Parser(Language(tree_sitter_fsharp.language_signature()))
        except Exception:
            self.fail("Error loading Fsharp signature grammar")

    def test_can_parse(self):
        parser = Parser(Language(tree_sitter_fsharp.language()))
        tree = parser.parse(b"let x = 1\n")
        self.assertFalse(tree.root_node.has_error)

    def test_can_parse_signature(self):
        parser = Parser(Language(tree_sitter_fsharp.language_signature()))
        tree = parser.parse(b"module Test\nval x : int\n")
        self.assertFalse(tree.root_node.has_error)
