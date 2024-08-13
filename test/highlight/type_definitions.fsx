type A() =
//<- keyword.type
//   ^ type.definition
  member this.F(x) = ()
//^ keyword.function
//       ^ variable.parameter.builtin
//            ^ function.method

let x = ResizeArray<string>()
//<- keyword.function
//  ^ variable
//      ^ function.call
//                 ^ operator
//                  ^ type.builtin
//                        ^ operator
//                         ^ variable
//                          ^ variable

let x = ResizeArray<A>()
//<- keyword.function
//  ^ variable
//      ^ function.call
//                 ^ operator
//                  ^ type
//                   ^ operator
//                    ^ variable
//                     ^ variable

let x = ResizeArray<{|word: string |}>()
//<- keyword.function
//  ^ variable
//      ^ function.call
//                 ^ operator
//                  ^ punctuation.bracket
//                   ^ punctuation.bracket
//                    ^ property
//                          ^ type.builtin
