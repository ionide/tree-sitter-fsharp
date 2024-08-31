const root = require("path").join(__dirname, "..", "..");

module.exports = require("node-gyp-build")(root);

try {
  module.exports.fsharp.nodeTypeInfo = require("../../grammars/fsharp/src/node-types.json");
  module.exports.signature.nodeTypeInfo = require("../../grammars/signature/src/node-types.json");
} catch (_) {}
