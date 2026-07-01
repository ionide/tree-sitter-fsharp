# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

[0.3.1]: https://github.com/ionide/tree-sitter-fsharp/compare/0.3.0...0.3.1
[0.3.0]: https://github.com/ionide/tree-sitter-fsharp/releases/tag/0.3.0

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
