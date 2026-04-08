#ifndef TREE_SITTER_FSHARP_SCANNER_H_
#define TREE_SITTER_FSHARP_SCANNER_H_

#include "tree_sitter/alloc.h"
#include "tree_sitter/array.h"
#include "tree_sitter/parser.h"

enum TokenType {
  NEWLINE,
  INDENT,
  DEDENT,
  THEN,
  ELSE,
  ELIF,
  PREPROC_IF,
  PREPROC_ELSE,
  PREPROC_END,
  CLASS,
  BEGIN,
  STRUCT,
  INTERFACE,
  END,
  AND,
  WITH,
  TRIPLE_QUOTE_CONTENT,
  FORMAT_TRIPLE_QUOTE_CONTENT,
  BLOCK_COMMENT_CONTENT,
  INSIDE_STRING,
  NEWLINE_NO_ALIGNED,
  TUPLE_MARKER,
  QUOTED_CLOSE,
  UNTYPED_QUOTED_CLOSE,
  MULTI_DOLLAR_TRIPLE_QUOTE_START,
  MULTI_DOLLAR_TRIPLE_QUOTED_CONTENT,
  MULTI_DOLLAR_INTERP_START,
  MULTI_DOLLAR_INTERP_END,
  MULTI_DOLLAR_TRIPLE_QUOTE_END,
  TYAPP_OPEN,
  PAREN_INDENT,
  TYPE_DECL_NEWLINE,
  IN,
  ERROR_SENTINEL
};

typedef struct {
  Array(uint16_t) indents;
  Array(bool) paren_indents;
  Array(uint16_t) preprocessor_indents;
  uint8_t multi_dollar_count;
} Scanner;

static inline void advance(TSLexer *lexer) { lexer->advance(lexer, false); }

static inline void skip(TSLexer *lexer) { lexer->advance(lexer, true); }

static inline bool is_word_char(int32_t c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c == '_' || c == '\'';
}

static inline void push_indent(Scanner *scanner, uint16_t indent_length,
                               bool is_paren_indent) {
  array_push(&scanner->indents, indent_length);
  array_push(&scanner->paren_indents, is_paren_indent);
}

static inline void pop_indent(Scanner *scanner) {
  if (scanner->indents.size > 0) {
    array_pop(&scanner->indents);
  }
  if (scanner->paren_indents.size > 0) {
    array_pop(&scanner->paren_indents);
  }
}

static inline uint16_t peek_indent_length(Scanner *scanner) {
  return *array_back(&scanner->indents);
}

static inline bool peek_is_paren_indent(Scanner *scanner) {
  return scanner->paren_indents.size > 0 && *array_back(&scanner->paren_indents);
}

// Check if the content after '<' looks like type arguments per F# spec Section 15.3.
// Valid type argument content: identifiers, whitespace, and type-syntax characters
// like ',', '*', '->', '(', ')', '[', ']', '<', '>', '^', '#', ':', '{|', '|}'.
// Returns true if the '<' should be treated as a type application opener.
static inline bool is_type_application_open(TSLexer *lexer) {
  // We've already seen '<', now peek forward.
  // If immediately '>' this is '<>' (empty type args).
  // Track angle bracket depth to find the matching '>'.
  int angle_depth = 1;
  int paren_depth = 0;

  while (!lexer->eof(lexer) && angle_depth > 0) {
    int32_t c = lexer->lookahead;

    if (c == '\n' || c == '\r') {
      // Newline inside type args is not valid for type application lexical analysis
      return false;
    }

    // Valid type argument characters
    if (is_word_char(c) || c == ' ' || c == '\t' || c == ',' || c == '*' ||
        c == '.' || c == ':' || c == '#' || c == '^' || c == '/' || c == '|' ||
        c == '{' || c == '}' || c == '[' || c == ']') {
      advance(lexer);
      continue;
    }

    if (c == '(') {
      paren_depth++;
      advance(lexer);
      continue;
    }

    if (c == ')') {
      if (paren_depth <= 0) {
        // Unbalanced ')' - not type args (e.g., "(l<r)")
        return false;
      }
      paren_depth--;
      advance(lexer);
      continue;
    }

    if (c == '<') {
      angle_depth++;
      advance(lexer);
      continue;
    }

    if (c == '>') {
      angle_depth--;
      if (angle_depth == 0) {
        // Found matching '>' - this is a type application
        return true;
      }
      advance(lexer);
      continue;
    }

    if (c == '-') {
      // '->' is valid in function types, but '-' alone followed by
      // a digit or other op char is not type syntax
      advance(lexer);
      if (lexer->lookahead == '>') {
        advance(lexer);
        continue;
      }
      // Bare '-' is not valid in type args
      return false;
    }

    // Any other character is not valid in type arguments
    // This catches: '&', '!', '=', '+', ';', '@', '$', '%', '?', etc.
    return false;
  }

  return false;
}

