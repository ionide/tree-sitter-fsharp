import XCTest
import SwiftTreeSitter
import TreeSitterFsharp

final class TreeSitterFsharpTests: XCTestCase {
    func testCanLoadGrammar() throws {
        let parser = Parser()
        let language = Language(language: tree_sitter_fsharp())
        XCTAssertNoThrow(try parser.setLanguage(language),
                         "Error loading Fsharp grammar")
    }

    func testCanLoadSignatureGrammar() throws {
        let parser = Parser()
        let language = Language(language: tree_sitter_fsharp_signature())
        XCTAssertNoThrow(try parser.setLanguage(language),
                         "Error loading Fsharp signature grammar")
    }
}
