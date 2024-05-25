;; ----------------------------------------------------------------------------
;; Literals and comments

[
  (line_comment)
  (block_comment)
] @comment @spell

(xml_doc) @comment.documentation @spell

((const) @constant
 (#set! "priority" 90))

((identifier) @variable
 (#set! "priority" 90))

(primary_constr_args (_) @variable.parameter)

((identifier_pattern (long_identifier (identifier) @character.special))
 (#match? @character.special "^\_.*")
 (#set! "priority" 90))

;; ----------------------------------------------------------------------------
;; Punctuation

(wildcard_pattern) @character.special

(type_name type_name: (_) @type.definition)

(type) @type

(member_signature
  .
  (identifier) @function.member
  (curried_spec
    (arguments_spec
      "*"* @operator
      (argument_spec
        (argument_name_spec
          "?"? @character.special
          name: (_) @variable.parameter)
        (_) @type))))

[
  (union_type_case)
] @type

(rules
  (rule
    [
      (type_check_pattern (_) @type)
      (identifier_pattern (long_identifier) . (long_identifier) @variable)
      (_ (identifier_pattern (long_identifier) . (long_identifier) @variable))
      (_ (_ (identifier_pattern (long_identifier) . (long_identifier) @variable)))
      (_ (_ (_ (identifier_pattern (long_identifier) . (long_identifier) @variable))))
      (_ (_ (_ (_ (identifier_pattern (long_identifier) . (long_identifier) @variable)))))
      (_ (_ (_ (_ (_ (identifier_pattern (long_identifier) . (long_identifier) @variable))))))
      (_ (_ (_ (_ (_ (_ (identifier_pattern (long_identifier) . (long_identifier) @variable)))))))
      (_ (_ (_ (_ (_ (_ (_ (identifier_pattern (long_identifier) . (long_identifier) @variable))))))))
    ]))

(fsi_directive_decl . (string) @module)

(import_decl . (_) @module)
(named_module
  name: (_) @module)
(namespace
  name: (_) @module)
(module_defn
  .
  (_) @module)

(ce_expression
  .
  (_) @constant.macro)

(field_initializer
  field: (_) @property)

(record_fields
  (record_field
    .
    (identifier) @property))

(dot_expression
  base: (_) @variable
  field: (_) @variable.member)

(value_declaration_left . (_) @variable)

(function_declaration_left
  . (_) @function
  [
    (argument_patterns)
    (argument_patterns (long_identifier (identifier)))
  ] @variable.parameter)

(member_defn
  (method_or_prop_defn
    [
      (property_or_ident) @function
      (property_or_ident
        instance: (identifier) @variable.parameter.builtin
        method: (identifier) @function.method)
    ]
    args: (_)? @variable.parameter))

(application_expression
  .
  [
    (_) @function.call
    (long_identifier_or_op (long_identifier (identifier) (identifier) @function.call))
    (typed_expression . (long_identifier_or_op (long_identifier (identifier)* . (identifier) @function.call)))
  ]
  .
  (_)? @variable)

(application_expression
  .
  [
    (dot_expression base: (_) @variable.member field: (_) @function.call)
    (_ (dot_expression base: (_) @variable.member field: (_) @function.call))
    (_ (_ (dot_expression base: (_) @variable.member field: (_) @function.call)))
    (_ (_ (_ (dot_expression base: (_) @variable.member field: (_) @function.call))))
    (_ (_ (_ (_ (dot_expression base: (_) @variable.member field: (_) @function.call)))))
    (_ (_ (_ (_ (_ (dot_expression base: (_) @variable.member field: (_) @function.call))))))
  ])

((infix_expression
  .
  (_)
  .
  (infix_op) @operator
  .
  (_) @function.call
  )
 (#eq? @operator "|>")
 )

((infix_expression
  .
  (_) @function.call
  .
  (infix_op) @operator
  .
  (_)
  )
 (#eq? @operator "<|")
 )

(typecast_expression
  .
  (_) @variable
  .
  (_) @type)

[
  (int)
  (int16)
  (uint16)
  (int32)
  (uint32)
  (int64)
  (uint64)
  (nativeint)
  (unativeint)
] @number

[
  (ieee32)
  (ieee64)
  (float)
  (decimal)
] @number.float

(bool) @boolean

([
  (string)
  (triple_quoted_string)
  (verbatim_string)
  (char)
] @spell @string)

(compiler_directive_decl) @keyword.directive

(attribute) @attribute

[
  "("
  ")"
  "{"
  "}"
  "["
  "]"
  "[|"
  "|]"
  "{|"
  "|}"
  "[<"
  ">]"
] @punctuation.bracket

(format_string_eval
  [
    "{"
    "}"
  ] @punctuation.special)

[
  ","
  ";"
] @punctuation.delimiter

[
  "|"
  "="
  ">"
  "<"
  "-"
  "~"
  "->"
  "&&"
  "||"
  (infix_op)
  (prefix_op)
] @operator

[
  "if"
  "then"
  "else"
  "elif"
  "when"
  "match"
  "match!"
  "then"
] @keyword.conditional

[
  "and"
  "or"
  "not"
  "upcast"
  "downcast"
] @keyword.operator

[
  "return"
  "return!"
  "yield"
  "yield!"
] @keyword.return

[
  "for"
  "while"
  "downto"
  "to"
] @keyword.repeat


[
  "open"
  "#r"
  "#load"
] @keyword.import

[
  "abstract"
  "delegate"
  "static"
  "inline"
  "mutable"
  "override"
  "rec"
  "global"
  (access_modifier)
] @keyword.modifier

[
  "let"
  "let!"
  "use"
  "use!"
  "member"
] @keyword.function

[
  "enum"
  "type"
  "inherit"
  "interface"
] @keyword.type

[
  "try"
  "with"
  "finally"
] @keyword.exception

[
  "as"
  "assert"
  "begin"
  "end"
  "done"
  "default"
  "in"
  "do"
  "do!"
  "event"
  "field"
  "fun"
  "function"
  "get"
  "set"
  "lazy"
  "new"
  "null"
  "of"
  "param"
  "property"
  "struct"
  "val"
  "module"
  "namespace"
] @keyword

(long_identifier
  ((identifier)* @module)
  .
  ((identifier)))

((long_identifier
  (identifier)*
  .
  (identifier) @property)
 (#set! "priority" 91))

((type
  (long_identifier (identifier) @type.builtin))
 (#any-of? @type.builtin "bool" "byte" "sbyte" "int16" "uint16" "int" "uint" "int64" "uint64" "nativeint" "unativeint" "decimal" "float" "double" "float32" "single" "char" "string" "unit"))

(const
  (unit) @constant)

(preproc_if
  [
    "#if" @keyword.directive
    "#endif" @keyword.directive
  ]
  condition: (_)? @keyword.directive)

(preproc_else
  "#else" @keyword.directive)

(long_identifier_or_op
  (op_identifier) @operator)

((identifier) @module.builtin
 (#any-of? @module.builtin "Array" "Async" "Directory" "File" "List" "Option" "Path" "Map" "Set" "Lazy" "Seq" "Task" "String" ))