static inline bool scan_n_chars(TSLexer *lexer, char ch, uint8_t count) {
  lexer->mark_end(lexer);
  for (uint8_t i = 0; i < count; i++) {
    if (lexer->lookahead != ch) {
      return false;
    }
    advance(lexer);
  }
  lexer->mark_end(lexer);
  return true;
}

static inline bool scan_block_comment(TSLexer *lexer) {
  lexer->mark_end(lexer);
  if (lexer->lookahead != '(')
    return false;

  advance(lexer);
  if (lexer->lookahead != '*')
    return false;

  advance(lexer);

  while (true) {
    switch (lexer->lookahead) {
    case '(':
      scan_block_comment(lexer);
      break;
    case '*':
      advance(lexer);
      if (lexer->lookahead == ')') {
        advance(lexer);
        return true;
      }
      break;
    case '\0':
      return true;
    default:
      advance(lexer);
    }
  }
}

static inline bool is_infix_op_start(TSLexer *lexer) {
  switch (lexer->lookahead) {
  case '+':
  case '-':
    skip(lexer);
    return !(lexer->lookahead >= '0' && lexer->lookahead <= '9');
  case '*':
  case '%':
  case '&':
  case '=':
  case '?':
  case '<':
  case '>':
  case '^':
    return true;
  case '/':
    skip(lexer);
    return lexer->lookahead != '/';
  case '.':
    skip(lexer);
    return lexer->lookahead != '.';
  case '!':
    skip(lexer);
    return lexer->lookahead == '=';
  case ':':
    skip(lexer);
    return lexer->lookahead == '=' || lexer->lookahead == ':' ||
           lexer->lookahead == '?' || lexer->lookahead == ' ' ||
           lexer->lookahead == '>';
  case 'o':
    skip(lexer);
    return lexer->lookahead == 'r';
  case '@':
  case '$':
    skip(lexer);
    return lexer->lookahead != '"';
  default:
    return false;
  }
}

static inline bool is_bracket_end(TSLexer *lexer) {
  switch (lexer->lookahead) {
  case ')':
  case ']':
  case '}':
    return true;
  default:
    return false;
  }
  }

  // Match remaining characters of a keyword after the first character.
  // Returns true if the rest of the keyword matches and is followed by a non-word char.
  static inline bool match_keyword_rest(TSLexer *lexer, const char *rest) {
    for (; *rest; rest++) {
      if (lexer->lookahead != *rest) return false;
      advance(lexer);
    }
    return !is_word_char(lexer->lookahead);
  }

  // Table-driven block opener matching for class/begin/struct/interface.
  typedef struct {
    char first_char;
    const char *rest;
    enum TokenType token;
  } BlockOpener;

  static const BlockOpener block_openers[] = {
    {'c', "lass",      CLASS},
    {'b', "egin",      BEGIN},
    {'s', "truct",     STRUCT},
    {'i', "nterface",  INTERFACE},
  };

  // Check if a preprocessor directive requires emitting DEDENT before itself.
  // Used by both #endif and #else handlers to pop indent when the preprocessor
  // indent is less than the current block indent.
  static inline bool try_dedent_for_preproc(Scanner *scanner, TSLexer *lexer) {
    if (scanner->indents.size > 0 &&
        scanner->preprocessor_indents.size > 0) {
      uint16_t current_indent_length = peek_indent_length(scanner);
      uint16_t current_preproc_length =
          *array_back(&scanner->preprocessor_indents);
      if (current_preproc_length < current_indent_length) {
        pop_indent(scanner);
        lexer->result_symbol = DEDENT;
        return true;
      }
    }
    return false;
  }

