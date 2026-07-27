# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

[0.3.2]: https://github.com/ionide/tree-sitter-fsharp/compare/0.3.1...0.3.2
[0.3.1]: https://github.com/ionide/tree-sitter-fsharp/compare/0.3.0...0.3.1
[0.3.0]: https://github.com/ionide/tree-sitter-fsharp/releases/tag/0.3.0

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
