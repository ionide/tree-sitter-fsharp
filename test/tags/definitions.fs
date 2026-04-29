module TopLevel
//     ^^^^^^^^ definition.module

let topValue = 1
//  ^^^^^^^^ definition.constant

let topFunction x = x + 1
//  ^^^^^^^^^^^ definition.function

type Person = { Name: string }
//   ^^^^^^ definition.class

exception Boom of string
//        ^^^^ definition.class

type Service() =
//   ^^^^^^^ definition.class
  member this.Run() =
  //          ^^^ definition.method
    let localValue = 1
    let localFunction y = y + localValue
    localFunction localValue

type IWorker =
//   ^^^^^^^ definition.class
  abstract member Work : unit -> unit
  //              ^^^^ definition.method

[<Measure>]
type kg
//   ^^ definition.class

module Inner =
//     ^^^^^ definition.module
  let innerVal = 42
  //  ^^^^^^^^ definition.constant
