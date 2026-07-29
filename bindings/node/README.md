# tree-sitter-fsharp

F# grammars for [tree-sitter][], packaged for Node.js.

The package ships **two** parsers:

| Export      | Grammar              | File types    |
| ----------- | -------------------- | ------------- |
| `fsharp`    | implementation files | `.fs`, `.fsx` |
| `signature` | signature files      | `.fsi`        |

## Installation

```bash
npm install tree-sitter-fsharp tree-sitter
```

`tree-sitter` (the Node runtime, `^0.25.0`) is an optional peer dependency: install it
alongside this package to parse with the native bindings.

Prebuilt binaries are published for the common platforms and picked up automatically by
`node-gyp-build`. On any other platform the parsers are compiled from the bundled C
sources at install time, which requires a C compiler and the usual `node-gyp` toolchain.

## Usage

```js
const Parser = require("tree-sitter");
const { fsharp, signature } = require("tree-sitter-fsharp");

const parser = new Parser();
parser.setLanguage(fsharp);

const tree = parser.parse("let x = 1\n");
console.log(tree.rootNode.toString());
```

Signature files need the other grammar — it is a separate parser, not a mode of the first:

```js
parser.setLanguage(signature);
const tree = parser.parse("module M\nval x : int\n");
```

Both exports carry a `nodeTypeInfo` property holding the parsed `node-types.json` for that
grammar, and TypeScript declarations are included.

## Queries

Highlight, injection, locals and tags queries ship inside the package. Resolve them
relative to the package root:

```js
const fs = require("node:fs");
const path = require("node:path");

const root = path.join(path.dirname(require.resolve("tree-sitter-fsharp")), "..", "..");

const highlights = fs.readFileSync(path.join(root, "queries", "highlights.scm"), "utf8");
const sigHighlights = fs.readFileSync(
  path.join(root, "fsharp_signature", "queries", "highlights.scm"),
  "utf8",
);
```

`queries/` holds `highlights.scm`, `indents.scm`, `injections.scm`, `locals.scm` and
`tags.scm` for the implementation grammar; `fsharp_signature/queries/` holds
`highlights.scm` and `tags.scm` for the signature grammar.

## WebAssembly

The published tarball also contains `tree-sitter-fsharp.wasm` and
`tree-sitter-fsharp_signature.wasm` at the package root, for use with
[`web-tree-sitter`][web-tree-sitter]. `Parser.init()` must resolve before anything else:

```js
const { Parser, Language } = require("web-tree-sitter");

await Parser.init();
const fsharp = await Language.load(
  require.resolve("tree-sitter-fsharp/tree-sitter-fsharp.wasm"),
);

const parser = new Parser();
parser.setLanguage(fsharp);
```

In a browser bundle, serve the two `.wasm` files as static assets and pass their URLs to
`Language.load` instead of a filesystem path.

## Contributing and grammar development

Grammar sources, the test corpus and the development workflow live in the repository:
see the [main README][repo] and [AGENTS.md][agents]. Bug reports for mis-parsed F# code
are welcome — reduce the snippet to a minimal example and open an issue or PR.

## License

MIT — see [LICENSE][license].

[tree-sitter]: https://tree-sitter.github.io/tree-sitter/
[web-tree-sitter]: https://www.npmjs.com/package/web-tree-sitter
[repo]: https://github.com/ionide/tree-sitter-fsharp#readme
[agents]: https://github.com/ionide/tree-sitter-fsharp/blob/main/AGENTS.md
[license]: https://github.com/ionide/tree-sitter-fsharp/blob/main/LICENSE
