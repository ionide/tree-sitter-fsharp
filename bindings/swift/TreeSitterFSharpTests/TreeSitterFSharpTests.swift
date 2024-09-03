import XCTest
import SwiftTreeSitter
import TreeSitterFSharp

final class TreeSitterFSharpTests: XCTestCase {

    func testFSharp() throws {
        let language = Language(language: tree_sitter_fsharp())

        let parser = Parser()
        try parser.setLanguage(language)

        let source = "module M = ()"

        let tree = try XCTUnwrap(parser.parse(source))
        let root = try XCTUnwrap(tree.rootNode)

        XCTAssertFalse(root.hasError)
    }

    func testFSharpSignature() throws {
        let language = Language(language: tree_sitter_fsharp_signature())

        let parser = Parser()
        try parser.setLanguage(language)

        let source = "val x : int -> int"

        let tree = try XCTUnwrap(parser.parse(source))
        let root = try XCTUnwrap(tree.rootNode)

        XCTAssertFalse(root.hasError)
    }

}
