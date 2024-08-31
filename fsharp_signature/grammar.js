/**
 * @file FSharp signature grammar for tree-sitter
 * @author Nikolaj Sidorenco
 * @license MIT
 * @see {@link https://fsharp.org/specs/language-spec/4.1/FSharpSpec-4.1-latest.pdf f# grammar}
 */

/* eslint-disable arrow-parens */
/* eslint-disable camelcase */
/* eslint-disable-next-line spaced-comment */
/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar(require("../fsharp/grammar"), {
  name: "fsharp_signature",

  conflicts: ($) => [
    [$.long_identifier, $._identifier_or_op],
    [$.type_argument, $.simple_type],
    [$.rules],
  ],

  supertypes: ($) => [$._module_signature_elements],

  rules: {
    file: ($, _original) =>
      choice($.namespace, $.module, repeat($._module_signature_elements)),

    namespace: ($, _original) =>
      seq("namespace", $.long_identifier, repeat($._module_signature_elements)),

    module: ($, _original) =>
      seq(
        "module",
        $.long_identifier,
        "=",
        scoped(repeat($._module_signature_elements), $._indent, $._dedent),
      ),

    _module_signature_elements: ($, _original) =>
      choice(
        // $.value_signature,
        $.value_definition,
      ),

    // value_signature: ($, _original) =>
    //   seq("val", optional("mutable"), $.curried_spec),

    value_definition: ($, _original) =>
      prec.left(
        4,
        seq(
          optional($.attributes),
          "val",
          $.value_declaration_left,
          ":",
          $._type,
          optional(seq("=", field("body", $._expression_block))),
        ),
      ),
  },
});

/**
 *
 * @param {Rule} rule
 *
 * @param {Rule} indent
 *
 * @param {Rule} dedent
 *
 * @return {Rule}
 */
function scoped(rule, indent, dedent) {
  return field("block", seq(indent, rule, dedent));
}
