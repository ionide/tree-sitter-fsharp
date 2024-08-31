
//! This crate provides FSharp language support for the [tree-sitter][] parsing
//! library. There are separate languages for implementation, (`.fs`),
//! signature (`.fsi`).
//!
//! Typically, you will use the [language_fsharp][language func] function to add
//! this language to a tree-sitter [Parser][], and then use the parser to parse
//! some code:
//!
//! ```
//! let code = r#"
//!   module M =
//!     let x = 0
//! "#;
//! let mut parser = tree_sitter::Parser::new();
//! parser
//!     .set_language(&tree_sitter_fsharp::language_fsharp())
//!     .expect("Error loading FSharp grammar");
//! let tree = parser.parse(code, None).unwrap();
//! assert!(!tree.root_node().has_error());
//! ```
//!
//! [Language]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Language.html
//! [language func]: fn.language_fsharp.html
//! [Parser]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Parser.html
//! [tree-sitter]: https://tree-sitter.github.io/

use tree_sitter::Language;

extern "C" {
    fn tree_sitter_fsharp() -> Language;
    fn tree_sitter_fsharp_signature() -> Language;
}

/// Get the tree-sitter [Language][] for FSharp.
///
/// [Language]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Language.html
pub fn language_fsharp() -> Language {
    unsafe { tree_sitter_fsharp() }
}

/// Get the tree-sitter [Language][] for FSharp signature.
///
/// [Language]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Language.html
pub fn language_fsharp_signature() -> Language {
    unsafe { tree_sitter_fsharp_signature() }
}

/// The content of the [`node-types.json`][] file for FSharp.
///
/// [`node-types.json`]: https://tree-sitter.github.io/tree-sitter/using-parsers#static-node-types
pub const FSHARP_NODE_TYPES: &'static str = include_str!("../../fsharp/src/node-types.json");

/// The content of the [`node-types.json`][] file for FSharp signature.
///
/// [`node-types.json`]: https://tree-sitter.github.io/tree-sitter/using-parsers#static-node-types
pub const SIGNATURE_NODE_TYPES: &'static str = include_str!("../../fsharp_signature/src/node-types.json");

/// The syntax highlighting query for FSharp.
pub const HIGHLIGHTS_QUERY: &'static str = include_str!("../../queries/highlights.scm");

/// The local-variable syntax highlighting query for FSharp.
pub const LOCALS_QUERY: &'static str = include_str!("../../queries/locals.scm");

/// The symbol tagging query for FSharp.
pub const TAGGING_QUERY: &'static str = include_str!("../../queries/tags.scm");

#[cfg(test)]
mod tests {
    #[test]
    fn test_fsharp() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::language_fsharp())
            .expect("Error loading FSharp grammar");

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
            .set_language(&super::language_fsharp_signature())
            .expect("Error loading FSharp signature grammar");

        let code = r#"
            module M =
              val x : int -> int
        "#;

        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();
        assert!(!root.has_error());
    }
}

