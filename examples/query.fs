let f =
    match immutQuery with
    | CallJoin([ F(A, B)
                 G([A
                    B; C], C)]) -> ()

