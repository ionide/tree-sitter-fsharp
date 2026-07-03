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

The `tree-sitter` CLI is installed as a local devDependency (`node_modules/.bin/tree-sitter`), not globally. Pick one:

- `npx tree-sitter <command>` — resolves the local binary automatically.
- `source ./activate` (from the repo root) — puts `node_modules/.bin` on `$PATH` for the current shell, so plain `tree-sitter <command>` works. Run `deactivate` to undo. See the README for details.

All examples in this document use `npx tree-sitter ...`; if you have sourced `./activate` you can drop the `npx` prefix.

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
#r "nuget: Fantomas.FCS, 7.0.5"
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

This runs `tree-sitter generate` inside each parser directory and produces
both `fsharp/src/{grammar.json,parser.c}` and
`fsharp_signature/src/{grammar.json,parser.c}`. Because
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

**IMPORTANT**: You may never change the expected parse tree of an existing test without consulting the user first. If a test fails to parse it is because the parser is broken.
