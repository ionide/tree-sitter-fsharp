namespace Company.Product
// <- keyword
//        ^ module

open System
// <- keyword.import
//   ^ module

/// A documented type.
// <- comment.documentation
type Shape =
// <- keyword.type
//   ^ type.definition
    | Circle of float
    //^ constant
    //          ^ type.builtin
    | Rect of float * float
    //              ^ operator

type Box<'T> = { Value: 'T }
//               ^ property

[<Sealed>]
// ^ attribute
type Service =
    abstract member Run : unit -> unit
    //              ^ function.member
    //                    ^ type.builtin
    abstract member Create : name: string -> Service
    //              ^ function.member
    //                       ^ variable.parameter

module Helpers =
//     ^ module
    val add : int -> int -> int
    //  ^ variable
    //        ^ type.builtin
    val inline zero : unit
    //  ^ keyword.modifier
    //         ^ variable
    val mutable counter : int
    //          ^ variable

[<Sealed>]
type Counter =
    new: start: int -> Counter
    //   ^ variable.parameter
    member Value: int
    //     ^ function.member
    static member Create: start: int * ?step: int -> Counter
    //            ^ function.member
    //                    ^ variable.parameter
    //                                 ^ character.special
    interface System.IDisposable
    // <- keyword.type

val describe: x: int | null -> string
//            ^ variable.parameter
//               ^ type.builtin
