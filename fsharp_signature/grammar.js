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
    [$._type, $._argument_type],
    // `a * b` in a member signature: an argument group if `->` follows, the
    // returned tuple otherwise.
    [$._curried_tuple_element, $.argument_spec],
    [$._type, $._curried_return_type],
    [$.union_type_cases],
  ],

  supertypes: ($) => [$._module_signature_elements],

  rules: {
    file: ($, _original) =>
      choice(
        $.named_module,
        repeat1($.namespace),
        repeat($._module_signature_elements),
      ),

    named_module: ($, _original) =>
      seq(
        optional($.attributes),
        "module",
        optional($.attributes),
        optional($.access_modifier),
        optional("rec"),
        field("name", $.long_identifier),
        repeat($._module_signature_elements),
      ),

    namespace: ($, _original) =>
      seq(
        "namespace",
        choice(
          "global",
          field("name", seq(optional("rec"), $.long_identifier)),
        ),
        repeat($._module_signature_elements),
      ),

    module_defn: ($, _original) =>
      prec.left(
        seq(
          optional($.attributes),
          "module",
          optional($.attributes),
          optional($.access_modifier),
          optional("rec"),
          $.identifier,
          "=",
          choice(
            scoped(repeat1($._module_signature_elements), $._indent, $._dedent),
            seq(
              "begin",
              scoped(repeat($._module_signature_elements), $._indent, $._dedent),
              "end",
            ),
          ),
        ),
      ),

    // What a `#if` branch holds at module level: signature elements, not the
    // declarations and expressions of an implementation file.
    _module_elem: ($, _original) => $._module_signature_elements,

    // A type in a signature file has no `let` or `do` bindings.
    _type_extension_inner: ($, _original) => repeat1($._type_defn_elements),

    // A member in a signature file declares a name and its signature; it has
    // no body: `member P: int`, `static member M: x: int -> T`,
    // `abstract M: unit -> unit`, `new: int -> T`, `val x: int`.
    member_defn: ($, _original) =>
      seq(
        optional($.attributes),
        choice(
          seq(
            optional($.access_modifier),
            choice(
              seq(optional("static"), "member"),
              seq(optional("static"), "abstract", optional("member")),
              "override",
              "default",
            ),
            optional("inline"),
            optional($.access_modifier),
            $.member_signature,
          ),
          seq(optional($.access_modifier), "new", ":", $.curried_spec),
          seq(
            optional("static"),
            "val",
            optional("mutable"),
            optional($.access_modifier),
            $.identifier,
            ":",
            $._type,
          ),
        ),
      ),

    member_signature: ($, _original) =>
      prec.left(
        seq(
          $._identifier_or_op,
          optional($.type_arguments),
          ":",
          $.curried_spec,
          optional($.type_argument_constraints),
          // A literal value: `member Zero: int = 0`.
          optional(seq("=", $._expression_block)),
          optional(
            choice(
              seq("with", "get"),
              seq("with", "set"),
              seq("with", "get", ",", "set"),
              seq("with", "set", ",", "get"),
            ),
          ),
        ),
      ),

    // Right-associative: after a member's `when` clause an `and` continues
    // the constraints, not the `type ... and ...` group.
    type_argument_constraints: ($, _original) =>
      prec.right(seq("when", $.constraint, repeat(seq("and", $.constraint)))),

    _module_signature_elements: ($, _original) =>
      choice(
        $.value_definition,
        $.type_definition,
        $.exception_definition,
        $.import_decl,
        $.module_defn,
        $.module_abbrev,
        $.preproc_if,
      ),

    value_definition: ($, _original) =>
      prec.left(
        4,
        seq(
          optional($.attributes),
          "val",
          $.value_declaration_left,
          ":",
          $.curried_spec,
          optional($.type_argument_constraints),
          optional(seq("=", field("body", $._expression_block))),
        ),
      ),

    _expression_block: ($, _original) =>
      choice(seq($._indent, $._expression, $._dedent), $._expression),

    // Expressions appear only as literal values and attribute arguments:
    // `[<AttributeUsage(AttributeTargets.Class ||| AttributeTargets.Struct,
    // AllowMultiple = false)>]`, `[<A(typeof<int>)>]`.
    _expression: ($, _original) =>
      choice(
        "null",
        $.const,
        $.long_identifier_or_op,
        $.paren_expression,
        $.infix_expression,
        $.typed_expression,
      ),

    paren_expression: ($, _original) =>
      prec(1, seq("(", choice($.tuple_expression, $._expression), ")")),

    tuple_expression: ($, _original) =>
      prec.left(seq($._expression, ",", $._expression, repeat(seq(",", $._expression)))),
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