static bool scan(Scanner *scanner, TSLexer *lexer, const bool *valid_symbols) {
  if (valid_symbols[ERROR_SENTINEL]) {
    // During error recovery, all valid_symbols are true. Tree-sitter's error
    // recovery mechanism cannot emit external scanner tokens, so we must still
    // produce tokens like DEDENT and PREPROC_END when we can identify them.
    // This enables partial parse tree recovery -- e.g., "match x with" needs
    // DEDENT to be recognized as a partially correct match-statement for
    // syntax highlighting purposes.

    // At EOF, emit DEDENT to drain the indent stack. This is critical for
    // closing partial parse trees at end of input.
    if (lexer->eof(lexer) && scanner->indents.size > 1) {
      pop_indent(scanner);
      lexer->result_symbol = DEDENT;
      return true;
    }

    // For non-EOF cases, fall through to normal scanning logic below.
    // The normal path handles whitespace consumption and emits DEDENT/NEWLINE
    // based on actual indentation levels. Features that should not run during
    // error recovery (multi-dollar strings, quotation closers, etc.) are
    // already guarded by !valid_symbols[ERROR_SENTINEL] checks.
  }

  if (valid_symbols[INSIDE_STRING] && !valid_symbols[ERROR_SENTINEL]) {
    return false;
  }

  // Type application '<' disambiguation (F# spec Section 15.3).
  // When the grammar expects TYAPP_OPEN (i.e., a '<' immediately after an expression),
  // peek ahead to determine if the content between '<' and '>' looks like type arguments.
  // If not (e.g., it's a comparison operator like l<r), return false so the grammar
  // falls through to infix_op.
  if (valid_symbols[TYAPP_OPEN] && !valid_symbols[ERROR_SENTINEL] &&
      lexer->lookahead == '<') {
    lexer->mark_end(lexer);
    advance(lexer);
    // Mark end right after '<' - this is what we want the token to contain
    lexer->mark_end(lexer);
    // Now peek ahead (advancing further) to check if content looks like type args.
    // Even though we advance past the type content, mark_end is already set to
    // just after '<', so the emitted token will be exactly '<'.
    if (is_type_application_open(lexer)) {
      lexer->result_symbol = TYAPP_OPEN;
      return true;
    }
    // Not a type application - don't consume the '<', let grammar handle it as infix_op.
    // But we already advanced past '<' and potentially more. That's OK because
    // we return false and tree-sitter will reset the lexer position.
    return false;
  }

  if (!valid_symbols[ERROR_SENTINEL] && scanner->multi_dollar_count > 1) {
    if (valid_symbols[MULTI_DOLLAR_INTERP_START] && lexer->lookahead == '{') {
      if (scan_n_chars(lexer, '{', scanner->multi_dollar_count)) {
        lexer->result_symbol = MULTI_DOLLAR_INTERP_START;
        return true;
      }
    }

    if (valid_symbols[MULTI_DOLLAR_INTERP_END] && lexer->lookahead == '}') {
      if (scan_n_chars(lexer, '}', scanner->multi_dollar_count)) {
        lexer->result_symbol = MULTI_DOLLAR_INTERP_END;
        return true;
      }
    }

    if (valid_symbols[MULTI_DOLLAR_TRIPLE_QUOTE_END] && lexer->lookahead == '"') {
      if (scan_n_chars(lexer, '"', 3)) {
        scanner->multi_dollar_count = 0;
        lexer->result_symbol = MULTI_DOLLAR_TRIPLE_QUOTE_END;
        return true;
      }
    }
  }

  if (!valid_symbols[ERROR_SENTINEL] &&
      (valid_symbols[TRIPLE_QUOTE_CONTENT] || valid_symbols[FORMAT_TRIPLE_QUOTE_CONTENT] ||
       valid_symbols[MULTI_DOLLAR_TRIPLE_QUOTED_CONTENT])) {
    bool is_format = valid_symbols[FORMAT_TRIPLE_QUOTE_CONTENT];
    bool is_multi_dollar = valid_symbols[MULTI_DOLLAR_TRIPLE_QUOTED_CONTENT];
    bool has_content = false;
    lexer->mark_end(lexer);
    while (true) {
      if (lexer->lookahead == '\0') {
        break;
      }
      if ((is_format || is_multi_dollar) && lexer->lookahead == '{') {
        // In format triple-quoted strings, stop at '{' to allow interpolation.
        // Multi-dollar interpolated strings require N braces, where N is the
        // number of leading '$' characters.
        uint8_t brace_count = is_multi_dollar ? scanner->multi_dollar_count : 1;
        lexer->mark_end(lexer);

        if (!is_multi_dollar) {
          advance(lexer);
          if (lexer->lookahead == '{') {
            advance(lexer);
            lexer->mark_end(lexer);
            has_content = true;
            continue;
          }
          if (!has_content) {
            return false;
          }
          break;
        }

        bool matches_interp_start = true;
        for (uint8_t i = 0; i < brace_count; i++) {
          if (lexer->lookahead != '{') {
            matches_interp_start = false;
            break;
          }
          advance(lexer);
        }
        if (matches_interp_start) {
          if (!has_content) {
            return false;
          }
          break;
        }
        has_content = true;
        lexer->mark_end(lexer);
        continue;
      }
      if (lexer->lookahead != '"') {
        advance(lexer);
        has_content = true;
      } else {
        if (is_multi_dollar) {
          advance(lexer);
          if (lexer->lookahead == '"') {
            advance(lexer);
            if (lexer->lookahead == '"') {
              break;
            }
          }
          has_content = true;
          lexer->mark_end(lexer);
        } else {
          lexer->mark_end(lexer);
          skip(lexer);
          if (lexer->lookahead == '"') {
            skip(lexer);
            if (lexer->lookahead == '"') {
              skip(lexer);
              break;
            }
          }
        }
        has_content = true;
        lexer->mark_end(lexer);
      }
    }
    if (is_multi_dollar) {
      lexer->result_symbol = MULTI_DOLLAR_TRIPLE_QUOTED_CONTENT;
    } else {
      lexer->result_symbol = is_format ? FORMAT_TRIPLE_QUOTE_CONTENT : TRIPLE_QUOTE_CONTENT;
    }
    return true;
  }

  if (valid_symbols[TYPE_DECL_NEWLINE]) {
    // Only fire at EOF or newline; if the current character is something else
    // (e.g. '=' during GLR exploration), fall through to general scanning —
    // the lexer position is unchanged so this is safe.
    if (lexer->eof(lexer)) {
      lexer->result_symbol = TYPE_DECL_NEWLINE;
      return true;
    }
    if (lexer->lookahead == '\n' || lexer->lookahead == '\r') {
      // Peek ahead: skip newlines/whitespace to find indentation of next content.
      // If next content is NOT more indented than current scope, this is a bare
      // type declaration (e.g. [<Measure>] type Dollars).
      // If next content IS more indented, the type has a body (e.g. type CsvFile
      //   private (...) = ...) and TYPE_DECL_NEWLINE should not fire.
      uint32_t next_indent = 0;
      for (;;) {
        if (lexer->lookahead == '\n' || lexer->lookahead == '\r') {
          next_indent = 0;
          skip(lexer);
        } else if (lexer->lookahead == ' ') {
          next_indent++;
          skip(lexer);
        } else if (lexer->lookahead == '\t') {
          next_indent += 8;
          skip(lexer);
        } else {
          break;
        }
      }
      if (lexer->eof(lexer)) {
        lexer->result_symbol = TYPE_DECL_NEWLINE;
        return true;
      }
      uint32_t scope_indent = (scanner->indents.size > 0)
        ? (uint32_t)*array_back(&scanner->indents)
        : 0;
      if (next_indent <= scope_indent) {
        // Next line is at same or lower indentation — bare type declaration
        lexer->result_symbol = TYPE_DECL_NEWLINE;
        return true;
      }
      // else: next line is more indented — type has a body, don't fire
      return false;
    }
  }

  lexer->mark_end(lexer);

  bool found_end_of_line = false;
  bool found_end_of_line_semi_colon = false;
  bool found_start_of_infix_op = false;
  bool found_same_line_pipe_infix = false;
  bool found_bracket_end = false;
  bool found_preprocessor_end = false;
  bool found_preproc_if = false;
  bool found_comment_start = false;
  uint32_t indent_length = lexer->get_column(lexer);

  for (;;) {
    if (lexer->lookahead == '\n') {
      found_end_of_line = true;
      indent_length = 0;
      skip(lexer);
    } else if (lexer->lookahead == ' ') {
      indent_length++;
      skip(lexer);
    } else if (lexer->lookahead == '\r' || lexer->lookahead == '\f') {
      indent_length = 0;
      skip(lexer);
    } else if (lexer->lookahead == '\t') {
      indent_length += 8;
      skip(lexer);
    } else if (lexer->eof(lexer)) {
      found_end_of_line = true;
      indent_length = 0;
      break;
    } else if (lexer->lookahead == '/') {
      skip(lexer);
      if (!valid_symbols[INSIDE_STRING] && lexer->lookahead == '/') {
        if (!found_preproc_if) {
          return false;
        }
        while (lexer->lookahead != '\n' && !lexer->eof(lexer)) {
          skip(lexer);
        }
      } else {
        return false;
      }
    } else if (lexer->lookahead == '#') {
      advance(lexer);
      if (lexer->lookahead == 'e') {
        advance(lexer);
        if (lexer->lookahead == 'n') {
          advance(lexer);
          if (lexer->lookahead == 'd') {
            advance(lexer);
            if (lexer->lookahead == 'i') {
              advance(lexer);
              if (lexer->lookahead == 'f') {
                advance(lexer);
                found_preprocessor_end = true;
                if (try_dedent_for_preproc(scanner, lexer)) {
                  return true;
                }
                if (valid_symbols[PREPROC_END]) {
                  if (scanner->preprocessor_indents.size > 0) {
                    array_pop(&scanner->preprocessor_indents);
                  }
                  lexer->mark_end(lexer);
                  lexer->result_symbol = PREPROC_END;
                  return true;
                }
              }
            }
          }
        } else if (lexer->lookahead == 'l') {
          advance(lexer);
          if (lexer->lookahead == 's') {
            advance(lexer);
            if (lexer->lookahead == 'e') {
              advance(lexer);
              if (try_dedent_for_preproc(scanner, lexer)) {
                return true;
              }
              if (valid_symbols[PREPROC_ELSE]) {
                lexer->mark_end(lexer);
                lexer->result_symbol = PREPROC_ELSE;
                return true;
              }
            }
          }
        }
      } else if (lexer->lookahead == 'i') {
        advance(lexer);
        if (lexer->lookahead == 'f') {
          advance(lexer);
          found_preproc_if = true;
          if (valid_symbols[NEWLINE] || valid_symbols[INDENT]) {
            while (lexer->lookahead != '\n' && !lexer->eof(lexer)) {
              skip(lexer);
            }
          } else {
            if (scanner->indents.size > 0) {
              if (valid_symbols[PREPROC_IF]) {
                uint16_t current_indent_length = peek_indent_length(scanner);
                array_push(&scanner->preprocessor_indents,
                           current_indent_length);
              } else {
                pop_indent(scanner);
                lexer->result_symbol = DEDENT;
                return true;
              }
            } else {
              lexer->mark_end(lexer);
              lexer->result_symbol = PREPROC_IF;
              return true;
            }
          }
        }
      } else {
        if (found_end_of_line && valid_symbols[NEWLINE_NO_ALIGNED]) {
          lexer->result_symbol = NEWLINE_NO_ALIGNED;
          return true;
        }
        return false;
      }
    } else {
      break;
    }
  }

  // Handle @> and @@> as external tokens to prevent them from being
  // tokenized as infix operators inside quotation expressions.
  // If a dedent is pending for a same-line expression block (e.g. fun x -> x),
  // emit the dedent first and leave the quote closer for the next scan.
  bool failed_at_sign_match = false;
  if (!valid_symbols[ERROR_SENTINEL] && lexer->lookahead == '@') {
    lexer->mark_end(lexer);
    advance(lexer);
    if (lexer->lookahead == '@') {
      advance(lexer);
      if (lexer->lookahead == '>') {
        if (valid_symbols[DEDENT] && scanner->indents.size > 1) {
          pop_indent(scanner);
          lexer->result_symbol = DEDENT;
          return true;
        }
        advance(lexer);
        lexer->mark_end(lexer);
        lexer->result_symbol = UNTYPED_QUOTED_CLOSE;
        return true;
      }
      // @@ not followed by > -- this is an infix operator, not a quotation closer.
      // The lexer has advanced past both @ chars; flag so we skip the keyword chain
      // and treat this like an infix op.
      failed_at_sign_match = true;
      found_start_of_infix_op = true;
    } else if (lexer->lookahead == '>') {
      if (valid_symbols[DEDENT] && scanner->indents.size > 1) {
        pop_indent(scanner);
        lexer->result_symbol = DEDENT;
        return true;
      }
      advance(lexer);
      lexer->mark_end(lexer);
      lexer->result_symbol = QUOTED_CLOSE;
      return true;
    } else {
      // @ not followed by > or @ -- this is an infix operator, not a quotation closer.
      // The lexer has advanced past @; flag so we skip the keyword chain
      // and treat this like an infix op.
      failed_at_sign_match = true;
      found_start_of_infix_op = true;
    }
  }

  if (!valid_symbols[ERROR_SENTINEL] && lexer->lookahead == '$') {
    lexer->mark_end(lexer);

    if (valid_symbols[MULTI_DOLLAR_TRIPLE_QUOTE_START]) {
      uint8_t dollar_count = 0;
      while (lexer->lookahead == '$' && dollar_count < UINT8_MAX) {
        advance(lexer);
        dollar_count++;
      }
      if (dollar_count > 1 && scan_n_chars(lexer, '"', 3)) {
        scanner->multi_dollar_count = dollar_count;
        lexer->result_symbol = MULTI_DOLLAR_TRIPLE_QUOTE_START;
        return true;
      }
      // Not a multi-dollar string. Before bailing out, check if DEDENT or
      // NEWLINE should be emitted -- the '$' might be the start of an
      // interpolated string on a new, less-indented line.
      if (found_end_of_line && scanner->indents.size > 0) {
        uint16_t current_indent_length = peek_indent_length(scanner);
        if (valid_symbols[DEDENT] && indent_length < current_indent_length) {
          bool can_dedent_paren_indent = !peek_is_paren_indent(scanner) || indent_length == 0 || lexer->eof(lexer);
          if (can_dedent_paren_indent) {
            pop_indent(scanner);
            lexer->result_symbol = DEDENT;
            return true;
          }
        }
        if (valid_symbols[NEWLINE] && indent_length == current_indent_length && indent_length > 0) {
          lexer->result_symbol = NEWLINE;
          return true;
        }
      }
      return false;
    }
  }

  bool failed_block_opener = false;
  {
    for (size_t i = 0; i < sizeof(block_openers) / sizeof(block_openers[0]); i++) {
      const BlockOpener *op = &block_openers[i];
      if (valid_symbols[op->token] && lexer->lookahead == op->first_char) {
        lexer->mark_end(lexer);
        indent_length = lexer->get_column(lexer);
        advance(lexer);
        if (match_keyword_rest(lexer, op->rest)) {
          lexer->mark_end(lexer);
          lexer->result_symbol = op->token;
          return true;
        }
        failed_block_opener = true;
        break;
      }
    }
  }

  if (found_end_of_line && valid_symbols[NEWLINE_NO_ALIGNED] &&
      !found_start_of_infix_op && !found_preprocessor_end) {
    lexer->result_symbol = NEWLINE_NO_ALIGNED;
    return true;
  }

  if (!failed_block_opener && !failed_at_sign_match) {

  if (valid_symbols[NEWLINE] && lexer->lookahead == ';') {
    advance(lexer);
    while (lexer->lookahead == ' ' || lexer->lookahead == '\n') {
      advance(lexer);
    }
    found_end_of_line = true;
    found_end_of_line_semi_colon = true;
    lexer->mark_end(lexer);
  }

  if (lexer->lookahead == 't' &&
      (valid_symbols[THEN] || valid_symbols[DEDENT])) {
    advance(lexer);
    if (lexer->lookahead == 'h') {
      advance(lexer);
      if (lexer->lookahead == 'e') {
        advance(lexer);
        if (lexer->lookahead == 'n') {
          advance(lexer);
          if (!is_word_char(lexer->lookahead)) {
            // the 'THEN' token is only valid if we have popped the appropriate
            // amount of dedent tokens.
            // If 'THEN' is not valid we just continue to pop dedent tokens.
            if (valid_symbols[THEN]) {
              lexer->mark_end(lexer);
              lexer->result_symbol = THEN;
              return true;
            } else {
              pop_indent(scanner);
              lexer->result_symbol = DEDENT;
              return true;
            }
          }
        }
      }
    }
  } else if (lexer->lookahead == 'a' &&
             (valid_symbols[AND] || valid_symbols[DEDENT])) {
    advance(lexer);
    if (lexer->lookahead == 'n') {
      advance(lexer);
      if (lexer->lookahead == 'd') {
        advance(lexer);
        if (lexer->lookahead == ' ') {
          // the 'AND' token is only valid if we have popped the appropriate
          // amount of dedent tokens.
          // If 'AND' is not valid we just continue to pop dedent tokens.
          if (valid_symbols[AND]) {
            lexer->mark_end(lexer);
            lexer->result_symbol = AND;
            return true;
          } else {
            pop_indent(scanner);
            lexer->result_symbol = DEDENT;
            return true;
          }
        }
      }
    }
  } else if (lexer->lookahead == 'w' &&
             (valid_symbols[WITH] || valid_symbols[DEDENT])) {
    advance(lexer);
    if (lexer->lookahead == 'i') {
      advance(lexer);
      if (lexer->lookahead == 't') {
        advance(lexer);
        if (lexer->lookahead == 'h') {
          advance(lexer);
          if (lexer->lookahead == ' ') {
            // the 'WITH' token is only valid if we have popped the appropriate
            // amount of dedent tokens.
            // If 'WITH' is not valid we just continue to pop dedent tokens.
            if (valid_symbols[WITH]) {
              lexer->mark_end(lexer);
              lexer->result_symbol = WITH;
              return true;
            } else {
              pop_indent(scanner);
              lexer->result_symbol = DEDENT;
              return true;
            }
          }
        }
      }
    }
  } else if (lexer->lookahead == 'i' && valid_symbols[IN]) {
    advance(lexer);
    if (lexer->lookahead == 'n') {
      advance(lexer);
      if (!is_word_char(lexer->lookahead)) {
        // Produce the IN token to close an _expression_block_for_let.
        // Pop the indent that was pushed by the matching INDENT, since
        // _in replaces _dedent as the block terminator.
        pop_indent(scanner);
        lexer->mark_end(lexer);
        lexer->result_symbol = IN;
        return true;
      }
    }
  } else if (lexer->lookahead == 'e' &&
             (valid_symbols[ELSE] || valid_symbols[ELIF] ||
              valid_symbols[END] || valid_symbols[DEDENT])) {
    advance(lexer);
    int16_t token_indent_level = lexer->get_column(lexer);
    if (lexer->lookahead == 'l') {
      advance(lexer);
      if (lexer->lookahead == 's' &&
          (valid_symbols[ELSE] || valid_symbols[DEDENT])) {
        advance(lexer);
        if (lexer->lookahead == 'e') {
          advance(lexer);
          if (!is_word_char(lexer->lookahead)) {
            if (valid_symbols[ELSE]) {
              if (scanner->indents.size > 0 &&
                  token_indent_level < peek_indent_length(scanner)) {
                pop_indent(scanner);
                lexer->result_symbol = DEDENT;
                return true;
              } else {
                lexer->mark_end(lexer);
                for (;;) {
                  if (lexer->lookahead == ' ' || lexer->lookahead == '\n' ||
                      lexer->lookahead == '\r' || lexer->lookahead == '\t') {
                    advance(lexer);
                  } else {
                    break;
                  }
                }
                if (lexer->lookahead == 'i') {
                  advance(lexer);
                  if (lexer->lookahead == 'f') {
                    advance(lexer);
                    if (!is_word_char(lexer->lookahead)) {
                      lexer->mark_end(lexer);
                      lexer->result_symbol = ELIF;
                      return true;
                    }
                  }
                }
                lexer->result_symbol = ELSE;
                return true;
              }
            } else {
              pop_indent(scanner);
              lexer->result_symbol = DEDENT;
              return true;
            }
          }
        }
      } else if (lexer->lookahead == 'i' &&
                 (valid_symbols[ELIF] || valid_symbols[DEDENT])) {
        advance(lexer);
        if (lexer->lookahead == 'f') {
          advance(lexer);
          if (!is_word_char(lexer->lookahead)) {
            if (valid_symbols[ELIF]) {
              if (scanner->indents.size > 0 &&
                  token_indent_level < peek_indent_length(scanner)) {
                pop_indent(scanner);
                lexer->result_symbol = DEDENT;
                return true;
              } else {
                lexer->mark_end(lexer);
                lexer->result_symbol = ELIF;
                return true;
              }
            } else {
              pop_indent(scanner);
              lexer->result_symbol = DEDENT;
              return true;
            }
          }
        }
      }
    } else if (lexer->lookahead == 'n' &&
               (valid_symbols[END] || valid_symbols[DEDENT])) {
      advance(lexer);
      if (lexer->lookahead == 'd') {
        advance(lexer);
        if (!is_word_char(lexer->lookahead)) {
          if (valid_symbols[END]) {
            lexer->mark_end(lexer);
            lexer->result_symbol = END;
            return true;
          } else if (valid_symbols[DEDENT] && scanner->indents.size > 0) {
            pop_indent(scanner);
            lexer->result_symbol = DEDENT;
            return true;
          }
        }
      }
    }
  } else if (is_bracket_end(lexer)) {
    found_bracket_end = true;
  } else if (is_infix_op_start(lexer)) {
    found_start_of_infix_op = true;
  } else if (lexer->lookahead == '|') {
    skip(lexer);
    switch (lexer->lookahead) {
    case ']':
    case '}':
      found_bracket_end = true;
      break;
    case '>':
      found_start_of_infix_op = true;
      break;
    case ' ':
      if (!found_end_of_line) {
        found_start_of_infix_op = true;
        found_same_line_pipe_infix = true;
        break;
      }
      if (indent_length == 0) {
        indent_length = 1;
      }
      if (scanner->indents.size > 0) {
        uint16_t current_indent_length = peek_indent_length(scanner);
        if (found_end_of_line && indent_length == current_indent_length &&
            indent_length > 0 && !found_start_of_infix_op &&
            !found_bracket_end) {
          if (valid_symbols[NEWLINE] && !found_preprocessor_end) {
            lexer->result_symbol = NEWLINE;
            return true;
          }
        }
      }
      break;
    default:
      found_start_of_infix_op = true;
      break;
    }
  } else if (lexer->lookahead == '(') {
    skip(lexer);
    if (lexer->lookahead == '*') {
      found_comment_start = true;
    }
  }

  } // end of !failed_block_opener && !failed_at_sign_match

  if (valid_symbols[NEWLINE] && found_end_of_line_semi_colon &&
      !found_comment_start && !found_bracket_end) {
    lexer->result_symbol = NEWLINE;
    return true;
  }

  if (valid_symbols[INDENT] && !valid_symbols[ERROR_SENTINEL] &&
      !found_bracket_end && !found_preprocessor_end &&
      !found_same_line_pipe_infix) {
    push_indent(scanner, indent_length, false);
    lexer->result_symbol = INDENT;
    return true;
  }

  if (valid_symbols[PAREN_INDENT] && !valid_symbols[ERROR_SENTINEL] &&
      !found_bracket_end &&
      !found_preprocessor_end && !found_same_line_pipe_infix) {
    // Like INDENT, but tracked separately as a paren indent so DEDENT/NEWLINE
    // logic can be more lenient inside parenthesized expressions, where the
    // closing ')' determines scope rather than indentation alone.
    push_indent(scanner, indent_length, true);
    lexer->result_symbol = PAREN_INDENT;
    return true;
  }

  if (scanner->indents.size > 0) {
    bool is_paren_indent = peek_is_paren_indent(scanner);
    uint16_t current_indent_length = peek_indent_length(scanner);

    if (found_bracket_end && valid_symbols[DEDENT]) {
      pop_indent(scanner);
      lexer->result_symbol = DEDENT;
      return true;
    }

    if (found_end_of_line) {
      if (indent_length == current_indent_length && indent_length > 0 &&
          !found_start_of_infix_op && !found_bracket_end) {
        if (valid_symbols[NEWLINE] && !found_preprocessor_end &&
            !found_comment_start) {
          lexer->result_symbol = NEWLINE;
          return true;
        }
      }

      bool can_dedent_preproc;

      if (scanner->preprocessor_indents.size > 0) {
        uint16_t current_preproc_length =
            *array_back(&scanner->preprocessor_indents);
        can_dedent_preproc = current_preproc_length < indent_length;
      } else {
        can_dedent_preproc = true;
      }

      bool can_dedent_infix_op;

      if (found_start_of_infix_op) {
        can_dedent_infix_op = indent_length + 2 < current_indent_length;
      } else {
        can_dedent_infix_op = true;
      }

      // Inside paren-indented blocks, avoid DEDENT on ordinary under-indented
      // continuation lines. But still allow it at true EOF / column 0 so a
      // paren indent can't get stranded forever if its closing bracket was
      // consumed by the grammar rather than seen here at the start of a line.
      bool can_dedent_paren_indent = !is_paren_indent || indent_length == 0 || lexer->eof(lexer);

      if (indent_length < current_indent_length && !found_bracket_end &&
          can_dedent_preproc && can_dedent_infix_op &&
          (!valid_symbols[TUPLE_MARKER] || valid_symbols[ERROR_SENTINEL]) && can_dedent_paren_indent) {
        pop_indent(scanner);
        lexer->result_symbol = DEDENT;
        return true;
      }
    }
  }

  if (valid_symbols[BLOCK_COMMENT_CONTENT] && !valid_symbols[ERROR_SENTINEL]) {
    lexer->mark_end(lexer);
    while (true) {
      if (lexer->lookahead == '\0') {
        break;
      }
      if (lexer->lookahead != '(' && lexer->lookahead != '*') {
        advance(lexer);
      } else if (lexer->lookahead == '*') {
        lexer->mark_end(lexer);
        advance(lexer);
        if (lexer->lookahead == ')') {
          break;
        }
      } else if (scan_block_comment(lexer)) {
        lexer->mark_end(lexer);
        advance(lexer);
        if (lexer->lookahead == '*') {
          break;
        }
      }
    }
    lexer->result_symbol = BLOCK_COMMENT_CONTENT;
    return true;
  }

  return false;
}

