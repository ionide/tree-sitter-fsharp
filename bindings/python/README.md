# tree-sitter-fsharp

F# grammars for [tree-sitter][], packaged for Python.

The package ships **two** parsers:

| Function               | Grammar              | File types    |
| ---------------------- | -------------------- | ------------- |
| `language()`           | implementation files | `.fs`, `.fsx` |
| `language_signature()` | signature files      | `.fsi`        |

## Installation

```bash
pip install tree-sitter-fsharp tree-sitter
```

Requires Python 3.10 or later. The `tree-sitter` runtime (`~=0.24`) is an optional
dependency, also installable as an extra:

```bash
pip install "tree-sitter-fsharp[core]"
```

Wheels are published for the common platforms; other platforms build the extension from
the bundled C sources, which needs a C11 compiler.

## Usage

```python
from tree_sitter import Language, Parser
import tree_sitter_fsharp

parser = Parser(Language(tree_sitter_fsharp.language()))

tree = parser.parse(b"let x = 1\n")
print(tree.root_node)
```

Signature files need the other grammar — it is a separate parser, not a mode of the first:

```python
parser = Parser(Language(tree_sitter_fsharp.language_signature()))
tree = parser.parse(b"module M\nval x : int\n")
```

## Queries

Highlight, injection, locals and tags queries ship inside the package and are exposed as
lazily-read module attributes holding the query source (each is `None` when that query
file is absent):

```python
import tree_sitter_fsharp

source = tree_sitter_fsharp.HIGHLIGHTS_QUERY
```

Available: `HIGHLIGHTS_QUERY`, `INJECTIONS_QUERY`, `LOCALS_QUERY` and `TAGS_QUERY` for the
implementation grammar, plus `SIGNATURE_HIGHLIGHTS_QUERY` and `SIGNATURE_TAGS_QUERY` for
the signature grammar.

Type stubs (`py.typed`, `__init__.pyi`) are included.

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
