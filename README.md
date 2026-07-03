# tree-sitter-fsharp

tree-sitter grammar for F# (still WIP)
Based on [the 4.1 F# language specification](https://fsharp.org/specs/language-spec/4.1/FSharpSpec-4.1-latest.pdf) (Mostly, Appendix A)
and the [F# compiler parser](https://github.com/dotnet/fsharp/blob/main/src/Compiler/pars.fsy)

## Getting started

First, run `npm install` to install the `tree-sitter` CLI as a local devDependency (it lands at `node_modules/.bin/tree-sitter`, not on your global `$PATH`).

You can then run the CLI in any of these ways:

- `npx tree-sitter <command>` — resolves the local binary automatically.
- `source ./activate` (from the repo root) — prepends `node_modules/.bin` to `$PATH` for the current shell, similar to Python's `venv/bin/activate`. After sourcing, plain `tree-sitter <command>` works. Run `deactivate` to undo.
- `npm run generate` — regenerates both parsers (`fsharp` and `fsharp_signature`).

To parse a file: `npx tree-sitter parse <file>` (see the [Development](#development) section for more).

### Project structure

This project defines two parsers:

- [./fsharp/grammar.js](./fsharp/grammar.js) — grammar for `.fs`/`.fsx` (source files).
- [./fsharp_signature/grammar.js](./fsharp_signature/grammar.js) — grammar for `.fsi` (signature files). This is **not** a standalone grammar: it starts with `grammar(require("../fsharp/grammar"), { ... })` and overrides/adds a few rules on top of the base. Any change to `fsharp/grammar.js` also changes the derived signature grammar, so both parsers must be regenerated together (`npm run generate`).

Both parsers share the external scanner at [./common/scanner.h](./common/scanner.h). The per-parser `src/scanner.c` files are thin shims that `#include` the header — real scanner logic (newlines, comments, indent/dedent bookkeeping) lives in the header. Each grammar starts with the `file` node.

The generated `src/grammar.json` and `src/parser.c` files are checked into git and validated by CI. Do not hand-edit them; merge conflicts are resolved by re-running `npm run generate`. See [AGENTS.md](./AGENTS.md) for the full development workflow.

### Adding to neovim

tree-sitter-fsharp is supported through the usual installation methods of [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter).

Installing the lastest grammar from this repo involves the following three steps:

1. Update your Neovim config for nvim-treesitter to refer to tree-sitter-fsharp.
2. Run `:TSInstall fsharp` inside Neovim.
3. Copy the files from [./queries/](./queries) to the neovim config directory at`$XDG_CONFIG_HOME/nvim/queries/fsharp/` - see [the Adding queries section of the nvim-treesitter README](https://github.com/nvim-treesitter/nvim-treesitter?tab=readme-ov-file#adding-queries).

The config you need is this (you can use a local path for `url` if you prefer):

```lua
local parser_config = require('nvim-treesitter.parsers').get_parser_configs()
parser_config.fsharp = {
  install_info = {
    url = 'https://github.com/ionide/tree-sitter-fsharp',
    branch = 'main',
    files = { 'src/scanner.c', 'src/parser.c' },
    location = "fsharp"
  },
  requires_generate_from_grammar = false,
  filetype = 'fsharp',
}
```

## Development

The `package.json` defines some helpful targets for developing the grammar:

- Run commands from the repository root so tree-sitter picks up both grammars from `tree-sitter.json`.
- `npm run generate` rebuilds both parsers.
- `npx tree-sitter test` runs all tests for both parsers.
- `npx tree-sitter parse $file` runs the `fsharp` parser on `$file` and outputs the parse tree.
- `npx tree-sitter parse -d $file` runs the `fsharp` parser on `$file` and prints debug information.
- For `.fsi` files, use `npx tree-sitter parse -p fsharp_signature $file` because `tree-sitter parse` does not reliably select the signature parser automatically.

## How to contribute

Clone the project and start playing around with it.
If you find a code example which fails to parse, please reduce it to a minimal example and added to the corpus (`fsharp/test/corpus/*.txt`) as a test case.

For an introduction to developing tree-sitter parsers the [official documentation](https://tree-sitter.github.io/tree-sitter/creating-parsers) is a good reference point.

PRs fleshing out the grammar or fixing bugs are welcome!
