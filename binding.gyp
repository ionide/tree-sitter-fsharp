{
  "targets": [
    {
      "target_name": "tree_sitter_fsharp_binding",
      "dependencies": [
        "<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except",
      ],
      "include_dirs": [
        "grammars/fsharp/src",
      ],
      "sources": [
        "bindings/node/binding.cc",
        "grammars/fsharp/src/parser.c",
        "grammars/fsharp/src/scanner.c",
        "grammars/signature/src/parser.c",
        "grammars/signature/src/scanner.c"
      ],
      "conditions": [
        ["OS!='win'", {
          "cflags_c": [
            "-std=c11",
          ],
        }, { # OS == "win"
          "cflags_c": [
            "/std:c11",
            "/utf-8",
          ],
        }],
      ],
    },
  ]
}
