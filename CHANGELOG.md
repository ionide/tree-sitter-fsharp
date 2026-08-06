# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Negative test suite under `test/corpus/invalid/`: snippets that are not valid
  F# and must produce an `ERROR` node, using the corpus `:error` attribute.

- `static member val` is now parsed by the auto-property rule rather than
  falling through to the generic member rule with `val` as the method name,
  which mis-nested the declaration that followed it.
- `new (args) as this = ...` on an additional constructor, which previously
  parsed only by accident, as an application of the identifiers `as` and `this`.

### Fixed
- Constructs that F#'s own parser rejects are no longer accepted:
  an accessibility modifier *after* `abstract` (`abstract public M : int`; the
  modifier is still accepted before `abstract`, which FSC parses and rejects
  later as FS0561), an accessibility modifier on a record field
  (`type R = { private i : int }`), an unparenthesised function type in a
  union case or exception field (`type B = A of int -> int`, which F# requires
  to be written `A of (int -> int)`), and an `as` binding on an object
  expression (`{ new Base() as b with ... }`).
- `npm run generate` now works on Windows. It was a `for dir in ...; do` shell
  loop, which npm hands to `cmd.exe` there, failing with "dir was unexpected at
  this time"; it is now two `tree-sitter generate --output` calls joined by `&&`.
- `as`, `namespace`, `open` and `type` are now reserved words, so they no longer
  fall back to identifiers in positions where the keyword is not valid. This
  makes `let type = 2`, `open` inside a function body, a `namespace` declaration
  after code, and `inherit Base(x) as base` report an error, as FSC does.

## [0.3.11] - 2026-07-30

### Added
- SRTP support constraints and trait calls over several typars:
  `((^a or ^b) : (static member op_Implicit: ^a -> ^b) x)` and
  `when (^t or ^u) : (static member M : string)`.
- F# 7 interfaces with static abstract members (`static abstract member M: 'T`)
  and self-constrained typars (`when IStaticProperty<'T>`).
- Units of measure: juxtaposed products (`kg m / s^2`), negative exponents
  (`s^-1`), rational exponents (`kg^(1/2)`, `kg^(-1/2)`) and quotient chains
  (`m / s / s`).
- `inherit` inside an `interface ... end` body.
- Empty and inherit-only object initializers in additional constructors:
  `new (x) = {}`, `new () = { inherit Base() }`.
- Attributes and `inline` on property accessors
  (`with [<A>] get () = v and [<B>] inline set x = ...`), attributes on return
  types (`let f x : [<A>] int = ...`), `abstract inline`, accessibility
  modifiers before `abstract`, `use (pat: T) = ...`, and a trailing separator
  in an attribute set (`[<A; >]`).
- `#light "off"` / `#light "on"` (the directive with an explicit argument).
- `\UXXXXXXXX` escapes in character literals.

### Fixed
- A trailing `;` or `;;` now terminates a top-level element instead of acting as
  a sequential-expression separator, so `exit 0;;` at end of file — or followed
  by a further declaration — parses.
- `_unicodegraph_long` matched `\u` with eight digits, which the four-digit
  short form already claimed, leaving the long form unreachable.

### Changed
- `measure_quotient` reports a division chain as one node with the operands in
  order, matching FSC's own flat `SynType.Tuple [Type; Slash; Type]` shape. A
  single `/` is unaffected.

## [0.3.3] - 2026-07-27

