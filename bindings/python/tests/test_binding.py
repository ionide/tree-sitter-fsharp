from unittest import TestCase

import tree_sitter_fsharp
from tree_sitter import Language, Parser


class TestLanguage(TestCase):
    def test_fsharp_grammar(self):
        language = Language(tree_sitter_fsharp.fsharp())
        parser = Parser(language)
        tree = parser.parse(
            b"""
            module M =
              let x = 0
            """
        )
        self.assertFalse(tree.root_node.has_error)

    def test_signature_grammar(self):
        language = Language(tree_sitter_fsharp.signature())
        parser = Parser(language)
        tree = parser.parse(
            b"""
            module M =
              val x : int -> int
            """
        )
        self.assertFalse(tree.root_node.has_error)
