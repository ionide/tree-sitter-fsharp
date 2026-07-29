# tree-sitter-fsharp

F# grammars for [tree-sitter][], packaged for Go.

The package ships **two** parsers:

| Function              | Grammar              | File types    |
| --------------------- | -------------------- | ------------- |
| `Language()`          | implementation files | `.fs`, `.fsx` |
| `LanguageSignature()` | signature files      | `.fsi`        |

## Installation

```bash
go get github.com/ionide/tree-sitter-fsharp/bindings/go
go get github.com/tree-sitter/go-tree-sitter
```

The parsers are compiled from the bundled C sources, so **cgo must be enabled** and a C
compiler must be on `PATH`. Builds with `CGO_ENABLED=0` will fail.

> **Version note.** Releases from 0.3.0 onwards are tagged without the `v` prefix that Go
> modules require, so the module proxy still resolves `@latest` to `v0.2.0`. Until
> `v`-prefixed tags are published, pin to a revision to get a newer grammar:
>
> ```bash
> go get github.com/ionide/tree-sitter-fsharp/bindings/go@main
> ```
>
> which records a pseudo-version such as `v0.2.1-0.20250727...`.

## Usage

```go
package main

import (
	"fmt"
	"log"

	tree_sitter_fsharp "github.com/ionide/tree-sitter-fsharp/bindings/go"
	tree_sitter "github.com/tree-sitter/go-tree-sitter"
)

func main() {
	parser := tree_sitter.NewParser()
	defer parser.Close()

	if err := parser.SetLanguage(tree_sitter.NewLanguage(tree_sitter_fsharp.Language())); err != nil {
		log.Fatalf("error loading F# grammar: %v", err)
	}

	tree := parser.Parse([]byte("let x = 1\n"), nil)
	defer tree.Close()

	fmt.Println(tree.RootNode().ToSexp())
}
```

Signature files need the other grammar — it is a separate parser, not a mode of the first:

```go
if err := parser.SetLanguage(tree_sitter.NewLanguage(tree_sitter_fsharp.LanguageSignature())); err != nil {
	log.Fatalf("error loading F# signature grammar: %v", err)
}

tree := parser.Parse([]byte("module M\nval x : int\n"), nil)
```

Both functions return an `unsafe.Pointer` to a `TSLanguage`, which is what
`tree_sitter.NewLanguage` expects.

## Queries

The Go binding does not embed the query files, but they are part of the module, so `go:embed`
or a plain read works against the module directory:

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
[repo]: https://github.com/ionide/tree-sitter-fsharp#readme
[agents]: https://github.com/ionide/tree-sitter-fsharp/blob/main/AGENTS.md
[license]: https://github.com/ionide/tree-sitter-fsharp/blob/main/LICENSE
