const assert = require("node:assert/strict");
const { describe, it } = require("node:test");

const Parser = require("tree-sitter");
const { fsharp: FSharp, signature: FSharpSignature } = require("../..");

describe("FSharp language", () => {
  const parser = new Parser();
  parser.setLanguage(FSharp);

  it("should be named fsharp", () => {
    assert.strictEqual(parser.getLanguage().name, "fsharp");
  });

  it("should parse sourcecode", () => {
    const sourceCode = `
    module M =
      let x = 0
    `;
    const tree = parser.parse(sourceCode);
    assert(!tree.rootNode.hasError);
  });
});

describe("FSharpSignature language", () => {
  const parser = new Parser();
  parser.setLanguage(FSharpSignature);

  it("should be named fsharp_signature", () => {
    assert.strictEqual(parser.getLanguage().name, "fsharp_signature");
  });

  it("should parse sourcecode", () => {
    const sourceCode = `
    module M =
      val x : int -> int
    `;
    const tree = parser.parse(sourceCode);
    assert(!tree.rootNode.hasError);
  });
});
