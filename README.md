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
#### From the local copy:
```lua
local parser_config = require "nvim-treesitter.parsers".get_parser_configs()
parser_config.fsharp = {
  install_info = {
    url = "path/to/tree-sitter-fsharp",
    files = {"src/scanner.c", "src/parser.c" }
  },
  filetype = "fsharp",
}
```
#### From GitHub repository:
```lua
local parser_config = require "nvim-treesitter.parsers".get_parser_configs()
parser_config.fsharp = {
  install_info = {
    url = "https://github.com/ionide/tree-sitter-fsharp",
    branch = "main",
    files = {"src/scanner.c", "src/parser.c" },
  },
  filetype = "fsharp",
}
```

Then run `:TSInstall fsharp` inside neovim.

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
