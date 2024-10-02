# tree-sitter-fsharp

tree-sitter grammar for F# (still WIP)
Based on [the 4.1 F# language specification](https://fsharp.org/specs/language-spec/4.1/FSharpSpec-4.1-latest.pdf) (Mostly, Appendix A)
and the [F# compiler parser](https://github.com/dotnet/fsharp/blob/main/src/Compiler/pars.fsy)

## Getting started

First, run `npm install` to install the `tree-sitter cli`.
Next, the grammar can be build using `npm run build`, or used to parse a file with `npm run parse $file`

### Project structure

This project defined two parser:

- [./fsharp/grammar.js](./fsharp/grammar.js)`fsharp` grammar.
- [./fsharp_signature/grammar.js](./fsharp_signature/grammar.js) defines the grammar for signature files

In addition to the `grammar.js` files each parser depends on a common external scanner found at [./common/scanner.h](./common/scanner.h).
The external scanner is responsible for parsing newlines and comments and keeps track of indentation to open and close scopes.

each grammar starts with the `file` node at the beginning of the rules.

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

- `npm run generate` rebuilds all parser.
- `npx tree-sitter test` runs all tests for both parsers.
- `npx tree-sitter parse $file` run the `fsharp` parser on `$file` and outputs the parse tree.
- `npx tree-sitter parse -d $file` run the `fsharp` parser on `$file` and prints debug information.

## How to contribute

Clone the project and start playing around with it.
If you find a code example which fails to parse, please reduce it to a minimal example and added to the corpus (`fsharp/test/corpus/*.txt`) as a test case.

For an introduction to developing tree-sitter parsers the [official documentation](https://tree-sitter.github.io/tree-sitter/creating-parsers) is a good reference point.

PRs fleshing out the grammar or fixing bugs are welcome!
