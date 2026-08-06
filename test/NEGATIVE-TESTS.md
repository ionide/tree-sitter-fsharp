# Negative tests

`test/corpus/invalid/` holds **negative** test cases: F# that is *not* valid,
where the parser is expected to report an `ERROR` node. They run as part of
`npx tree-sitter test` like any other corpus file, so CI already covers them.

They exist because the rest of the test suite only pushes one way. `test/files.txt`
and the `Parse examples` CI job reward making more files parse, and nothing pushes
back when the grammar becomes loose enough to accept syntax F# rejects. A grammar
that accepted every input would score perfectly on the sample corpus.

## Writing a case

Use the `:error` attribute and leave the expected-tree section empty — the test
passes when the parse contains an `ERROR` node, and the tree itself is not checked:

```
================================================================================
open declaration with no name
:error
================================================================================

open

--------------------------------------------------------------------------------
```

`tree-sitter test -u` cannot silence an `:error` test. It reports the failure as
an "update" and exits 0, but it does not rewrite the file, so the next plain run
fails again.

Keep snippets minimal — the smallest input that isolates the error. If a case
needs module context (`val` is only invalid *in an implementation module*),
include the `module M` header; otherwise leave it out.

## Every case must be backed by FSC

Before adding a case, confirm F#'s own parser rejects it. A negative test that
asserts an error on legal F# is worse than no test at all:

```fsharp
#r "nuget: Fantomas.FCS, 7.0.5"
open Fantomas.FCS
let sample = "open\n"
let _ast, diags = Parse.parseFile false (Text.SourceText.ofString sample) []
diags |> List.filter (fun d ->
    d.Severity = Diagnostics.FSharpDiagnosticSeverity.Error && d.SubCategory = "parse")
```

A non-empty result means the snippet belongs here.

## What belongs here — and what does not

A tree-sitter grammar is not a compiler, and FSC rejects plenty of input for
reasons that are not the grammar's job. Only **structural syntax errors** belong:
wrong token order, missing required parts, constructs in contexts where the
grammar does not allow them, unterminated lexical constructs.

Deliberately **out of scope**, with the reasons:

| Category | Why not |
|---|---|
| `#light "off"` / `#indent "off"` | Deprecation, not a syntax error. Accepting these is arguably correct. |
| Offside / strict-indentation errors | Depend on `--langversion` and `--strict-indentation`. |
| Numeric literal ranges (`-129y`, `256uy`, `7.9e+28M`) | Value-range checks, not grammar. |
| Byte/Unicode char literal validity (`'♠'B`) | Lexical semantics; low value. |
| TABs not allowed | Conditional on `#indent "off"`. |
| `type T as this = …` | Error depends on whether the type has a primary constructor — semantic. |
| "`get` and/or `set` required", "abstract slots always have the same visibility", "a type definition requires one or more members" | Semantic rules, not parseable structure. |

Every case in this directory passes today. Do not add one that fails, and never
delete a case to make a change pass — the same rule that applies to expected
parse trees in `AGENTS.md` applies here.

## Not yet covered

These are F# parse errors that tree-sitter currently accepts. They are *not*
tests — adding them would leave the suite red — but each is a verified defect and
a ready-made test once the grammar handles it. Add the case to the matching file
here at the same time as the fix.

**Keywords falling back to identifiers.** `word: $ => $.identifier` gives
tree-sitter keyword extraction, but it falls back to the word token whenever the
keyword is not valid in the current parse state — so a keyword in a position the
grammar does not expect silently becomes an identifier. The `reserved` word set
in `fsharp/grammar.js` closes this for `as`, `namespace`, `open` and `type`. Two
keywords are deliberately left out, and each leaves a gap:

- **`member`** — `member this.Size = 1` at module level. Reserving it breaks 16
  valid files in `examples/`: the grammar relies on `member` lexing as an
  identifier to recover from over-indented member declarations (an `inherit
  Base()` followed by a more-indented `static member ...`), which FSC accepts.
  Modelling that indentation properly would let `member` join the set.
- **`val`** — `val x : int` in an implementation module, which parses as
  `(typecast_expression (application_expression "val" "x") "int")`. Reserving it
  is blocked only by the `basic long namespace` test in `source_file.txt`, which
  asserts a tree for `namespace test.val` — input FSC rejects with "Unexpected
  start of structured construct". Deciding that test is wrong unblocks this.

`private type Foo() = class end` is the same shape (`private` parses as a
standalone top-level identifier expression), but `access_modifier` is a
`token(prec(...))` rather than a bare keyword string, so it needs more than a
word-set entry.

**Others**, each verified as an FSC parse error:

| snippet | what F# says | note |
|---|---|---|
| `let o = new int T ()` | Unexpected identifier in expression | postfix type application; `new` takes an expression, not a type |
| `let l = [1;;2]` | Unexpected symbol ';;' in expression | `common/scanner.h` deliberately treats `;;` as `;` for `#light "off"` code |
| `let ( ~>. ) x y = x + y` | Invalid operator definition | needs a tighter `op_identifier` token regex; cheap in parser size, easy to over-tighten |
| `let (?!!) x y = x` | as above | `(?)` and `(?<-)` are legal, so the regex must not reject all `?` operators |

**`Array.empty<>`** is a special case. F# rejects an empty type argument list, but
`test/corpus/expr.txt` has an explicit `empty typed expression` test asserting
that tree-sitter parses it as a `typed_expression`. Making `type_arguments`
non-empty is a one-line change, but it contradicts that test, so it needs a
decision about which behaviour is intended rather than a quiet fix.

## Where the cases came from

They were derived from [`fsc-parse-failures.txt`](fsc-parse-failures.txt) — the
613 corpus files F#'s own parser rejects. tree-sitter already reports an `ERROR`
for 455 of them and accepts the other 158. Excluding `#light "off"` (21) and
indentation-sensitive cases (15) leaves 122 genuine syntax errors, which were
distilled into the minimal snippets here.

To repeat that analysis after grammar changes:

```bash
dotnet fsi scripts/find-fsc-parse-failures.fsx
```

then parse the resulting list and look for files that no longer produce an
`ERROR` — each one is a candidate for a new negative case.