static unsigned serialize(Scanner *scanner, char *buffer) {
  size_t size = 0;

  buffer[size++] = (char)scanner->multi_dollar_count;

  size_t preprocessor_count = scanner->preprocessor_indents.size;
  if (preprocessor_count > UINT8_MAX) {
    preprocessor_count = UINT8_MAX;
  }

  buffer[size++] = (char)preprocessor_count;

  for (size_t iter = 0; iter < preprocessor_count &&
                        size < TREE_SITTER_SERIALIZATION_BUFFER_SIZE;
       iter++) {
    char e = *array_get(&scanner->preprocessor_indents, iter);
    buffer[size++] = e;
  }

  size_t indent_count = scanner->indents.size > 0 ? scanner->indents.size - 1 : 0;
  if (indent_count > UINT8_MAX) {
    indent_count = UINT8_MAX;
  }
  buffer[size++] = (char)indent_count;

  uint32_t iter = 1;
  for (; iter <= indent_count && size < TREE_SITTER_SERIALIZATION_BUFFER_SIZE; ++iter) {
    buffer[size++] = (char)*array_get(&scanner->indents, iter);
  }

  size_t paren_bytes = (indent_count + 7) / 8;
  for (size_t byte_index = 0;
       byte_index < paren_bytes && size < TREE_SITTER_SERIALIZATION_BUFFER_SIZE;
       byte_index++) {
    uint8_t bits = 0;
    for (size_t bit = 0; bit < 8; bit++) {
      size_t indent_index = byte_index * 8 + bit + 1;
      if (indent_index > indent_count) {
        break;
      }
      if (*array_get(&scanner->paren_indents, indent_index)) {
        bits |= (uint8_t)(1u << bit);
      }
    }
    buffer[size++] = (char)bits;
  }

  return size;
}

