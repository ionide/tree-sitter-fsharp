# tree-sitter-fsharp

F# grammars for [tree-sitter][], built as a C library.

The library exports **two** parsers, both declared in
[`tree_sitter/tree-sitter-fsharp.h`](tree_sitter/tree-sitter-fsharp.h):

| Function                         | Grammar              | File types    |
| -------------------------------- | -------------------- | ------------- |
| `tree_sitter_fsharp()`           | implementation files | `.fs`, `.fsx` |
| `tree_sitter_fsharp_signature()` | signature files      | `.fsi`        |

Both grammars are compiled into one `libtree-sitter-fsharp`.

## Building

There is no C package on any registry — build from a checkout of the repository. Run the
commands from the **repository root**, not from this directory.

### Make

```bash
make
sudo make install
```

Honours the usual `PREFIX`, `DESTDIR`, `LIBDIR`, `INCLUDEDIR` and `CC`/`CFLAGS`
variables. `make install` installs the static and shared libraries, the public header,
a `tree-sitter-fsharp.pc` pkg-config file, and the query files under
`$(DATADIR)/tree-sitter/queries/{fsharp,fsharp_signature}`. `make uninstall` reverses it.

The Makefile is POSIX-only and aborts on Windows; use CMake there.

### CMake

```bash
cmake -B build
cmake --build build
cmake --install build
```

`BUILD_SHARED_LIBS` (default `ON`) selects a shared or static library, and
`TREE_SITTER_ABI_VERSION` (default `15`) sets the ABI passed to `tree-sitter generate`
when the checked-in `parser.c` files need regenerating.

Both build systems regenerate `parser.c` from `grammar.js` if it is out of date, which
needs the [tree-sitter CLI][cli] on `PATH`.

## Usage

The grammars provide only the language functions — you also need the
[tree-sitter runtime library][runtime] for the parser API itself.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-fsharp.h>

int main(void) {
  const char *source = "let x = 1\n";

  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_fsharp());

  TSTree *tree = ts_parser_parse_string(parser, NULL, source, strlen(source));
  char *sexp = ts_node_string(ts_tree_root_node(tree));
  printf("%s\n", sexp);

  free(sexp);
  ts_tree_delete(tree);
  ts_parser_delete(parser);
  return 0;
}
```

Signature files need the other grammar — it is a separate parser, not a mode of the first,
so swap `tree_sitter_fsharp()` for `tree_sitter_fsharp_signature()`.

Compile against the installed pkg-config files:

```bash
cc example.c $(pkg-config --cflags --libs tree-sitter tree-sitter-fsharp) -o example
```

`ts_parser_set_language` returns `false` if the runtime's ABI does not match the one the
parser was generated with; check it in real code.

## Contributing and grammar development

Grammar sources, the test corpus and the development workflow live in the repository:
see the [main README][repo] and [AGENTS.md][agents]. Bug reports for mis-parsed F# code
are welcome — reduce the snippet to a minimal example and open an issue or PR.

## License

MIT — see [LICENSE][license].

[tree-sitter]: https://tree-sitter.github.io/tree-sitter/
[cli]: https://github.com/tree-sitter/tree-sitter/tree/master/crates/cli
[runtime]: https://github.com/tree-sitter/tree-sitter/tree/master/lib
[repo]: https://github.com/ionide/tree-sitter-fsharp#readme
[agents]: https://github.com/ionide/tree-sitter-fsharp/blob/main/AGENTS.md
[license]: https://github.com/ionide/tree-sitter-fsharp/blob/main/LICENSE
