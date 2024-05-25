type A() =
  member _.B(x: 'T when 'T: equality) = ()
