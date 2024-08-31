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
                "fsharp/src/parser.c",
                "fsharp/src/scanner.c",
                "fsharp_signature/src/parser.c",
                "fsharp_signature/src/scanner.c",
            ],
            resources: [
                .copy("queries")
            ],
            publicHeadersPath: "bindings/swift",
            cSettings: [.headerSearchPath("fsharp/src")]
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

