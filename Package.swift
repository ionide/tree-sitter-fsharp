// swift-tools-version:5.3
import PackageDescription

let package = Package(
    name: "TreeSitterFSharp",
    products: [
        .library(name: "TreeSitterFSharp", targets: ["TreeSitterFSharp"]),
    ],
    dependencies: [
        .package(url: "https://github.com/ChimeHQ/SwiftTreeSitter", from: "0.8.0"),
    ],
    targets: [
        .target(
            name: "TreeSitterFSharp",
            dependencies: [],
            path: ".",
            sources: [
                "grammars/fsharp/src/parser.c",
                "grammars/fsharp/src/scanner.c",
                "grammars/signature/src/parser.c",
                "grammars/signature/src/scanner.c",
            ],
            resources: [
                .copy("queries")
            ],
            publicHeadersPath: "bindings/swift",
            cSettings: [.headerSearchPath("grammars/fsharp/src")]
        ),
        .testTarget(
            name: "TreeSitterFSharpTests",
            dependencies: [
                "SwiftTreeSitter",
                "TreeSitterFSharp",
            ],
            path: "bindings/swift/TreeSitterFSharpTests"
        )
    ],
    cLanguageStandard: .c11
)

