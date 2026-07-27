; Tags for the fsharp_signature grammar (.fsi files). Derived from
; queries/tags.scm; the expression layer does not exist here, so value/function
; definitions are tagged via the signature-specific `value_definition` rule.

(named_module
  name: (_) @name) @definition.module

(module_defn
  . (_) @name) @definition.module

(namespace
  name: (_) @name) @definition.module

(type_definition
  [
    (delegate_type_defn
      (type_name
        type_name: (_) @name))
    (record_type_defn
      (type_name
        type_name: (_) @name))
    (union_type_defn
      (type_name
        type_name: (_) @name))
    (interface_type_defn
      (type_name
        type_name: (_) @name))
    (anon_type_defn
      (type_name
        type_name: (_) @name))
    (enum_type_defn
      (type_name
        type_name: (_) @name))
    (type_abbrev_defn
      (type_name
        type_name: (_) @name))
    (type_extension
      (type_name
        type_name: (_) @name))
    (type_declaration
      (type_name
        type_name: (_) @name))
  ]) @definition.class

(exception_definition
  exception_name: (_) @name) @definition.class

(value_definition
  (value_declaration_left
    . (identifier_pattern
        (long_identifier_or_op
          (_) @name)))) @definition.function

(member_defn
  (method_or_prop_defn
    name: (property_or_ident
      method: (identifier) @name))) @definition.method

; Bare members: the name field holds a property_or_ident whose only child is
; the identifier.
(member_defn
  (method_or_prop_defn
    name: (property_or_ident
      . (identifier) @name .))) @definition.method

(member_defn
  (member_signature
    (identifier) @name)) @definition.method
