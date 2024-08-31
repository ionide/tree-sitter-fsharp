{
  "targets": [
    {
      "target_name": "tree_sitter_fsharp_binding",
      "dependencies": [
        "<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except",
      ],
      "include_dirs": [
        "fsharp/src",
      ],
      "sources": [
        "bindings/node/binding.cc",
        "fsharp/src/parser.c",
        "fsharp/src/scanner.c",
        "fsharp_signature/src/parser.c",
        "fsharp_signature/src/scanner.c"
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
