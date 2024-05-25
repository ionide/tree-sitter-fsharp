module A

let f cts =
  CommandDefinition(
    """
      test string
      """,
    {| A = 1 |},
    cancellationToken = cts
  )
