let f x = x + 1
// <- keyword.function
//  ^ function
//      ^ operator
//          ^ operator
//            ^ number

let add (a: int) b = a + b
//       ^ variable.parameter
//          ^ type.builtin
//               ^ variable.parameter

// Qualified application heads and the function side of a pipe.
let doubled = List.map (fun n -> n * 2) [ 1; 2 ]
//            ^ module.builtin
//                 ^ function.call
//                      ^ keyword
//                          ^ variable.parameter
//                                      ^ punctuation.bracket
//                                         ^ punctuation.delimiter

let total = [ 1; 2 ] |> List.sum
//                   ^ operator
//                           ^ function.call

let piped = 3 |> f
//               ^ function.call

let composed = f >> string
//               ^ operator
//                  ^ function.call

let name = nameof f
//         ^ function.builtin

let size = sizeof<int>
//         ^ function.builtin

let quoted = <@ 1 + 2 @>
//           ^ punctuation.special
//                    ^ punctuation.special

let answer = if f 1 > 1 then Some 2 else None
//           ^ keyword.conditional
//                      ^ keyword.conditional
//                           ^ constructor
//                                ^ number
//                                  ^ keyword.conditional
//                                       ^ constructor

let describe value =
    match value with
    //          ^ keyword.conditional
    | Some n when n > 0 -> "positive"
    //^ constructor
    //     ^ variable
    //       ^ keyword.conditional
    //                  ^ operator
    | _ -> "other"
    //^ character.special

let loop () =
    for i in 1 .. 3 do
    //^ keyword.repeat
    //    ^ keyword
    //         ^ operator
        printfn "%d" i
    while false do ()
    //^ keyword.repeat

let compute () = async {
    //           ^ keyword
    let! value = async { return 1 }
    //^ keyword.function
    //                   ^ keyword.return
    return value
    //^ keyword.return
}

let fetch url =
    task {
    //^ keyword
        let! first = get url
        //   ^ variable
        and! second = get url
        //^ keyword.function
        //   ^ variable
        use! conn = openAsync url
        //^ keyword.function
        //   ^ variable
        use reader = conn.Reader
        //  ^ variable
        do! flush conn
        //^ keyword
        while! reader.MoveNext url do
        //^ keyword.repeat
            ignore first
        match! reader.Read url with
        //^ keyword.conditional
        | 0 -> return! retry url
        //     ^ keyword.return
        | n -> return n + second
    }

let pinned (arr: int[]) =
    use ptr = fixed arr
    //        ^ keyword
    ()

let safe () =
    try
    //^ keyword.exception
        failwith "boom"
    //  ^ keyword.exception
    with
    //^ keyword.exception
    | ex -> ex.Message
    //      ^ variable.member
    //         ^ property

let cast (o: obj) = o :?> string
//                    ^ operator

let negated = not true
//            ^ keyword.operator

let p = { Name = "x"; Age = 1 }
//        ^ property
//                    ^ property

let n: int = 1
//     ^ type.builtin

let nothing = ()
//            ^ constant.builtin

let absent = null
//           ^ constant.builtin

let mutable counter = 0
//  ^ keyword.modifier
counter <- counter + 1
//      ^ operator

[<Literal>]
// ^ attribute
let maxSize = 100
//  ^ constant

let sorted = query { for x in [ 1 ] do sortByDescending x }
//                                     ^ keyword

let results =
    query {
    //^ keyword
        for x in [ 1 .. 10 ] do
        where (x % 2 = 0)
        //^ keyword
        sortByDescending x
        //^ keyword
        select (x * x)
        //^ keyword
        distinct
        //^ keyword
    }

let joined =
    query {
        for c in customers do
        join o in orders on (c.Id = o.CustomerId)
        //^ keyword
        //     ^ keyword
        //                ^ keyword
        groupBy c.City into g
        //^ keyword
        //             ^ keyword
        select g.Key
        //^ keyword
    }

let grouped =
    query {
        for c in customers do
        groupJoin o in orders on (c.Id = o.CustomerId) into g
        //                                             ^ keyword
        select g
    }

let total = query { for x in xs do count }
//                                 ^ keyword

let paged = query { for x in xs do sortBy x; skip 2; take 5 }
//                                 ^ keyword
//                                           ^ keyword
//                                                   ^ keyword

let crossLast = query { for x in xs do for y in ys do last }
//                                                    ^ keyword

let firstFew =
    query {
        for x in xs do
        take 3
        //^ keyword
        select x
        //^ keyword
        headOrDefault
        //^ keyword
    }

let search xs x =
    printfn "searching"
    find x
//  ^ function.call

let lexer () =
    skip ()
//  ^ function.call
    take ()
//  ^ function.call

let viaModule xs =
    printfn "sorting"
    List.sortBy id xs
//       ^ function.call

let builders = seq { yield task { return 1 } }
//             ^ keyword
//                         ^ keyword
