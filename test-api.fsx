#r "nuget: Fantomas.FCS, 7.0.5"

open System
open Fantomas.FCS
open Fantomas.FCS.Text
open Fantomas.FCS.Syntax
open Microsoft.FSharp.Reflection

let code = "let x = 1"
let ast, _ = Parse.parseFile false (SourceText.ofString code) []

let rec findConst (indent: int) (x: obj) =
    let prefix = String.replicate indent "  "
    let t = x.GetType()
    if FSharpType.IsUnion(t) then
        let case, fields = FSharpValue.GetUnionFields(x, t)
        if case.Name = "Const" then
            printfn "%sFound Const!" prefix
            fields |> Array.iteri (fun i f ->
                if isNull f then
                    printfn "%s  Field %d: NULL" prefix i
                else
                    let fType = f.GetType()
                    printfn "%s  Field %d: Type=%s Value=%s" prefix i fType.Name (f.ToString())
            )
        fields |> Array.iter (fun f ->
            if not (isNull f) && FSharpType.IsUnion(f.GetType()) then
                findConst (indent + 1) f
        )

findConst 0 ast
