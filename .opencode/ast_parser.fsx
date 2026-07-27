#r "nuget: Fantomas.FCS, 7.0.5"
open Fantomas.FCS.Text
open Fantomas.FCS.Parse
let sample = fsi.CommandLineArgs |> Seq.skip 1 |> String.concat " "
let ast, _ = parseFile false (SourceText.ofString sample) []
printfn "%A" ast
