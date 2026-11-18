# Improving the F# Tree-sitter Parser

This document explains how to add new language features to the F# parser.

## Project Structure

This project contains two parsers:

1. **`fsharp/`** - Parses `.fs` and `.fsx` files (F# source and script files)
2. **`fsharp_signature/`** - Parses `.fsi` files (F# signature files)

**These are not independent.** `fsharp_signature/grammar.js` starts with
`grammar(require("../fsharp/grammar"), { ... })` — it extends the base
grammar and overrides/adds a small number of rules. Consequences you must
internalize:

- A change to `fsharp/grammar.js` changes the *derived* signature grammar
  too, even if you never open `fsharp_signature/grammar.js`.
- Both `fsharp/src/parser.c` **and** `fsharp_signature/src/parser.c` must
  be regenerated after any change to `fsharp/grammar.js`. Forgetting the
  second one will fail CI's "parser matches grammar" check.
- Use `npm run generate` (regenerates both) rather than a single
  `tree-sitter generate` invocation — see the workflow section below.

Each parser directory contains:

- `grammar.js` - The grammar definition (written in JavaScript)
- `src/grammar.json` - Generated grammar in JSON format
- `src/parser.c` - Generated C parser code (never edit this file directly)
- `src/scanner.c` - Thin per-parser shim that `#include`s the shared scanner

The **real** external scanner logic lives in a single shared header at
[`common/scanner.h`](./common/scanner.h). Both `fsharp/src/scanner.c` and
`fsharp_signature/src/scanner.c` are just three-function shims that delegate
to functions defined in that header. If you need to change tokenization
behavior (indent/dedent, keyword handling, string interpolation, etc.),
edit `common/scanner.h` — never the per-parser `scanner.c` shim.

## Running the CLI

The `tree-sitter` CLI is installed as a local devDependency
(`node_modules/.bin/tree-sitter`), not globally. Invoke it with
`npx tree-sitter <command>`, which resolves the local binary
automatically. All examples in this document use `npx tree-sitter ...`.

## Workflow for Adding a New Feature

### 1. Create a Test Case First

Before implementing anything, write a test case in the appropriate corpus file under `test/corpus/`. Tests for `fsharp` go in `test/corpus/*.txt`, and tests for `fsharp_signature` go in `test/corpus/fsharp_signature/*.txt`.

Test format (from [tree-sitter docs](https://tree-sitter.github.io/tree-sitter/creating-parsers/index.html#the-test-format)):

```
================================================================================
test name
================================================================================

<input code>

--------------------------------------------------------------------------------

<expected parse tree>
```

Example from `test/corpus/constants.txt`:

```
================================================================================
simple string
================================================================================

let x = "test"

--------------------------------------------------------------------------------

(file
  (declaration_expression
    (function_or_value_defn
      (value_declaration_left
        (identifier_pattern
          (long_identifier_or_op
            (identifier))))
      (const
        (string)))))
```

To define the expected parse tree you should use the following fsi script to obtain the fsharp AST.
Translate the AST into the lisp like format ued by tree-sitter. Refer to the grammar.json for the correct node names and structure.
If the appropiate node names are not present in the grammar, you may need to update grammar.js to add the necessary rules and regenerate the parser before you can write the test.

```fsi
#r "nuget: Fantomas.FCS, 8.1.1"
let sample = ""
Parse.parseFile false (SourceText.ofString sample) []
```

where sample if the code you want to parse.

### 2. Run the Test

From the repo root (so `tree-sitter.json` picks up both parsers):

```bash
npx tree-sitter test
```

To limit to a single test file or a single test name:

```bash
npx tree-sitter test --file-name attributes.txt
npx tree-sitter test -i "top-level module attribute"
npx tree-sitter test --overview-only   # pass/fail summary only
```

To parse a single file (useful for iterating on real F# samples):

```bash
npx tree-sitter parse path/to/file.fs
# .fsi files need the signature parser selected explicitly:
npx tree-sitter parse -p fsharp_signature path/to/file.fsi
```

### 3. Implement the Feature

Update `grammar.js` in the appropriate parser directory to add the new grammar rule. If a feature applies to both `.fs` and `.fsi`, update **both** `fsharp/grammar.js` and `fsharp_signature/grammar.js`.

If you need to add special tokenization logic (indent/dedent behavior, keyword handling that competes with identifiers, string interpolation, etc.), edit the shared scanner at `common/scanner.h` — not the per-parser `src/scanner.c` shim.

### 4. Regenerate the Parser(s)

After modifying `grammar.js`, regenerate `grammar.json` and `parser.c` for
**both** parsers. The safe default is:

```bash
npm run generate
```

This runs `tree-sitter generate` once per parser and produces both
`fsharp/src/{grammar.json,parser.c}` and
`fsharp_signature/src/{grammar.json,parser.c}`. It uses `--output` and `&&`
rather than a shell loop, so it works on Windows (where npm runs scripts through
`cmd.exe`) as well as on macOS and Linux. Because
`fsharp_signature/grammar.js` extends `fsharp/grammar.js`, a change to the
base grammar shows up in the signature parser too — so this must be run
even if you only edited `fsharp/grammar.js`.

If you invoke `tree-sitter generate` directly, always pass `--output` or it
will dump a stray `src/` directory at the repo root:

```bash
npx tree-sitter generate --output fsharp/src           fsharp/grammar.js
npx tree-sitter generate --output fsharp_signature/src fsharp_signature/grammar.js
```

Commit the regenerated `grammar.json` and `parser.c` for both parsers
alongside your `grammar.js` change. CI runs a "parser matches grammar"
check that regenerates and diffs against the committed files; a stale
generated file in either parser fails the build.

**Merge conflicts in `parser.c` or `grammar.json`:** don't hand-edit them.
Resolve by re-running `npm run generate` on top of the merged `grammar.js`
and committing the result.

Changes to `common/scanner.h` do **not** require regeneration — they're
picked up automatically on the next build. But you do need `-r` on the
next test run to force a rebuild of the compiled scanner, otherwise
tree-sitter reuses the previously compiled artifact and you'll see stale
results:

```bash
npx tree-sitter test -r
```

### 5. Run Tests Again

Validate the feature is working:

```bash
npx tree-sitter test
```

### 6. Repeat as Necessary

If the test still fails, review your implementation and the expected parse tree. Make sure your grammar rules correctly capture the syntax of the new feature.

## Negative Tests

`test/corpus/invalid/` asserts the *opposite* of the rest of the suite: these are
snippets that are **not** valid F#, and the parser is expected to report an
`ERROR` node. They use the `:error` attribute and an empty expected-tree section,
and run under the same `npx tree-sitter test`.

This is the counterweight to making sample files parse: widening a rule until a
failing file goes green is easy, and nothing else in the suite notices when the
grammar starts accepting syntax F# rejects.

Only **structural** syntax errors belong — not type or name-resolution errors
(`let y = 4 + y` parses fine), and not offside errors (`--langversion`-dependent).
Before adding a case, confirm FSC reports a diagnostic with `SubCategory = "parse"`
using the script in step 1; asserting an error on legal F# is worse than no case.
Every case passes today — never add a failing one, mark one `:skip`, or delete one
to make a change pass.

`test/fsc-detected.txt` is the same idea at corpus scale: files F#'s parser
rejects, with the line it rejects them on, where tree-sitter reports an ERROR at
that same line. `npm run check:invalid` asserts it still does, so a loosened rule
that starts accepting invalid F# fails by name. Files where tree-sitter errors
somewhere unrelated are deliberately not listed — they would look like coverage
while checking nothing. The list is append-only; regenerate with
`dotnet fsi scripts/record-fsc-detected.fsx` and never delete an entry to go
green.

## Releasing a Version

`npx tree-sitter version X.Y.Z` bumps `tree-sitter.json`, `package.json`, `Cargo.toml`,
`pyproject.toml` and `CMakeLists.txt`, but not the `Makefile`, `package-lock.json`
(two lines) or `bindings/swift/README.md`; edit those by hand. Then run
`npm run generate` and commit both `parser.c` files: the generated parser embeds the
version as `TSLanguageMetadata`, and CI's "Verify generated parser" step fails on a
bump that leaves them stale. Move the `Unreleased` block of `CHANGELOG.md` under the
new version heading.

## Parse Baseline

`test/parse-baseline.txt` records, for every corpus file under `examples/` that
parses with errors, how many `ERROR` and `MISSING` nodes it has. `npm run
check:baseline` (`dotnet fsi scripts/check-parse-baseline.fsx`) fails when any file has
more than recorded, or a clean file gains some; CI runs it in the `Parse examples`
job. The parse step itself only notices a file that yields no tree at all, and a
tree full of error nodes still counts as parsed there.

A change that makes files parse better is reported as such - accept it with
`--update` and commit the new baseline with the grammar change. A change that makes
files parse worse must either be fixed or explained in the PR before the baseline
is loosened. The file is generated; never edit it by hand.

The same run checks `test/parser-size.txt`: the byte size of each generated `parser.c`,
which tracks the LR state count. A parser more than 15% larger than recorded fails, so a
rule that quietly doubles the tables is caught even when every test passes. `--update`
records the current sizes once the growth has been looked at and accepted. Record it from an LF checkout of the
submodules, as CI has: some corpus files get CRLF endings on Windows and a few of those
parse with a different number of error nodes (`git -C examples/FSharp.Compiler config
core.eol lf`, then `git -C examples/FSharp.Compiler checkout-index -a -f`).

Error recovery belongs to the tree-sitter runtime inside the CLI, so a CLI upgrade alone
can change the count of a file that already has errors (0.27.0 to 0.27.2 moved three
files by one or two nodes). Record the baseline with the CLI version in
`package-lock.json`, which is the one CI installs.

The tree-sitter CLI caches compiled parsers by grammar *name* under
`~/.cache/tree-sitter/lib`; another checkout whose grammar is also called `fsharp`
(a second worktree, or MangelMaxime's grammar) silently overwrites it. Set
`TREE_SITTER_LIBDIR` to a per-checkout directory when working with more than one.

## Queries

`queries/` holds the editor-facing queries for the `fsharp` grammar and
`fsharp_signature/queries/` the ones for `.fsi` files. Capture names and dialects
follow nvim-treesitter (`@keyword.conditional`, `@variable.member`, `@indent.begin`,
`@function.inner`, `@fold`); Helix and Zed keep their own copies mapped from these.

| file | consumers | tested by |
|---|---|---|
| `highlights.scm` | every editor | `test/highlight/*.fsx` (`.fsi` files test the signature grammar) |
| `locals.scm` | scope-aware highlighting | `tree-sitter test` compiles it |
| `injections.scm` | markdown in `(** *)`, xml in `///` | CI compile check |
| `indents.scm`, `folds.scm`, `textobjects.scm` | Neovim | CI compile check |
| `tags.scm` | symbol navigation (GitHub, difftastic, ...) | `test/tags/*.fs` |

Three resolution rules decide the order of `highlights.scm`: when several patterns
capture the same node, the last one in the file wins; a capture on a child node
overrides one on its parent; and tree-sitter-highlight (the CLI and Helix, not
Neovim) drops the remaining captures of a match once a later pattern captures the
same node as that match's first capture. That is why there is no
`(identifier) @variable` fallback (it would override
`(argument_patterns) @variable.parameter` and `(_type) @type` from inside), why
`@spell` is listed before the colour capture it shares a pattern with, why the
module-path and `@type.builtin` rules sit after the `long_identifier` member rule
they override, and why every rule that captures identifiers in expressions (member
paths, calls, pipes, constructors, builtins) is grouped at the end of the file in
general-to-specific order. `tree-sitter query` shows every capture regardless, so a
highlight assertion is the only check for the third rule.

Every capture in `highlights.scm` must be pinned by a `test/highlight` assertion:
`npm run check:highlights` (`scripts/highlight-coverage.sh --check`) fails in CI
otherwise. `tree-sitter test` checks that the expected name is among the highlights
at that position, so an assertion also catches a rule that stopped matching.

`tree-sitter query -p fsharp queries/<file>.scm some.fsx` prints every capture with
its range and text; that is the quickest way to see what a rule does on real code.
CI compiles every query the same way, which catches a node name that no longer
exists in queries `tree-sitter test` never loads.

## References

- **Tree-sitter Documentation**: https://tree-sitter.github.io/tree-sitter/creating-parsers/index.html
- **F# Language Specification**: https://fsharp.github.io/fslang-spec/

## Important Notes

1. **Always add tests first** - This ensures you understand the expected behavior and can verify your implementation.

2. **Use `tree-sitter test` to validate** - Don't assume the feature works; run the tests to confirm.
   - If the test run does not exit relatively fast it is likely you have an infinite loop in your grammar.
     This is most likely an issue with the `c` parser injecting tokens into the grammar without consuming any tokens. If you have a infinite loop you should cancel the test run and review your grammar rules and scanner logic.
   - A test is **only** considered passing if the expected parse tree matches the actual parse tree exactly. Even a small difference in node names or structure should be considered a failure and should be investigated.

3. **Check the F# spec** - When in doubt about language syntax, refer to https://fsharp.github.io/fslang-spec/

4. **Two parsers, one grammar chain** - `fsharp_signature` extends `fsharp`, so any change to `fsharp/grammar.js` requires regenerating **both** `fsharp/src/parser.c` and `fsharp_signature/src/parser.c`. `npm run generate` handles this; a single `tree-sitter generate` call does not. CI will fail if either is stale.

5. **Grammar.js vs parser.c** - You should only edit `grammar.js`. The `parser.c` file is auto-generated and will be overwritten when you run `tree-sitter generate`. Merge conflicts in `parser.c`/`grammar.json` are resolved by regenerating, not by hand-editing.

6. **Never weaken a negative test** - Do not delete a case from `test/corpus/invalid/`, and do not add `:skip` to one that was previously enforced, in order to make a change pass. A negative test that starts failing means the grammar now accepts invalid F#.

**IMPORTANT**: You may never change the expected parse tree of an existing test without consulting the user first. If a test fails to parse it is because the parser is broken.
