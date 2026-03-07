---
name: tree-sitter-tdd
description: Use when adding a new F# language feature to the tree-sitter parser with TDD workflow
---

# Tree-Sitter TDD Skill

## Overview

Use this skill when adding a new F# language feature to the tree-sitter parser. This skill automates the TDD workflow: write test first, get Fantomas AST, map to tree-sitter format, verify with user, then iterate on grammar until all tests pass.

**Critical:** This workflow loops until ALL tests pass. Adding a feature may break existing tests - all must be fixed.

## When to Use

- Adding a new language feature (e.g., new expression type, pattern, keyword)
- Fixing a parsing issue with existing syntax

## Input

The user provides:
1. **F# code snippet** to parse
2. **Test name** (e.g., "nullable type")
3. **Corpus file** (e.g., `test/corpus/type_defn.txt`)

## Workflow

### Step 1: Get Fantomas AST

Run Fantomas FCS to parse the F# code and get the AST:

```bash
dotnet fsi -r:nuget: Fantomas.FCS, 7.0.5 --exec - <<'EOF'
#r "nuget: Fantomas.FCS, 7.0.5"
let sample = fsi.CommandLineArgs.[0]
let result = Parse.parseFile false (SourceText.ofString sample) []
printfn "%A" result
EOF
```

Or use the helper script if available at project root.

### Step 2: Read Grammar

Read the parser's `src/grammar.json` to get valid node names:

```bash
cat fsharp/src/grammar.json | jq '.rules | keys'
```

### Step 3: Map AST to Tree-Sitter Format

Convert Fantomas AST to tree-sitter lisp format:

```
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

Use these mappings (add as needed):
- `Let` → `declaration_expression`
- `SynIdent` → `identifier`
- `Const` → `constant`
- `Int32` → `integer`
- `String` → `string`

### Step 4: Ask User for Unknown Nodes

If you encounter a Fantomas AST node that doesn't map to any node in grammar.json:

1. Propose a new node name based on F# language spec
2. Use `question` tool to ask user:

```
header: "New node name"
question: "Found Fantomas node 'X' that doesn't map to grammar. What's the tree-sitter node name?"
options:
  - label: "proposed_name"
    description: "Use proposed name based on F# spec"
  - label: "custom"
    description: "Enter custom name"
```

### Step 5: Verify Parse Tree

Show the user the expected parse tree and ask for verification:

```
header: "Verify parse tree"
question: "Does this parse tree look correct?"
options:
  - label: "Yes, add test"
    description: "Proceed to add test to corpus"
  - label: "No, fix mapping"
    description: "Revise node mappings"
```

### Step 6: Add Test to Corpus

Add the test to the specified corpus file:

```
================================================================================
test-name
================================================================================

<user's F# code>

--------------------------------------------------------------------------------

(expected parse tree)
```

### Step 6: Run Tests

```bash
cd fsharp && npx tree-sitter test
```

### Step 7: Fix Failures (Loop Until All Pass)

**IMPORTANT:** Adding a new language feature may break existing tests. You MUST run all tests and fix any failures before the feature is complete.

1. Check the test output for any failures
2. If failures exist:
   a. Use `tree-sitter-parse-testing` skill to debug each failing test
   b. Analyze the parse error and identify what grammar rule needs fixing
   c. Edit `grammar.js` to fix the issue
   d. Run `cd fsharp && npx tree-sitter generate` to regenerate the parser
   e. Run tests again with `cd fsharp && npx tree-sitter test`
   f. Repeat until all tests pass
3. Only proceed when ALL tests pass

### Step 8: Verify Complete

Confirm all tests pass and the new feature parses correctly.

## Test Format Example

```
================================================================================
nullable type
================================================================================

let x: int? = 1

--------------------------------------------------------------------------------

(file
  (declaration_expression
    (function_or_value_defn
      (value_declaration_left
        (identifier_pattern
          (long_identifier_or_op
            (identifier))))
      (type
        (nullable_type
          (type)))
      (const
        (integer)))))
```

## Parser Selection

- `.fs` / `.fsx` files → use `fsharp/` parser
- `.fsi` files → use `fsharp_signature/` parser

## Key Commands

| Command | Description |
|---------|-------------|
| `cd fsharp && npx tree-sitter test` | Run all tests |
| `cd fsharp && npx tree-sitter generate` | Regenerate parser after grammar changes |
| `echo 'code' \| npx tree-sitter parse` | Parse code snippet |
| `echo 'code' \| npx tree-sitter parse -d` | Debug parse (see tree-sitter-parse-testing) |
