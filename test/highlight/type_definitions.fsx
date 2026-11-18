type A() =
//<- keyword.type
//   ^ type.definition
  member this.F(x) = ()
//^ keyword.function
//       ^ variable.parameter.builtin
//            ^ function.method

// A capitalised application head is a constructor: an object construction
// without `new`, a union case, an exception.
let x = ResizeArray<string>()
//<- keyword.function
//  ^ variable
//      ^ constructor
//                  ^ type.builtin
//                        ^ punctuation.bracket

let x = ResizeArray<A>()
//<- keyword.function
//  ^ variable
//      ^ constructor
//                  ^ type
//                   ^ punctuation.bracket

let x = ResizeArray<{|word: string |}>()
//<- keyword.function
//  ^ variable
//      ^ constructor
//                  ^ punctuation.bracket
//                   ^ punctuation.bracket
//                    ^ property
//                          ^ type.builtin

type Shape =
    | Circle of radius: float
    //^ constructor
    //          ^ variable.member
    //                  ^ type.builtin
    | Rect of float * float
    //^ constructor
    member this.Area = 0.0
    //     ^ variable.parameter.builtin
    //          ^ function.method
    static member Create() = Circle 1.0
    //     ^ keyword.function
    //            ^ function
    //                       ^ constructor
    abstract member Describe : int -> string
    //              ^ function.member
    //                         ^ type.builtin
    member val Count = 1 with get, set
    //         ^ function.member
    //                        ^ keyword

type Level = | Low = 1 | High = 2
//             ^ constructor
//                   ^ number

type Box<'T> = { Value: 'T }
//       ^ type
//               ^ property
//                      ^ type

type Rec = { mutable Name: string }
//           ^ keyword.modifier
//                   ^ property

exception Boom of string
//<- keyword.type
//        ^ type.definition

[<AutoOpen>]
module Inner =
//     ^ module
    let inline zero< ^a when ^a : (static member Zero : ^a)> (x: ^a) = x
    //                                            ^ function.member

let speed = 1.0<m/s>
//          ^ number.float
//              ^ type

let describe (this: A) = this.F(1) + base.F(2)
//                       ^ variable.builtin
//                            ^ function.call
//                                   ^ variable.builtin

type Level =
    | [<System.Obsolete>] Low = 0
//                        ^ constructor
    | High = 1
//    ^ constructor

exception Wrapped = System.ArgumentException
//                  ^ type
