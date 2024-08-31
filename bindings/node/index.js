const root = require("path").join(__dirname, "..", "..");

module.exports = require("node-gyp-build")(root);

try {
  module.exports.fsharp.nodeTypeInfo = require("../../fsharp/src/node-types.json");
  module.exports.signature.nodeTypeInfo = require("../../fsharp_signature/src/node-types.json");
} catch (_) {}
