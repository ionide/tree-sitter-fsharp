let f =
    if b then
        ()
    else
        ()

#if REDUCED_ALLOCATIONS_BUT_RUNS_SLOWER
    // One allocation for While async context function
    1
#endif
