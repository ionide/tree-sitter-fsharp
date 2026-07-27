const path = require("path");

const root = path.join(__dirname, "..", "..");
const binding =
  typeof process.versions.bun === "string"
    // Support `bun build --compile` by being statically analyzable enough to find the .node file at build-time
    ? require(`../../prebuilds/${process.platform}-${process.arch}/tree-sitter-fsharp.node`)
    : require("node-gyp-build")(root);

try {
  binding.fsharp.nodeTypeInfo = require(path.join(
    root,
    "fsharp",
    "src",
    "node-types.json",
  ));
} catch (_) {}

try {
  binding.signature.nodeTypeInfo = require(path.join(
    root,
    "fsharp_signature",
    "src",
    "node-types.json",
  ));
} catch (_) {}

module.exports = binding;