### Added
- Python bindings for both grammars, published to PyPI: `language()` for
  implementation files and `language_signature()` for `.fsi`, with the query
  files bundled (including the signature set) ([#225]). Closes [#176].
- Go bindings (`go get github.com/ionide/tree-sitter-fsharp`): `Language()`
  and `LanguageSignature()` from one package; tested in CI across the
  three-OS matrix ([#225]).
- Swift bindings via SwiftPM (`Package.swift` building both grammars)
  ([#225]).
- Regression tests for de-indented function expressions ([#133]).

### Fixed
- `#if` directives nested inside another structured directive's branch are
  consumed as stray trivia, keeping the outer `#endif` paired — halves the
  error count on directive-heavy real-world files ([#221]).
- Record updates (`{ q with A = ...; ... }`) accept continuation fields
  indented less than the first field ([#220]).

### Changed
- Dependency bumps ([#214], [#226]).

## [0.3.2] - 2026-07-25

### Added
- Shebang (`#!`) line support ([#213]).
- Dedicated query files for the `fsharp_signature` grammar: `.fsi` files now
  get syntax highlighting and tags — the shared query files reference
  expression-layer nodes the signature grammar does not have, so every query
  file except injections previously failed to load ([#218]).
- Parser edge case tests ([#215]).

### Fixed
- `#if/#else/#endif` handling: directives at positions the grammar has no
  preproc rule for are consumed as trivia, root-level `#endif`, branches
  starting with continuation tokens, and static resolution of the
  module/expression ambiguity ([#204], [#205], [#207], [#208]).
- Offside/indentation: mixed separators and de-indentation rules ([#216]),
  INDENT no longer anchors to a block comment trailing the line ([#206]),
  trailing back-pipe (`<|`) continuations, and property accessors as the last
  member before `and` ([#217]).
- Tokenization: `(*)` is the multiplication operator, not a comment opener —
  including the GLR fork that could derail whole files; match arms glued to
  patterns (`|"A"`, `|(p, q)`); attributes between `type` and the name;
  trailing `;;`; `#indent "off"` consumed as trivia ([#217]).
- Literals and operators: uppercase `F` float32 suffix (`1.0F`); `,` is no
  longer accepted as an operator character (`(,)`); removed a bogus
  backslash line-continuation from `extras` ([#218]).
- More syntax fixes: `use mutable`, generic `new`, dotted operators,
  multi-line `else if`, trailing semicolons ([#200]), classes ([#199]),
  tuple and measure expressions ([#198]), whitespace in attributes ([#197]),
  and other fixes ([#212]).
- Queries: bare/static members are now tagged; the wildcard-parameter
  highlight no longer matches every parameter under vim regex ([#218]).
- Packaging: the `tree-sitter` peer dependency is correctly marked optional
  ([#218]).

### Changed
- Scanner performance improvement: state restores no longer free and
  reallocate per step (~30% faster parsing) ([#209]).
- Dependency bumps ([#210]).

## [0.3.1] - 2026-07-01

### Added
- Highlight unambiguous `query { ... }` custom operations as keywords ([#191]).
- Support for type definitions with multiline generics, trailing semicolons and more ([#181]).
- Tags for bare type declarations ([#179]).
- Doc comments between `(**` and `*)` are now treated as markdown for injection ([#186]).

### Fixed
- Additional F# syntax feature fixes ([#188], [#189]).
- Corrected the test suite for newer tree-sitter releases: removed stale `variable`
  highlight assertions on unit `()` that no query captures.

### Changed
- Updated tree-sitter to support ABI-15 ([#177]).
- Fixed the `Fuzz scanner` CI job: it now checks the real per-language scanner
  paths (`fsharp/src/scanner.c`, `fsharp_signature/src/scanner.c`) and uses the
  correct `fsharp_signature` grammar directory.
- Dependency bumps ([#174], [#178], [#185], [#190]).

## [0.3.0] - 2026-04-16

Initial `0.3.x` release.

[0.3.3]: https://github.com/ionide/tree-sitter-fsharp/compare/0.3.2...0.3.3
[0.3.2]: https://github.com/ionide/tree-sitter-fsharp/compare/0.3.1...0.3.2
[0.3.1]: https://github.com/ionide/tree-sitter-fsharp/compare/0.3.0...0.3.1
[0.3.0]: https://github.com/ionide/tree-sitter-fsharp/releases/tag/0.3.0

[#133]: https://github.com/ionide/tree-sitter-fsharp/pull/133
[#176]: https://github.com/ionide/tree-sitter-fsharp/issues/176
[#214]: https://github.com/ionide/tree-sitter-fsharp/pull/214
[#220]: https://github.com/ionide/tree-sitter-fsharp/pull/220
[#221]: https://github.com/ionide/tree-sitter-fsharp/pull/221
[#225]: https://github.com/ionide/tree-sitter-fsharp/pull/225
[#226]: https://github.com/ionide/tree-sitter-fsharp/pull/226

[#197]: https://github.com/ionide/tree-sitter-fsharp/pull/197
[#198]: https://github.com/ionide/tree-sitter-fsharp/pull/198
[#199]: https://github.com/ionide/tree-sitter-fsharp/pull/199
[#200]: https://github.com/ionide/tree-sitter-fsharp/pull/200
[#204]: https://github.com/ionide/tree-sitter-fsharp/pull/204
[#205]: https://github.com/ionide/tree-sitter-fsharp/pull/205
[#206]: https://github.com/ionide/tree-sitter-fsharp/pull/206
[#207]: https://github.com/ionide/tree-sitter-fsharp/pull/207
[#208]: https://github.com/ionide/tree-sitter-fsharp/pull/208
[#209]: https://github.com/ionide/tree-sitter-fsharp/pull/209
[#210]: https://github.com/ionide/tree-sitter-fsharp/pull/210
[#212]: https://github.com/ionide/tree-sitter-fsharp/pull/212
[#213]: https://github.com/ionide/tree-sitter-fsharp/pull/213
[#215]: https://github.com/ionide/tree-sitter-fsharp/pull/215
[#216]: https://github.com/ionide/tree-sitter-fsharp/pull/216
[#217]: https://github.com/ionide/tree-sitter-fsharp/pull/217
[#218]: https://github.com/ionide/tree-sitter-fsharp/pull/218

[#174]: https://github.com/ionide/tree-sitter-fsharp/pull/174
[#177]: https://github.com/ionide/tree-sitter-fsharp/pull/177
[#178]: https://github.com/ionide/tree-sitter-fsharp/pull/178
[#179]: https://github.com/ionide/tree-sitter-fsharp/pull/179
[#181]: https://github.com/ionide/tree-sitter-fsharp/pull/181
[#185]: https://github.com/ionide/tree-sitter-fsharp/pull/185
[#186]: https://github.com/ionide/tree-sitter-fsharp/pull/186
[#188]: https://github.com/ionide/tree-sitter-fsharp/pull/188
[#189]: https://github.com/ionide/tree-sitter-fsharp/pull/189
[#190]: https://github.com/ionide/tree-sitter-fsharp/pull/190
[#191]: https://github.com/ionide/tree-sitter-fsharp/pull/191
