// Asserts that tree-sitter still finds the errors F#'s own parser finds, in the
// same place.
//
// test/fsc-parse-failures.txt lists the corpus files FSC rejects. The `Parse
// examples` job passes them to parse-action as `invalid-files`, which only
// *tolerates* an error rather than requiring one: a listed file that parses
// cleanly produces a warning and still counts as a success. So a grammar that
// started accepting invalid F# goes green.
//
// Requiring "an ERROR somewhere" is not enough either. In FSharp.Core's inline
// IL files FSC reports a parse error at line 467 while tree-sitter errors at
// line 1 for an unrelated reason -- the file looks detected, but nothing about
// the real defect is being checked. So test/fsc-detected.txt records the line,
// and this asserts the ERROR is still within TOLERANCE of it.
//
// The list is append-only. A file that stops being detected, or whose error
// moves away from FSC's, fails by name and cannot be cleared by regenerating.
//
// Usage (from the repo root, with submodules checked out):
//     dotnet fsi scripts/check-fsc-detected.fsx
//
// Regenerate the list with scripts/record-fsc-detected.fsx, which asks FSC where
// each error is. This script deliberately needs nothing but the tree-sitter CLI,
// so CI does not restore a NuGet package to run it. Set TREE_SITTER to pick the
// CLI (default: on PATH).

open System
open System.Diagnostics
open System.IO

/// How far tree-sitter's ERROR may drift from FSC's line before this is treated
/// as a different error rather than the same one found slightly earlier or
/// later. Deliberately looser than the tolerance used when *recording* (2): the
/// recorder is a quality gate, this is a friction gate. A few lines of movement
/// during ordinary grammar work should not fail anyone's PR; a few hundred means
/// the file is only incidentally erroring and the entry has stopped meaning
/// anything.
let DRIFT_LIMIT = 10

let repoRoot = Path.GetFullPath(__SOURCE_DIRECTORY__ + "/..")
let listFile = Path.Combine(repoRoot, "test", "fsc-detected.txt")
let failuresFile = Path.Combine(repoRoot, "test", "fsc-parse-failures.txt")

let treeSitter =
    match Environment.GetEnvironmentVariable "TREE_SITTER" with
    | null | "" -> "tree-sitter"
    | value -> value

let readEntries path =
    if File.Exists path then
        File.ReadAllLines path
        |> Array.map (fun line -> line.Trim())
        |> Array.filter (fun line -> line <> "" && not (line.StartsWith "#"))
    else
        [||]

/// Runs tree-sitter over `paths`, returning the 1-based line of each file's
/// first ERROR. Output lines are "<path><padding>\tParse: ...\t(ERROR [r, c] ...)".
let treeSitterErrorLines (paths: string[]) =
    let listing = Path.GetTempFileName()

    try
        File.WriteAllLines(listing, paths)

        let psi = ProcessStartInfo(treeSitter)
        for arg in [ "parse"; "-q"; "-t"; "--paths"; listing ] do
            psi.ArgumentList.Add arg
        psi.WorkingDirectory <- repoRoot
        psi.RedirectStandardOutput <- true
        psi.RedirectStandardError <- true
        psi.UseShellExecute <- false

        use proc =
            try
                Process.Start psi
            with e ->
                eprintfn "could not run `%s`: %s" treeSitter e.Message
                eprintfn "Set TREE_SITTER to the CLI path, or put it on PATH."
                exit 2

        // Both pipes must be drained concurrently. Reading one to completion
        // first deadlocks as soon as the other fills its buffer -- which it
        // does when this is the run that compiles the parser and stderr
        // carries the whole build log.
        let stdoutTask = proc.StandardOutput.ReadToEndAsync()
        let stderrTask = proc.StandardError.ReadToEndAsync()
        proc.WaitForExit()
        let stdout = stdoutTask.Result
        let stderr = stderrTask.Result

        // tree-sitter prints one line per file, and exits non-zero whenever any
        // file has an error -- which is the normal case here, so the exit code
        // says nothing. Reporting on no files at all is the signal that the CLI
        // itself failed. Without this the script would read the silence as
        // "every recorded error disappeared" and cry regression on all of them.
        let reported =
            stdout.Split('\n') |> Array.filter (fun l -> l.StartsWith "examples/") |> Array.length

        if reported = 0 && paths.Length > 0 then
            eprintfn "`%s parse` reported on none of the %d files (exit %d)." treeSitter paths.Length proc.ExitCode
            eprintfn "This is a tooling failure, not a grammar regression."
            eprintfn ""
            eprintfn "%s" (stderr.Trim())
            exit 2

        stdout.Split('\n')
        |> Array.map (fun line -> line.TrimEnd('\r'))
        |> Array.choose (fun line ->
            if not (line.StartsWith "examples/") then
                None
            else
                let marker = line.IndexOf "(ERROR ["

                if marker < 0 then
                    None
                else
                    let rest = line.Substring(marker + 8)
                    let row = rest.Substring(0, rest.IndexOf ',')
                    // tree-sitter rows are 0-based, FSC lines are 1-based.
                    Some(line.Split('\t').[0].TrimEnd(), int row + 1))
        |> Map.ofArray
    finally
        File.Delete listing

let recorded =
    readEntries listFile
    |> Array.map (fun line ->
        let parts = line.Split('\t')
        parts.[0], int parts.[1])

if recorded.Length = 0 then
    eprintfn "%s is empty - record it first." listFile
    exit 1

let actual = treeSitterErrorLines (recorded |> Array.map fst)

let broken =
    recorded
    |> Array.choose (fun (path, expected) ->
        match Map.tryFind path actual with
        | None -> Some(path, expected, "reports no ERROR at all")
        | Some line when abs (line - expected) > DRIFT_LIMIT ->
            Some(path, expected, sprintf "now errors at line %d instead" line)
        | Some _ -> None)
    |> Array.sortBy (fun (path, _, _) -> path)

if broken.Length > 0 then
    eprintfn "REGRESSION: tree-sitter no longer reports the error F# reports here:"
    eprintfn ""

    for path, expected, why in broken do
        eprintfn "  %s" path
        eprintfn "      F# errors at line %d, but tree-sitter %s" expected why

    eprintfn ""
    eprintfn "The grammar now accepts syntax F# rejects. Tighten the rule rather than"
    eprintfn "removing entries from test/fsc-detected.txt. If the error genuinely"
    eprintfn "belongs somewhere else now, re-record with record-fsc-detected.fsx and"
    eprintfn "say so in the PR."
    exit 1

eprintfn "all %d recorded errors still found" recorded.Length
