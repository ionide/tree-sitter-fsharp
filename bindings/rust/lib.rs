//! This crate provides Fsharp language support for the [tree-sitter][] parsing library.
//!
//! Typically, you will use the [language][language func] function to add this language to a
//! tree-sitter [Parser][], and then use the parser to parse some code:
//!
//! ```
//! let code = r#"
//! "#;
//! let mut parser = tree_sitter::Parser::new();
//! let language = tree_sitter_fsharp::LANGUAGE_FSHARP;
//! parser
//!     .set_language(&language.into())
//!     .expect("Error loading Fsharp parser");
//! let tree = parser.parse(code, None).unwrap();
//! assert!(!tree.root_node().has_error());
//! ```
//!
//! [Language]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Language.html
//! [language func]: fn.language.html
//! [Parser]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Parser.html
//! [tree-sitter]: https://tree-sitter.github.io/

use tree_sitter_language::LanguageFn;

extern "C" {
    fn tree_sitter_fsharp() -> *const ();
    fn tree_sitter_fsharp_signature() -> *const ();
}

/// The tree-sitter [`LanguageFn`] for this grammar.
pub const LANGUAGE_FSHARP: LanguageFn = unsafe { LanguageFn::from_raw(tree_sitter_fsharp) };
pub const LANGUAGE_SIGNATURE: LanguageFn = unsafe { LanguageFn::from_raw(tree_sitter_fsharp_signature) };

/// The content of the [`node-types.json`][] file for this grammar.
///
/// [`node-types.json`]: https://tree-sitter.github.io/tree-sitter/using-parsers#static-node-types
pub const FSHARP_NODE_TYPES: &str = include_str!("../../fsharp/src/node-types.json");
pub const SIGNATURE_NODE_TYPES: &str = include_str!("../../fsharp_signature/src/node-types.json");

// NOTE: uncomment these to include any queries that this grammar contains:

pub const HIGHLIGHTS_QUERY: &str = include_str!("../../queries/highlights.scm");
pub const INJECTIONS_QUERY: &str = include_str!("../../queries/injections.scm");
pub const LOCALS_QUERY: &str = include_str!("../../queries/locals.scm");
pub const TAGS_QUERY: &str = include_str!("../../queries/tags.scm");

#[cfg(test)]
mod tests {
    #[test]
    fn test_fsharp() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE_FSHARP.into())
            .expect("Error loading Fsharp parser");

        let code = r#"
            module M =
              let x = 0
        "#;

        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();
        assert!(!root.has_error());
    }

    #[test]
    fn test_fsharp_signature() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE_SIGNATURE.into())
            .expect("Error loading Fsharp parser");

        let code = r#"
            module M =
              val x : int -> int
        "#;

        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();
        assert!(!root.has_error());
    }
}
