---
name: tree-sitter-parse-testing
description: Use when testing tree-sitter parser behavior for specific F# code snippets
---

# Tree-Sitter Parse Testing

## Overview
Use `tree-sitter parse -d` to debug parsing issues. The debug output shows the state machine transitions, helping identify where parsing fails.

## Parsing a Code Snippet

Run from the parser directory (`fsharp/` or `fsharp_signature/`):

```bash
echo 'let x = 1' | npx tree-sitter parse -d
```

## Interpreting Output

### Successful Parse
```
(file [0, 0] - [1, 0]
  (declaration_expression [0, 0] - [0, 9]
    ...))
```
- Node format: `(nodename [start_row, start_col] - [end_row, end_col])`
- Children indented inside parent nodes

### Parse Errors
```
(ERROR [0, 0] - [0, 7]
  ...)
```
- ERROR nodes indicate parsing failure at that location

### Missing Tokens
```
(MISSING identifier [0, 8] - [0, 8])
```
- MISSING indicates expected token that wasn't found

## Debug Output Analysis

Key patterns to look for:

| Pattern | Meaning |
|---------|---------|
| `detect_error` | Parser encountered unexpected token |
| `recover_with_missing` | Parser inserted placeholder to recover |
| `recover_with_error` | Parser skipped tokens to recover |
| `shift state:N` | Parser shifted to state N |
| `reduce sym:X` | Parser reduced using rule X |
| `accept` | Parse completed successfully |

## Debugging Workflow

1. Run parse without `-d` to see the tree
2. If tree looks wrong (ERROR nodes, unexpected structure):
3. Run with `-d` to see the debug output
4. Look for `detect_error` near the problem location
5. Check which state the parser was in when error occurred
6. Compare expected tokens at that state against actual input

## Infinite Loops

If the debug output repeatedly emits the same token or state indefinitely, the parser has entered an infinite loop. This typically happens when:
- A grammar rule injects tokens without consuming any input
- The scanner (`src/scanner.c`) is returning tokens that don't advance the parse position

** Symptoms:**
- Debug output shows the same token being processed repeatedly
- The parser never reaches `accept` or an error state
- Command appears to hang

**What to do:**
1. Interrupt the command (Ctrl+C)
2. Review the grammar rules and scanner logic
3. Look for rules that could cause the parser to re-enter the same state without consuming input

## Example Debug Session

```bash
# 1. See the parse tree
echo 'type X = ' | npx tree-sitter parse

# 2. Debug output shows recovery
echo 'type X = ' | npx tree-sitter parse -d | grep -A5 detect_error
```

The debug output shows the parser detecting an error at column 8 and recovering by inserting a MISSING identifier.

## Common Issues

- **Unexpected ERROR node**: Run with `-d`, find `detect_error`, check what tokens were expected
- **MISSING token**: Parser expected something that wasn't there
- **Extra nodes**: Check if grammar rule is too greedy (look at reduce steps)
