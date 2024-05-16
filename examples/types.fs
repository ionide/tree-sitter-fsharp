type MyClass() =
#if A
  member _.F(x: int) = x + 1
#endif