static void deserialize(Scanner *scanner, const char *buffer, unsigned length) {
  array_delete(&scanner->indents);
  array_push(&scanner->indents, 0);
  array_delete(&scanner->paren_indents);
  array_push(&scanner->paren_indents, false);

  array_delete(&scanner->preprocessor_indents);
  scanner->multi_dollar_count = 0;
  if (length > 0) {
    size_t size = 0;
    scanner->multi_dollar_count = (uint8_t)buffer[size++];

    if (size >= length) return;
    size_t preprocessor_count = (uint8_t)buffer[size++];

    size_t preproc_end = size + preprocessor_count;
    if (preproc_end > length) preproc_end = length;
    for (; size < preproc_end; size++) {
      array_push(&scanner->preprocessor_indents, (unsigned char)buffer[size]);
    }

    if (size >= length) return;
    size_t indent_count = (uint8_t)buffer[size++];
    size_t indent_bytes_end = size + indent_count;
    if (indent_bytes_end > length) indent_bytes_end = length;
    for (; size < indent_bytes_end; size++) {
      array_push(&scanner->indents, (unsigned char)buffer[size]);
    }

    size_t actual_indent_count = scanner->indents.size > 0 ? scanner->indents.size - 1 : 0;
    size_t paren_bytes = (actual_indent_count + 7) / 8;
    for (size_t indent_index = 0; indent_index < actual_indent_count; indent_index++) {
      size_t byte_offset = indent_index / 8;
      bool is_paren_indent = false;
      if (size + byte_offset < length) {
        uint8_t bits = (uint8_t)buffer[size + byte_offset];
        is_paren_indent = ((bits >> (indent_index % 8)) & 1u) != 0;
      }
      array_push(&scanner->paren_indents, is_paren_indent);
    }
    size += paren_bytes;
  }
}

static Scanner *create() {
  Scanner *scanner = ts_calloc(1, sizeof(Scanner));
  array_init(&scanner->indents);
  array_init(&scanner->paren_indents);
  array_init(&scanner->preprocessor_indents);
  deserialize(scanner, NULL, 0);
  return scanner;
}

static void destroy(Scanner *scanner) {
  array_delete(&scanner->indents);
  array_delete(&scanner->paren_indents);
  array_delete(&scanner->preprocessor_indents);
  ts_free(scanner);
}

#endif // TREE_SITTER_FSHARP_SCANNER_H_
