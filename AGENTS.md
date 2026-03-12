# Improving the F# Tree-sitter Parser

This document explains how to add new language features to the F# parser.

## Project Structure

This project contains two distinct parsers:

1. **`fsharp/`** - Parses `.fs` and `.fsx` files (F# source and script files)
2. **`fsharp_signature/`** - Parses `.fsi` files (F# signature files)

Each parser directory contains:

- `grammar.js` - The grammar definition (written in JavaScript)
- `src/grammar.json` - Generated grammar in JSON format
- `src/parser.c` - Generated C parser code (This file should never edited directly)
- `src/scanner.c` - External scanner for handling complex tokenization

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

Run the test to see the current failure:

```bash
# For fsharp parser
cd fsharp && npx tree-sitter test

# For fsharp_signature parser
cd fsharp_signature && npx tree-sitter test
```

### 3. Implement the Feature

Update `grammar.js` in the appropriate parser directory to add the new grammar rule. If you need to add special tokenization logic (like handling keywords that can be used as identifiers), you may need to modify `src/scanner.c`.

### 4. Regenerate the Parser

After modifying `grammar.js`, regenerate `grammar.json` and `parser.c`:

```bash
# For fsharp parser
cd fsharp && npx tree-sitter generate

# For fsharp_signature parser
cd fsharp_signature && npx tree-sitter generate
```

### 5. Run Tests Again

Validate the feature is working:

```bash
# For fsharp parser
cd fsharp && npx tree-sitter test

# For fsharp_signature parser
cd fsharp_signature && npx tree-sitter test
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

4. **Two parsers need to be in sync** - If you're adding a feature that applies to both `.fs` and `.fsi` files, you need to implement and test it in both `fsharp/` and `fsharp_signature/` directories.

5. **Grammar.js vs parser.c** - You should only edit `grammar.js`. The `parser.c` file is auto-generated and will be overwritten when you run `tree-sitter generate`.
