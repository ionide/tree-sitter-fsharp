type A() =
    member inline _.B<^Value
                                      when ^Value :> ValueType
                                      and ^Value : struct
                                      and ^Value : (new : unit -> ^Value)
                                      and ^Value : (static member ( + ) : ^Value * ^Value -> ^Value)
                                      and  ^Value : (static member Zero : ^Value)
                                      and default ^Value : int>
                  () = ()
