# tree-sitter-fsharp
tree-sitter grammar for F# (still WIP)
Based on [the 4.1 F# language specification](https://fsharp.org/specs/language-spec/4.1/FSharpSpec-4.1-latest.pdf) (Mostly, Appendix A)
and the [F# compiler parser](https://github.com/dotnet/fsharp/blob/main/src/Compiler/pars.fsy)

## Getting started

First, run `npm install` to install the `tree-sitter cli`.
Next, the grammar can be build using `npm run build`, or used to parse a file with `npm run parse $file`

### Project structure
The parser consists of two parts:
- `src/scanner.c` is responsible for parsing newlines and comments and keeps track of indentation to open and close scopes.
- `grammar.js` the main tree-sitter grammar. The indent tokens from the external scanner is access though the `indent` and `dedent` tokens.

The grammar starts with the `file` node at the beginning of the rules.

### Adding to neovim

tree-sitter-fsharp isn't supported out-of-the box by [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter).
However, nvim-treesitter has [instructions for how to manually add unsupported parsers](https://github.com/nvim-treesitter/nvim-treesitter?tab=readme-ov-file#adding-parsers).

For tree-sitter-fsharp, this involves three steps:

1. Update your Neovim config for nvim-treesitter to refer to tree-sitter-fsharp.
2. Run `:TSInstall fsharp` inside Neovim.
3. Copy the files from [./queries/](./queries) to the ./queries/fsharp/ directory of your Neovim runtime directory (where your config is stored) - see [the Adding queries section of the nvim-treesitter README](https://github.com/nvim-treesitter/nvim-treesitter?tab=readme-ov-file#adding-queries).

The config you need is this (you can use a local path for `url` if you prefer):

```lua
local parser_config = require('nvim-treesitter.parsers').get_parser_configs()
parser_config.fsharp = {
  install_info = {
    url = 'https://github.com/ionide/tree-sitter-fsharp',
    branch = 'main',
    files = { 'src/scanner.c', 'src/parser.c' },
  },
  requires_generate_from_grammar = false,
  filetype = 'fsharp',
}
```

## Status
The grammar currently has support for most language features, but might have rough edges.
Offside tokens aren't supported.

## Testing
### Testing corpus
To run all tests stores in `corpus/` run

```sh
$ npm test
```

### Test parsing a specific file
```
$ npm run debug $file
```

## How to contribute
Clone the project and start playing around with it.
If you find a code example which fails to parse, please reduce it to a minimal example and added to the corpus (`test/corpus/*.txt`) as a test case.

For an introduction to developing tree-sitter parsers the [official documentation](https://tree-sitter.github.io/tree-sitter/creating-parsers) is a good reference point.

PRs fleshing out the grammar or fixing bugs are welcome!
