let x =
  match  e with
  | A -> ()
  | _ ->
      match x with
      | [] -> B
      | _ -> C
