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
//                  ^ type.builtin
//                        ^ punctuation.bracket
//                         ^ variable
//                          ^ variable

let x = ResizeArray<A>()
//<- keyword.function
//  ^ variable
//      ^ function.call
//                  ^ type
//                   ^ punctuation.bracket
//                    ^ variable
//                     ^ variable

let x = ResizeArray<{|word: string |}>()
//<- keyword.function
//  ^ variable
//      ^ function.call
//                  ^ punctuation.bracket
//                   ^ punctuation.bracket
//                    ^ property
//                          ^ type.builtin
