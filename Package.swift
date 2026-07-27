// swift-tools-version:5.3

import PackageDescription

let package = Package(
    name: "TreeSitterFsharp",
    products: [
        .library(name: "TreeSitterFsharp", targets: ["TreeSitterFsharp"]),
    ],
    dependencies: [
        .package(name: "SwiftTreeSitter", url: "https://github.com/tree-sitter/swift-tree-sitter", from: "0.9.0"),
    ],
    targets: [
        .target(
            name: "TreeSitterFsharp",
            dependencies: [],
            path: ".",
            // Both grammars compile into one target; each file is its own
            // translation unit, so the two scanners' static helpers do not
            // collide. tree_sitter_fsharp() and tree_sitter_fsharp_signature()
            // are declared in bindings/swift/TreeSitterFsharp/fsharp.h.
            sources: [
                "fsharp/src/parser.c",
                "fsharp/src/scanner.c",
                "fsharp_signature/src/parser.c",
                "fsharp_signature/src/scanner.c",
            ],
            resources: [
                .copy("queries")
            ],
            publicHeadersPath: "bindings/swift",
            cSettings: [
                .headerSearchPath("fsharp/src"),
                .headerSearchPath("fsharp_signature/src"),
            ]
        ),
        .testTarget(
            name: "TreeSitterFsharpTests",
            dependencies: [
                "SwiftTreeSitter",
                "TreeSitterFsharp",
            ],
            path: "bindings/swift/TreeSitterFsharpTests"
        )
    ],
    cLanguageStandard: .c11
)
