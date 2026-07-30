# tree-sitter-fsharp

F# grammars for [tree-sitter][], packaged as a Swift package.

The `TreeSitterFsharp` target exposes **two** parsers:

| Function                         | Grammar              | File types    |
| -------------------------------- | -------------------- | ------------- |
| `tree_sitter_fsharp()`           | implementation files | `.fs`, `.fsx` |
| `tree_sitter_fsharp_signature()` | signature files      | `.fsi`        |

Both are declared in [`TreeSitterFsharp/fsharp.h`](TreeSitterFsharp/fsharp.h) and compiled
into a single C target.

## Installation

Add the package to your `Package.swift`:

```swift
dependencies: [
    .package(url: "https://github.com/ionide/tree-sitter-fsharp", from: "0.3.11"),
    .package(url: "https://github.com/tree-sitter/swift-tree-sitter", from: "0.9.0"),
],
targets: [
    .target(
        name: "MyTarget",
        dependencies: [
            .product(name: "TreeSitterFsharp", package: "tree-sitter-fsharp"),
            .product(name: "SwiftTreeSitter", package: "swift-tree-sitter"),
        ]
    )
]
```

`TreeSitterFsharp` is a plain C target and does not depend on a Swift runtime itself —
pair it with [SwiftTreeSitter][swift-tree-sitter] for the Swift API, as the package's own
tests do.

## Usage

```swift
import SwiftTreeSitter
import TreeSitterFsharp

let parser = Parser()
try parser.setLanguage(Language(language: tree_sitter_fsharp()))

let tree = parser.parse("let x = 1\n")
```

Signature files need the other grammar — it is a separate parser, not a mode of the first:

```swift
try parser.setLanguage(Language(language: tree_sitter_fsharp_signature()))
let tree = parser.parse("module M\nval x : int\n")
```

## Queries

`queries/` is declared as a copied resource of the `TreeSitterFsharp` target, so the
`.scm` files land in the package's resource bundle with their directory structure intact.
Because the target is C rather than Swift, SwiftPM does not synthesise a `Bundle.module`
accessor for it; locate the bundle yourself, or vendor the query files you need directly
from the repository:

- `queries/` — `highlights.scm`, `indents.scm`, `injections.scm`, `locals.scm`, `tags.scm`
  for the implementation grammar
- `fsharp_signature/queries/` — `highlights.scm` and `tags.scm` for the signature grammar

## Contributing and grammar development

Grammar sources, the test corpus and the development workflow live in the repository:
see the [main README][repo] and [AGENTS.md][agents]. Bug reports for mis-parsed F# code
are welcome — reduce the snippet to a minimal example and open an issue or PR.

## License

MIT — see [LICENSE][license].

[tree-sitter]: https://tree-sitter.github.io/tree-sitter/
[swift-tree-sitter]: https://github.com/tree-sitter/swift-tree-sitter
[repo]: https://github.com/ionide/tree-sitter-fsharp#readme
[agents]: https://github.com/ionide/tree-sitter-fsharp/blob/main/AGENTS.md
[license]: https://github.com/ionide/tree-sitter-fsharp/blob/main/LICENSE
