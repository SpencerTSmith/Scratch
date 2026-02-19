#ifndef C_PARSE
#define C_PARSE

#include "common.h"

#include "c_tokenize.h"

// FIXME:
// FIXME:
// FIXME:
// FIXME:
// - Will probably need to start having distinct types for statement links, once start actually using the AST
//
// TODO:
// - Functionality:
//   - Type parsing
//     - Struct bit fields
//     - Comma separated identifiers for same declarator i.e. int i, j;
//     - Caching these for typechecking/referencing in symbol table
//   - Symbol Table(s)

#define C_Node_Type(X)           \
  X(C_NODE_NONE)                 \
  X(C_NODE_IDENTIFIER)           \
  X(C_NODE_TYPE)                 \
  X(C_NODE_TYPE_POINTER)         \
  X(C_NODE_TYPE_FUNCTION)        \
  X(C_NODE_TYPE_ARRAY)           \
  X(C_NODE_LITERAL)              \
  X(C_NODE_COMPOUND_LITERAL)     \
  X(C_NODE_ROOT)                 \
  X(C_NODE_VARIABLE_DECLARATION) \
  X(C_NODE_STRUCT_DECLARATION)   \
  X(C_NODE_ENUM_DECLARATION)     \
  X(C_NODE_DECLARATOR_LIST)      \
  X(C_NODE_DECLARATOR)           \
  X(C_NODE_FUNCTION_DECLARATION) \
  X(C_NODE_TYPEDEF)              \
  X(C_NODE_UNARY)                \
  X(C_NODE_BINARY)               \
  X(C_NODE_TERNARY)              \
  X(C_NODE_FUNCTION_CALL)        \
  X(C_NODE_BLOCK)                \
  X(C_NODE_IF)                   \
  X(C_NODE_ELSE)                 \
  X(C_NODE_WHILE)                \
  X(C_NODE_RETURN)               \
  X(C_NODE_FOR)                  \
  X(C_NODE_DO_WHILE)             \
  X(C_NODE_BREAK)                \
  X(C_NODE_CONTINUE)             \
  X(C_NODE_SWITCH)               \
  X(C_NODE_CASE)                 \
  X(C_NODE_DEFAULT)              \
  X(C_NODE_GOTO)                 \
  X(C_NODE_LABEL)                \
  X(C_NODE_COUNT)

ENUM_TABLE(C_Node_Type);

#define C_MIN_PRECEDENCE INT32_MIN

typedef enum C_Binary
{
  C_BINARY_NONE,

  C_BINARY_ACCESS,
  C_BINARY_POINTER_ACCESS,
  C_BINARY_ARRAY_ACCESS,
  C_BINARY_MULTIPLY,
  C_BINARY_DIVIDE,
  C_BINARY_MODULO,
  C_BINARY_ADD,
  C_BINARY_SUBTRACT,
  C_BINARY_LEFT_SHIFT,
  C_BINARY_RIGHT_SHIFT,
  C_BINARY_LESS_THAN,
  C_BINARY_LESS_THAN_EQUAL,
  C_BINARY_GREATER_THAN,
  C_BINARY_GREATER_THAN_EQUAL,
  C_BINARY_COMPARE_EQUAL,
  C_BINARY_COMPARE_NOT_EQUAL,
  C_BINARY_BITWISE_AND,
  C_BINARY_XOR,
  C_BINARY_BITWISE_OR,
  C_BINARY_LOGICAL_AND,
  C_BINARY_LOGICAL_OR,
  C_BINARY_ASSIGN,
  C_BINARY_ADD_ASSIGN,
  C_BINARY_SUBTRACT_ASSIGN,
  C_BINARY_MULTIPLY_ASSIGN,
  C_BINARY_DIVIDE_ASSIGN,
  C_BINARY_MODULO_ASSIGN,
  C_BINARY_AND_ASSIGN,
  C_BINARY_OR_ASSIGN,
  C_BINARY_XOR_ASSIGN,
  C_BINARY_LEFT_SHIFT_ASSIGN,
  C_BINARY_RIGHT_SHIFT_ASSIGN,
  C_BINARY_COMMA,

  C_BINARY_COUNT,
} C_Binary;

typedef enum C_Unary
{
  C_UNARY_NONE,

  C_UNARY_PRE_INCREMENT,
  C_UNARY_PRE_DECREMENT,
  C_UNARY_POST_INCREMENT,
  C_UNARY_POST_DECREMENT,
  C_UNARY_NEGATE,
  C_UNARY_PLUS,
  C_UNARY_REFERENCE,
  C_UNARY_DEREFERENCE,
  C_UNARY_BITWISE_NOT,
  C_UNARY_LOGICAL_NOT,
  C_UNARY_CAST,
  C_UNARY_SIZEOF,

  C_UNARY_COUNT,
} C_Unary;

typedef enum C_Declaration_Flags
{
  C_DECLARATION_FLAG_NONE     = 0,
  C_DECLARATION_FLAG_CONST    = 1 << 0,
  C_DECLARATION_FLAG_STATIC   = 1 << 1,
  C_DECLARATION_FLAG_EXTERN   = 1 << 2,
  C_DECLARATION_FLAG_VOLATILE = 1 << 3,
  C_DECLARATION_FLAG_RESTRICT = 1 << 4,
} C_Declaration_Flags;

typedef struct C_Node C_Node;

// Just for acceleration, that is, don't need to traverse the child linked list.
typedef struct C_Statement_Links C_Statement_Links;
struct C_Statement_Links
{
  C_Node *init;
  C_Node *condition;
  C_Node *update;
};

struct C_Node
{
  C_Node_Type type;

  C_Node *parent;

  C_Node *first_child;
  C_Node *last_child;
  usize  child_count;

  C_Node *next_sibling;
  C_Node *prev_sibling;

  C_Statement_Links links;

  C_Declaration_Flags declaration_flags;

  union
  {
    String    name;
    C_Literal literal;
    C_Binary  binary;
    C_Unary   unary;
  };
};

typedef struct C_Parser C_Parser;
struct C_Parser
{
  String source;

  C_Token_Array tokens;
  usize         at;

  i32 loop_nests;
  i32 switch_nests;

  b32 had_error;
};

typedef struct C_Token_Node_Map C_Token_Node_Map;
struct C_Token_Node_Map
{
  C_Unary  unary;
  C_Binary binary;
  i32 unary_precedence;
  i32 binary_precedence;
};

// TODO: Could maybe condense this by just setting the enum values to their precedence...
// also could make sure the C_TOKEN_... are low numbers to keep this table small
static
C_Token_Node_Map token_to_node_table[] =
{
  [C_TOKEN_DOT]                = { C_UNARY_NONE,          C_BINARY_ACCESS,             C_MIN_PRECEDENCE, 15 },
  [C_TOKEN_ARROW]              = { C_UNARY_NONE,          C_BINARY_POINTER_ACCESS,     C_MIN_PRECEDENCE, 15 },
  [C_TOKEN_BEGIN_SQUARE_BRACE] = { C_UNARY_NONE,          C_BINARY_ARRAY_ACCESS,       C_MIN_PRECEDENCE, 15 },
  [C_TOKEN_INCREMENT]          = { C_UNARY_PRE_INCREMENT, C_BINARY_NONE,               15,               C_MIN_PRECEDENCE }, // NOTE: Special-case
  [C_TOKEN_DECREMENT]          = { C_UNARY_PRE_DECREMENT, C_BINARY_NONE,               15,               C_MIN_PRECEDENCE }, // NOTE: Special-case
  [C_TOKEN_BITWISE_NOT]        = { C_UNARY_BITWISE_NOT,   C_BINARY_NONE,               14,               C_MIN_PRECEDENCE },
  [C_TOKEN_LOGICAL_NOT]        = { C_UNARY_LOGICAL_NOT,   C_BINARY_NONE,               14,               C_MIN_PRECEDENCE },
  [C_TOKEN_BITWISE_AND]        = { C_UNARY_REFERENCE,     C_BINARY_BITWISE_AND,        14,               8 },
  [C_TOKEN_STAR]               = { C_UNARY_DEREFERENCE,   C_BINARY_MULTIPLY,           14,               13 },
  [C_TOKEN_ADD]                = { C_UNARY_PLUS,          C_BINARY_ADD,                14,               12 },
  [C_TOKEN_MINUS]              = { C_UNARY_NEGATE,        C_BINARY_SUBTRACT,           14,               12 },
  [C_TOKEN_DIVIDE]             = { C_UNARY_NONE,          C_BINARY_DIVIDE,             C_MIN_PRECEDENCE, 12 },
  [C_TOKEN_MODULO]             = { C_UNARY_NONE,          C_BINARY_MODULO,             C_MIN_PRECEDENCE, 12 },
  [C_TOKEN_LEFT_SHIFT]         = { C_UNARY_NONE,          C_BINARY_LEFT_SHIFT,         C_MIN_PRECEDENCE, 11 },
  [C_TOKEN_RIGHT_SHIFT]        = { C_UNARY_NONE,          C_BINARY_RIGHT_SHIFT,        C_MIN_PRECEDENCE, 11 },
  [C_TOKEN_LESS_THAN]          = { C_UNARY_NONE,          C_BINARY_LESS_THAN,          C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_LESS_THAN_EQUAL]    = { C_UNARY_NONE,          C_BINARY_LESS_THAN_EQUAL,    C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_GREATER_THAN]       = { C_UNARY_NONE,          C_BINARY_GREATER_THAN,       C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_GREATER_THAN_EQUAL] = { C_UNARY_NONE,          C_BINARY_GREATER_THAN_EQUAL, C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_COMPARE_EQUAL]      = { C_UNARY_NONE,          C_BINARY_COMPARE_EQUAL,      C_MIN_PRECEDENCE, 9 },
  [C_TOKEN_COMPARE_NOT_EQUAL]  = { C_UNARY_NONE,          C_BINARY_COMPARE_NOT_EQUAL,  C_MIN_PRECEDENCE, 9 },
  [C_TOKEN_XOR]                = { C_UNARY_NONE,          C_BINARY_XOR,                C_MIN_PRECEDENCE, 7 },
  [C_TOKEN_BITWISE_OR]         = { C_UNARY_NONE,          C_BINARY_BITWISE_OR,         C_MIN_PRECEDENCE, 6 },
  [C_TOKEN_LOGICAL_AND]        = { C_UNARY_NONE,          C_BINARY_LOGICAL_AND,        C_MIN_PRECEDENCE, 5 },
  [C_TOKEN_LOGICAL_OR]         = { C_UNARY_NONE,          C_BINARY_LOGICAL_OR,         C_MIN_PRECEDENCE, 4 },
  [C_TOKEN_QUESTION]           = { C_UNARY_NONE,          C_BINARY_NONE,               C_MIN_PRECEDENCE, 3 }, // NOTE: Ternary here for convenience
  [C_TOKEN_ASSIGN]             = { C_UNARY_NONE,          C_BINARY_ASSIGN,             C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_ADD_ASSIGN]         = { C_UNARY_NONE,          C_BINARY_ADD_ASSIGN,         C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_SUBTRACT_ASSIGN]    = { C_UNARY_NONE,          C_BINARY_SUBTRACT_ASSIGN,    C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_MULTIPLY_ASSIGN]    = { C_UNARY_NONE,          C_BINARY_MULTIPLY_ASSIGN,    C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_DIVIDE_ASSIGN]      = { C_UNARY_NONE,          C_BINARY_DIVIDE_ASSIGN,      C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_MODULO_ASSIGN]      = { C_UNARY_NONE,          C_BINARY_MODULO_ASSIGN,      C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_AND_ASSIGN]         = { C_UNARY_NONE,          C_BINARY_AND_ASSIGN,         C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_OR_ASSIGN]          = { C_UNARY_NONE,          C_BINARY_OR_ASSIGN,          C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_XOR_ASSIGN]         = { C_UNARY_NONE,          C_BINARY_XOR_ASSIGN,         C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_LEFT_SHIFT_ASSIGN]  = { C_UNARY_NONE,          C_BINARY_LEFT_SHIFT_ASSIGN,  C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_RIGHT_SHIFT_ASSIGN] = { C_UNARY_NONE,          C_BINARY_RIGHT_SHIFT_ASSIGN, C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_COMMA]              = { C_UNARY_NONE,          C_BINARY_COMMA,              C_MIN_PRECEDENCE, 1 },
};

static
C_Token_Node_Map c_token_to_node_mapping(C_Token_Type type)
{
  C_Token_Node_Map result = {0};

  if (type > 0 && type < STATIC_COUNT(token_to_node_table))
  {
    result = token_to_node_table[type];
  }

  return result;
}

// Could easily convert these to lookups
static
b32 c_token_is_type_keyword(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = t == C_TOKEN_KEYWORD_VOID     ||
               t == C_TOKEN_KEYWORD_CHAR     ||
               t == C_TOKEN_KEYWORD_SIGNED   ||
               t == C_TOKEN_KEYWORD_UNSIGNED ||
               t == C_TOKEN_KEYWORD_SHORT    ||
               t == C_TOKEN_KEYWORD_INT      ||
               t == C_TOKEN_KEYWORD_LONG     ||
               t == C_TOKEN_KEYWORD_FLOAT    ||
               t == C_TOKEN_KEYWORD_DOUBLE;

  return result;
}

static
C_Declaration_Flags c_token_to_declaration_flag(C_Token token)
{
  C_Token_Type t = token.type;

  C_Declaration_Flags result = C_DECLARATION_FLAG_NONE;

  switch (t)
  {
    default: {} break;
    case C_TOKEN_KEYWORD_CONST:
    {
      result = C_DECLARATION_FLAG_CONST;
    } break;
    case C_TOKEN_KEYWORD_STATIC:
    {
      result = C_DECLARATION_FLAG_STATIC;
    } break;
    case C_TOKEN_KEYWORD_EXTERN:
    {
      result = C_DECLARATION_FLAG_EXTERN;
    } break;
    case C_TOKEN_KEYWORD_VOLATILE:
    {
      result = C_DECLARATION_FLAG_VOLATILE;
    } break;
    case C_TOKEN_KEYWORD_RESTRICT:
    {
      result = C_DECLARATION_FLAG_RESTRICT;
    } break;
  }

  return result;
}

static
b32 c_token_is_unary_operator(C_Token token)
{
  b32 result = c_token_to_node_mapping(token.type).unary != C_UNARY_NONE;

  return result;
}

static
b32 c_token_is_binary_operator(C_Token token)
{
  b32 result = c_token_to_node_mapping(token.type).binary != C_BINARY_NONE;

  return result;
}

static
C_Binary c_token_to_binary(C_Token token)
{
  C_Binary result = c_token_to_node_mapping(token.type).binary;

  return result;
}

static
C_Unary c_token_to_unary(C_Token token, b32 is_post)
{
  C_Unary result = c_token_to_node_mapping(token.type).unary;

  // Special case for post increment and decrement
  if (is_post)
  {
    if (result == C_UNARY_PRE_INCREMENT)
    {
      result = C_UNARY_POST_INCREMENT;
    }
    else if (result == C_UNARY_PRE_DECREMENT)
    {
      result = C_UNARY_POST_DECREMENT;
    }
  }

  return result;
}

static
i32 c_operator_precedence(C_Token token, b32 is_unary, b32 is_post)
{
  i32 result = C_MIN_PRECEDENCE;

  C_Token_Node_Map info = c_token_to_node_mapping(token.type);
  if (is_unary)
  {
    result = info.unary_precedence;

    // NOTE: Special case
    if ((info.unary == C_UNARY_PRE_INCREMENT || info.unary == C_UNARY_PRE_DECREMENT) && is_post)
    {
      result += 1;
    }
  }
  else
  {
    result = info.binary_precedence;
  }

  return result;
}

static
b32 c_binary_is_right_associative(C_Binary binary)
{
  b32 result = false;

  if (binary == C_BINARY_ASSIGN             ||
      binary == C_BINARY_ADD_ASSIGN         ||
      binary == C_BINARY_SUBTRACT_ASSIGN    ||
      binary == C_BINARY_MULTIPLY_ASSIGN    ||
      binary == C_BINARY_DIVIDE_ASSIGN      ||
      binary == C_BINARY_MODULO_ASSIGN      ||
      binary == C_BINARY_LEFT_SHIFT_ASSIGN  ||
      binary == C_BINARY_RIGHT_SHIFT_ASSIGN ||
      binary == C_BINARY_AND_ASSIGN         ||
      binary == C_BINARY_XOR_ASSIGN         ||
      binary == C_BINARY_OR_ASSIGN)
  {
    result = true;
  }

  return result;
}

static
C_Token c_parse_peek(C_Parser parser, isize offset)
{
  C_Token result = {0};

  if (parser.at + offset < parser.tokens.count && ((isize)parser.at + offset) >= 0)
  {
    result = parser.tokens.v[parser.at + offset];
  }

  return result;
}

static
b32 c_parse_match(C_Parser *parser, C_Token_Type type)
{
  return c_parse_peek(*parser, 0).type == type;
}

static
b32 c_parse_eat(C_Parser *parser, C_Token_Type type)
{
  b32 result = false;

  if (c_parse_match(parser, type))
  {
    result = true;
    parser->at += 1;
  }

  return result;
}

// TODO: Maybe consider splitting source by line before-hand...
// though hopefully this will only be needed for error reporting.
static
String c_get_line(String source, usize wish_line)
{
  usize substring_start = 0;
  usize substring_close = 0;

  b32 found_line = false;

  usize current_line = 1;
  for (usize char_index = 0; char_index < source.count; char_index++)
  {
    u8 c = source.v[char_index];

    if (c == '\n')
    {
      current_line += 1;

      if (found_line)
      {
        substring_close = char_index;
        break;
      }
      else
      {
        continue;
      }
    }

    if (current_line == wish_line && !found_line)
    {
      substring_start = char_index;
      found_line = true;
    }
  }

  return string_substring(source, substring_start, substring_close);
}

static
void c_parse_error(C_Parser *parser, char *format, ...)
{
  C_Token current_token = c_parse_peek(*parser, 0);

  usize line_number = current_token.line;
  String line = c_get_line(parser->source, line_number);

  printf("Parse Error, line %lu:\n", line_number);

  va_list var_args;
  va_start(var_args, format);
  vprintf(format, var_args);
  va_end(var_args);

  printf("\n :: %.*s\n", STRF(line));

  parser->had_error = true;
}

static
void c_node_add_child(C_Node *parent, C_Node *child);

static
C_Node *c_nil_node()
{
  static C_Node nil = {0};

  static b32 init = false;

  if (!init)
  {
    nil.first_child  = &nil;
    nil.last_child   = &nil;
    nil.next_sibling = &nil;
    nil.prev_sibling = &nil;
    init = true;
  }

  return &nil;
}

static
C_Node *c_new_node(Arena *arena, C_Node_Type type)
{
  C_Node *result = arena_new(arena, C_Node);
  result->type = type;

  result->parent       = c_nil_node();
  result->first_child  = c_nil_node();
  result->last_child   = c_nil_node();
  result->next_sibling = c_nil_node();
  result->prev_sibling = c_nil_node();

  return result;
}

static
void c_node_add_child(C_Node *parent, C_Node *child)
{
  if (child != c_nil_node())
  {
    child->parent = parent;
    DLL_push_last_nil(parent->first_child, parent->last_child,
                      child, next_sibling, prev_sibling, c_nil_node());
    parent->child_count += 1;
  }
}

static
b32 c_parse_incomplete(C_Parser parser)
{
  return parser.at < parser.tokens.count && !parser.had_error;
}

static
C_Node *c_parse_enum_or_struct_declaration(Arena *arena, C_Parser *parser, C_Token_Type enum_or_struct);

static
C_Declaration_Flags c_parse_declaration_flag_chain(C_Parser *parser)
{
  C_Declaration_Flags result = C_DECLARATION_FLAG_NONE;

  while (true)
  {
    C_Token maybe_flag = c_parse_peek(*parser, 0);

    C_Declaration_Flags flag = c_token_to_declaration_flag(maybe_flag);

    if (flag == C_DECLARATION_FLAG_NONE)
    {
      break;
    }

    result |= flag;
    c_parse_eat(parser, maybe_flag.type);
  }

  return result;
}

static
C_Node *c_parse_expression(Arena *arena, C_Parser *parser, i32 min_precedence);

static
C_Node *c_parse_postfix_declarators(Arena *arena, C_Parser *parser, C_Node *after_pointers_type)
{
  C_Node *result = after_pointers_type;


  return result;
}

// NOTE: This will also handle just plain struct/enum declarations, since this is only called in parse_full_declarator, if the declarator has no identifier,
// we can then decide this is just a simple struct/enum decl. at the right time.
static
C_Node *c_parse_base_type(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  // Pre-type flags
  C_Declaration_Flags flags = c_parse_declaration_flag_chain(parser);

  C_Token token = c_parse_peek(*parser, 0);

  if (c_token_is_type_keyword(token))
  {
    result = c_new_node(arena, C_NODE_TYPE);
    result->name = token.raw;
    result->declaration_flags = flags;

    c_parse_eat(parser, token.type);
  }
  else if (token.type == C_TOKEN_KEYWORD_STRUCT || token.type == C_TOKEN_KEYWORD_ENUM)
  {
    result = c_parse_enum_or_struct_declaration(arena, parser, token.type);
  }
  else if (token.type == C_TOKEN_IDENTIFIER) // Custom type
  {
  }

  // Post fix flags immediately after still apply to the base type
  if (result != c_nil_node())
  {
    result->declaration_flags |= c_parse_declaration_flag_chain(parser);
  }

  return result;
}

static
C_Node *c_parse_identifier(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Token token = c_parse_peek(*parser, 0);

  if (token.type == C_TOKEN_IDENTIFIER)
  {
    result = c_new_node(arena, C_NODE_IDENTIFIER);
    result->name = token.raw;

    c_parse_eat(parser, C_TOKEN_IDENTIFIER);
  }

  return result;
}

static
C_Node *c_parse_function_call(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  // I suppose this check is not necessary since only called in one place.
  if (c_parse_peek(*parser, 0).type == C_TOKEN_IDENTIFIER)
  {
    result = c_new_node(arena, C_NODE_FUNCTION_CALL);

    C_Node *identifier = c_parse_identifier(arena, parser);
    c_node_add_child(result, identifier);

    c_parse_eat(parser, C_TOKEN_IDENTIFIER);

    if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
    {
      // Keep going until we consume a close parenthesis.
      while (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
      {
        // Pass precedence of comma so that we stop parsing at comma.
        C_Node *argument = c_parse_expression(arena, parser, token_to_node_table[C_TOKEN_COMMA].binary_precedence);
        c_node_add_child(result, argument);

        // Skip comma
        c_parse_eat(parser, C_TOKEN_COMMA);
      }
    }
    else
    {
      c_parse_error(parser, "Expected function call to have open parenthesis");
    }
  }
  else
  {
    c_parse_error(parser, "Expected function call to begin with identifier");
  }

  return result;
}

static
C_Node *c_parse_full_declarators(Arena *arena, C_Parser *parser);

// We parse identifiers, function calls, prefix unary ops, and open parenthesis here.
static
C_Node *c_parse_expression_start(Arena *arena, C_Parser *parser)
{
  C_Token token = c_parse_peek(*parser, 0);

  C_Node *result = c_nil_node();

  if (token.type == C_TOKEN_LITERAL)
  {
    result = c_new_node(arena, C_NODE_LITERAL);

    result->literal = token.literal; // Copy over

    c_parse_eat(parser, C_TOKEN_LITERAL);
  }
  // Compound Literal
  else if (token.type == C_TOKEN_BEGIN_CURLY_BRACE)
  {
    c_parse_eat(parser, C_TOKEN_BEGIN_CURLY_BRACE);

    // TODO: Maybe can consolidate compound literal with normal literals
    result = c_new_node(arena, C_NODE_COMPOUND_LITERAL);

    // Commas must act like separators so pass its precedence when parsing expressions
    i32 comma_precedence = token_to_node_table[C_TOKEN_COMMA].binary_precedence;

    while (!c_parse_eat(parser, C_TOKEN_CLOSE_CURLY_BRACE))
    {
      C_Node *initializer = c_nil_node();
      // Designated initializer
      if (c_parse_eat(parser, C_TOKEN_DOT))
      {
        initializer = c_parse_identifier(arena, parser);

        if (initializer == c_nil_node())
        {
          c_parse_error(parser, "Expected identifier following . in designated initializer");
          break;
        }

        if(c_parse_eat(parser, C_TOKEN_ASSIGN))
        {
          C_Node *expression = c_parse_expression(arena, parser, comma_precedence);
          c_node_add_child(initializer, expression);
        }
        else
        {
          c_parse_error(parser, "Expected assignment following designated initializer for member '%.*s'.", STRF(initializer->name));
          break;
        }
      }
      // Positional initializer
      else
      {
        // Commas must act like separators so pass its precedence
        initializer = c_parse_expression(arena, parser, comma_precedence);
      }

      c_node_add_child(result, initializer);

      // Skip comma separators
      c_parse_eat(parser, C_TOKEN_COMMA);
    }
  }
  else if (token.type == C_TOKEN_IDENTIFIER)
  {
    C_Token peek = c_parse_peek(*parser, 1);

    if (peek.type != C_TOKEN_BEGIN_PARENTHESIS)
    {
      result = c_parse_identifier(arena, parser);
    }
    else
    {
      result = c_parse_function_call(arena, parser);
    }
  }
  // As a side effect, handling prefix unaries here ensures right associativity.
  else if (c_token_is_unary_operator(token))
  {
    result = c_new_node(arena, C_NODE_UNARY);

    b32 is_post = false;

    result->unary = c_token_to_unary(token, is_post);
    c_parse_eat(parser, token.type);

    b32 is_unary = true;
    i32 precedence = c_operator_precedence(token, is_unary, is_post);
    C_Node *child = c_parse_expression(arena, parser, precedence);
    c_node_add_child(result, child);
  }
  else if (token.type == C_TOKEN_BEGIN_PARENTHESIS)
  {
    c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS);

    // Check if we can parse this as an abstract declarator
    C_Node *type = c_parse_full_declarators(arena, parser);
    // TODO: Check that this is truly an abstract declarator.

    // It is a unary cast if we were able to parse a declarator..
    if (type != c_nil_node())
    {
      result = c_new_node(arena, C_NODE_UNARY);
      result->unary = C_UNARY_CAST;

      c_node_add_child(result, type);

      if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
      {
        c_parse_error(parser, "Expected close parenthesis following cast.");
      }

      // FIXME: pass unary precedence in a good way, perhaps factor unary creation out into a function
      C_Node *child = c_parse_expression(arena, parser, 14);
      c_node_add_child(type, child);
    }

    // Its an expression
    else
    {
      // Parenthesis resets precedence
      result = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);

      if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
      {
        c_parse_error(parser, "Expected close parenthesis following expression.");
      }
    }
  }
  else if (token.type == C_TOKEN_KEYWORD_SIZEOF)
  {
    c_parse_eat(parser, C_TOKEN_KEYWORD_SIZEOF);

    if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
    {
      result = c_new_node(arena, C_NODE_UNARY);
      result->unary = C_UNARY_SIZEOF;

      // Try to get an abstract declarator
      C_Node *child = c_parse_full_declarators(arena, parser);
    // TODO: Check that this is truly an abstract declarator.

      // We couldn't get an abstract declarator, has to be expression.
      if (child == c_nil_node())
      {
        child = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
      }

      c_node_add_child(result, child);
    }
    else
    {
      c_parse_error(parser, "Expected ( following sizeof.");
    }
  }

  return result;
}

static
C_Node *c_parse_expression(Arena *arena, C_Parser *parser, i32 min_precedence)
{
  C_Node *left = c_parse_expression_start(arena, parser);

  C_Node *result = left; // Return just the left if we don't meet later checks

  // Idea from https://www.youtube.com/watch?v=fIPO4G42wYE&t=4260s
  while (true)
  {
    C_Token peek = c_parse_peek(*parser, 0);

    if (c_token_is_binary_operator(peek))
    {
      C_Binary operator  = c_token_to_binary(peek);
      i32 new_precedence = c_operator_precedence(peek, false, false);

      // Assignments are right associative, everything else is left
      b32 should_right_associate = (new_precedence > min_precedence) ||
                                   (new_precedence == min_precedence && c_binary_is_right_associative(operator));

      if (should_right_associate)
      {
        // We can recurse down and build a deeper tree
        result = c_new_node(arena, C_NODE_BINARY);
        result->binary = operator;

        c_parse_eat(parser, peek.type);

        // We act like the inner expression of array access is surrounded by parenthesis
        if (operator == C_BINARY_ARRAY_ACCESS)
        {
          new_precedence = C_MIN_PRECEDENCE;
        }

        C_Node *right = c_parse_expression(arena, parser, new_precedence);

        // For array access, skip over the subsequent close square brace
        if (operator == C_BINARY_ARRAY_ACCESS)
        {
          if(!c_parse_eat(parser, C_TOKEN_CLOSE_SQUARE_BRACE))
          {
            c_parse_error(parser, "Expected closing square brace for array access");
          }
        }

        c_node_add_child(result, left);
        c_node_add_child(result, right);
      }
      else
      {
        // We need to head back up the tree (i.e. pop some recursive calls)
        // to parse the next operator, need a lower precedence operator
        break;
      }
    }
    // Handle postfix unary operators here
    else if (peek.type == C_TOKEN_INCREMENT || peek.type == C_TOKEN_DECREMENT)
    {
      b32 is_post  = true;
      b32 is_unary = true;

      i32 new_precedence = c_operator_precedence(peek, is_unary, is_post);

      if (new_precedence > min_precedence)
      {
        result = c_new_node(arena, C_NODE_UNARY);

        result->unary = c_token_to_unary(peek, is_post);

        c_parse_eat(parser, peek.type);

        c_node_add_child(result, left);
      }
      else
      {
        // We need to head back up the tree (i.e. pop some recursive calls)
        // to parse the next operator, need a lower precedence operator
        break;
      }
    }
    else
    {
      // No more binary operators, we are done.
      break;
    }

    // Now left hand side becomes this new tree
    left = result;
  }


  // Check for ternary and if we were called with a lower or equal (since ternary is right-associative) precedence than a ternary...
  // (assignment operators, comma), if so grab it, rebuild result tree
  // since we know that actually the assignment wants to be assigned
  // to the evaluation of the ternary... that is, place the ternary
  // on the right side of the tree when our parent is =, +=, -=, etc.
  // This could potentially be put into the main loop, but since this is just such a special case I decided against it.

  C_Token post_peek = c_parse_peek(*parser, 0);
  if (post_peek.type == C_TOKEN_QUESTION &&
      c_operator_precedence(post_peek, false, false) >= min_precedence)
  {
    c_parse_eat(parser, C_TOKEN_QUESTION);

    // We know what we have so far is the condition for the ternary.
    C_Node *condition = result;

    result = c_new_node(arena, C_NODE_TERNARY);

    c_node_add_child(result, condition);

    // Middle expr should act as if parenthesis according to docs
    C_Node *true_expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
    c_node_add_child(result, true_expression);

    if (!c_parse_eat(parser, C_TOKEN_COLON))
    {
      c_parse_error(parser, "Expected colon after 2nd ternary argument.");
    }

    // This gets the precedence of the ternary so we can parse commas correctly as not part of the false expression...
    // Though, this lead to side effect of things like a ? b : c = e being parsed even though they are invalid.
    i32 ternary_precedence = c_operator_precedence(post_peek, false, false);
    C_Node *false_expression = c_parse_expression(arena, parser, ternary_precedence);
    c_node_add_child(result, false_expression);
  }

  return result;
}

typedef struct C_Declarator_Item C_Declarator_Item;
struct C_Declarator_Item
{
  C_Node *type_tree;
  C_Node *identifier;
};

static
C_Node *c_parse_block(Arena *arena, C_Parser *parser);

static
C_Declarator_Item c_parse_declarator_item(Arena *arena, C_Parser *parser)
{
  C_Declarator_Item result =
  {
    .type_tree  = c_nil_node(),
    .identifier = c_nil_node()
  };

  // Grab pointers. Left leaning tree
  // But don't attach yet! We attach these last at the bottom.
  C_Node *pointer_tree = c_nil_node();
  while (c_parse_eat(parser, C_TOKEN_STAR))
  {
    C_Node *pointer = c_new_node(arena, C_NODE_TYPE_POINTER);

    // Flags immediately following apply to this pointer
    pointer->declaration_flags |= c_parse_declaration_flag_chain(parser);

    c_node_add_child(pointer, pointer_tree);
    pointer_tree = pointer;
  }

  // Potentially grab an identifier, a grouped declarator piece, or nothing if abstract.
  C_Token peek = c_parse_peek(*parser, 0);
  if (peek.type == C_TOKEN_IDENTIFIER)
  {
    result.identifier = c_parse_identifier(arena, parser);
  }
  else if (peek.type == C_TOKEN_BEGIN_PARENTHESIS)
  {
    c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS);

    // We recurse because we want whatever comes out of this to be at the top of the current tree.
    result = c_parse_declarator_item(arena, parser);

    if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
    {
      c_parse_error(parser, "Expected closing parenthesis in grouped declarator");
    }
  }
  // Else its an abstract declarator.

  // Traverse all the way to the bottom. We must attach the array, then the pointers,
  // in that order.
  C_Node *result_bottom = result.type_tree;
  while (result_bottom->first_child != c_nil_node() &&
         (result_bottom->first_child->type == C_NODE_TYPE_ARRAY ||
          result_bottom->first_child->type == C_NODE_TYPE_POINTER))
  {
    result_bottom = result_bottom->first_child;
  }

  // Grab the post fix declarator pieces, [], ()...
  while (true)
  {
    // Array
    if (c_parse_eat(parser, C_TOKEN_BEGIN_SQUARE_BRACE))
    {
      C_Node *array = c_new_node(arena, C_NODE_TYPE_ARRAY);

      // Always on bottom.
      if (result_bottom == c_nil_node())
      {
        result.type_tree = array;
        result_bottom      = array;
      }
      else
      {
        c_node_add_child(result_bottom, array);
        result_bottom = array;
      }

      // FIXME: Ok it has now become urgent to fix the super generic links between nodes
      C_Node *array_count = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
      c_node_add_child(array, array_count);

      if (!c_parse_eat(parser, C_TOKEN_CLOSE_SQUARE_BRACE))
      {
        c_parse_error(parser, "Expected closing square brace in array type declaration.");
        break;
      }
    }
    // Function.
    else if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
    {
      C_Node *function = c_new_node(arena, C_NODE_TYPE_FUNCTION);

      b32 is_function_pointer = false;

      // Always on bottom.
      if (result_bottom == c_nil_node())
      {
        result.type_tree = function;
        result_bottom    = function;
      }
      else
      {
        c_node_add_child(result_bottom, function);
        result_bottom = function;
      }

      while (!c_parse_match(parser, C_TOKEN_CLOSE_PARENTHESIS))
      {
        do
        {
          // Even function pointers may have non-abstract declarator parameters,
          // so we should be fine to do the same thing for all cases.
          C_Node *parameter = c_parse_full_declarators(arena, parser);
          c_node_add_child(function, parameter);
        }
        while (c_parse_eat(parser, C_TOKEN_COMMA));
      }

      if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
      {
        c_parse_error(parser, "Expected close parenthesis at end of function parameter list");
      }
    }
    else
    {
      break;
    }
  }

  // Attach pointer tree to bottom, after having parsed it earlier.
  if (result.type_tree == c_nil_node())
  {
    result.type_tree = pointer_tree;
  }
  else
  {
    c_node_add_child(result_bottom, pointer_tree);
  }

  return result;
}

static
C_Node *c_parse_declarator(Arena *arena, C_Parser *parser, C_Node *base_type)
{
  C_Node *result = c_new_node(arena, C_NODE_DECLARATOR);
  C_Declarator_Item item = c_parse_declarator_item(arena, parser);
  c_node_add_child(result, base_type);
  c_node_add_child(result, item.type_tree);
  c_node_add_child(result, item.identifier);

  return result;
}

// For parsing things like int i ... float j ... struct Foo_T j
// As a nice side effect parsing base type will work for parsing struct/enum declarations and subsequent pointer/postfix/identifier checks will do nothing
// (of course if we get valid syntax, you could potentially have a const struct declaration, invalid postfix on struct declaration with no identifier, should probably do error checking that case...)
//
// Can also parse abstract declarators if want_identifier is false.
static
C_Node *c_parse_full_declarators(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  // Try to grab the base type.
  C_Node *base_type  = c_parse_base_type(arena, parser);
  if (base_type != c_nil_node())
  {
    result = c_new_node(arena, C_NODE_DECLARATOR_LIST);
    do
    {
      C_Node *declarator = c_parse_declarator(arena, parser, base_type);
      c_node_add_child(result, declarator);
    } while (c_parse_eat(parser, C_TOKEN_COMMA));
  }

  return result;
}

static
C_Node *c_parse_variable_init(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  // If we see an equal sign, grab initializing expression
  if (c_parse_eat(parser, C_TOKEN_ASSIGN))
  {
    result = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);

    if (result == c_nil_node())
    {
      c_parse_error(parser, "Expected expression to initialize variable declaration.");
    }
  }

  return result;
}

static
C_Node *c_parse_variable_declaration(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Node *declarator = c_parse_full_declarators(arena, parser);

  if (declarator != c_nil_node())
  {
    result = c_new_node(arena, C_NODE_VARIABLE_DECLARATION);
    c_node_add_child(result, declarator);

    C_Node *init = c_parse_variable_init(arena, parser);
    c_node_add_child(result, init);
  }

  return result;
}

static
C_Node *c_parse_declaration(Arena *arena, C_Parser *parser, b32 at_top_level);

static
C_Node *c_parse_statement(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Token peek = c_parse_peek(*parser, 0);
  switch (peek.type)
  {
    case C_TOKEN_SEMICOLON: { c_parse_eat(parser, C_TOKEN_SEMICOLON); } break;

    // Basically same case.
    case C_TOKEN_KEYWORD_WHILE:
    case C_TOKEN_KEYWORD_IF:
    {
      c_parse_eat(parser, peek.type);
      result = c_new_node(arena, peek.type == C_TOKEN_KEYWORD_IF ? C_NODE_IF : C_NODE_WHILE);

      if (peek.type == C_TOKEN_KEYWORD_WHILE)
      {
        parser->loop_nests += 1;
      }

      if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
      {
        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        c_node_add_child(result, condition);
        result->links.condition = condition;

        if (c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
        {
          C_Node *statement = c_parse_statement(arena, parser);
          c_node_add_child(result, statement);

          // Check for else
          if (c_parse_eat(parser, C_TOKEN_KEYWORD_ELSE))
          {
            C_Node *els = c_new_node(arena, C_NODE_ELSE);
            c_node_add_child(result, els);

            C_Node *else_statement = c_parse_statement(arena, parser);
            c_node_add_child(els, else_statement);
          }
        }
        else
        {
          c_parse_error(parser, "Expected close parenthesis following condition expression.");
        }
      }
      else
      {
        c_parse_error(parser, "Expected begin parenthesis following %s.", peek.type == C_TOKEN_KEYWORD_IF ? "if" : "while");
      }

      if (peek.type == C_TOKEN_KEYWORD_WHILE)
      {
        parser->loop_nests -= 1;
      }

    } break;
    case C_TOKEN_KEYWORD_FOR:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_FOR);
      result = c_new_node(arena, C_NODE_FOR);

      parser->loop_nests += 1;

      if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
      {
        // First part is decl or expression
        C_Node *init = c_parse_variable_declaration(arena, parser);

        // Try to parse as expression if not a declaration
        if (init == c_nil_node())
        {
          init = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        }

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
        {
          c_parse_error(parser, "Expected semicolon following first initialization loop clause.");
          // Should continue after failing this?
        }

        c_node_add_child(result, init);
        result->links.init = init;


        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        if (condition != c_nil_node())
        {
          c_node_add_child(result, condition);
        }
        result->links.condition = condition;

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
        {
          c_parse_error(parser, "Expected semicolon following second condition loop clause.");
          // Should continue after failing this?
        }

        C_Node *update = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        if (update != c_nil_node())
        {
          c_node_add_child(result, update);
        }
        result->links.update = update;

        if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
        {
          c_parse_error(parser, "Expected semicolon following third update loop clause.");
          // Should continue after failing this?
        }

        C_Node *statement = c_parse_statement(arena, parser);
        c_node_add_child(result, statement);
      }
      else
      {
        c_parse_error(parser, "Expected begin parenthesis following for.");
      }

      parser->loop_nests -= 1;
    } break;
    case C_TOKEN_KEYWORD_DO:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_DO);

      result = c_new_node(arena, C_NODE_DO_WHILE);

      parser->loop_nests += 1;

      C_Node *statement = c_parse_statement(arena, parser);
      c_node_add_child(result, statement);

      if (c_parse_eat(parser, C_TOKEN_KEYWORD_WHILE))
      {
        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        c_node_add_child(result, condition);
        result->links.condition = condition;
      }
      else
      {
        c_parse_error(parser, "Expected while following do.");
      }

      parser->loop_nests -= 1;

    } break;
    case C_TOKEN_KEYWORD_SWITCH:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_SWITCH);

      result = c_new_node(arena, C_NODE_SWITCH);

      if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
      {
        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        c_node_add_child(result, condition);

        if (c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
        {
          parser->switch_nests += 1;

          // Begin parsing case statement(s).
          // Multiple cases must be contained with a block.
          C_Node *statement = c_parse_statement(arena, parser);

          if (statement->type == C_NODE_CASE || statement->type == C_NODE_BLOCK)
          {
            c_node_add_child(result, statement);
          }
          else
          {
            c_parse_error(parser, "Invalid statement type following switch.");
          }

          parser->switch_nests -= 1;
        }
        else
        {
          c_parse_error(parser, "Expected close parenthesis following switch condition.");
        }
      }
      else
      {
        c_parse_error(parser, "Expected an open parenthesis following switch.");
      }

    } break;
    case C_TOKEN_KEYWORD_DEFAULT:
    case C_TOKEN_KEYWORD_CASE:
    {
      if (parser->switch_nests > 0)
      {
        c_parse_eat(parser, peek.type);

        b32 is_default = peek.type == C_TOKEN_KEYWORD_DEFAULT;

        result = c_new_node(arena, is_default ? C_NODE_DEFAULT : C_NODE_CASE);

        // Grab the match expression if not default.
        if (!is_default)
        {
          C_Node *match_expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
          c_node_add_child(result, match_expression);
        }

        if (c_parse_eat(parser, C_TOKEN_COLON))
        {
          C_Token case_or_close_maybe = c_parse_peek(*parser, 0);

          b32 got_default = is_default;

          // TODO: This seems not great to be checking for close curly here.
          while (case_or_close_maybe.type != C_TOKEN_KEYWORD_CASE    &&
                 case_or_close_maybe.type != C_TOKEN_KEYWORD_DEFAULT &&
                 case_or_close_maybe.type != C_TOKEN_CLOSE_CURLY_BRACE)
          {
            if (case_or_close_maybe.type == C_TOKEN_KEYWORD_DEFAULT)
            {
              if (got_default)
              {
                c_parse_error(parser, "Multiple default cases are not allowed.");
                break;
              }

              got_default = true;
            }

            C_Node *statement = c_parse_statement(arena, parser);
            c_node_add_child(result, statement);

            case_or_close_maybe = c_parse_peek(*parser, 0);
          }
        }
        else
        {
          c_parse_error(parser, "Expected colon following case match expression.");
        }
      }
      else
      {
        c_parse_error(parser, "Case statement without enclosing switch.");
      }
    } break;
    case C_TOKEN_KEYWORD_RETURN:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_RETURN);
      result = c_new_node(arena, C_NODE_RETURN);

      C_Node *expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
      c_node_add_child(result, expression);

      if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
      {
        c_parse_error(parser, "Expected semicolon following return.");
      }
    } break;
    case C_TOKEN_KEYWORD_GOTO:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_GOTO);

      result = c_new_node(arena, C_NODE_GOTO);

      C_Node *label = c_parse_identifier(arena, parser);
      c_node_add_child(result, label);

      if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
      {
        c_parse_error(parser, "Expected semicolon following goto label.");
      }
    } break;
    case C_TOKEN_KEYWORD_BREAK:
    {
      if (parser->loop_nests > 0 || parser->switch_nests > 0)
      {
        c_parse_eat(parser, C_TOKEN_KEYWORD_BREAK);

        result = c_new_node(arena, C_NODE_BREAK);

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
        {
          c_parse_error(parser, "Expected semicolon following break.");
        }
      }
      else
      {
        c_parse_error(parser, "Break statement not in a loop or switch.");
      }
    } break;
    case C_TOKEN_KEYWORD_CONTINUE:
    {
      if (parser->loop_nests > 0)
      {
        c_parse_eat(parser, C_TOKEN_KEYWORD_CONTINUE);

        result = c_new_node(arena, C_NODE_CONTINUE);

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
        {
          c_parse_error(parser, "Expected semicolon following continue.");
        }
      }
      else
      {
        c_parse_error(parser, "Break statement not in a loop or switch.");
      }
    } break;
    case C_TOKEN_BEGIN_CURLY_BRACE:
    {
      result = c_parse_block(arena, parser);
    } break;

    // Everything else
    default:
    {
      b32 at_top_level = false;
      result = c_parse_declaration(arena, parser, at_top_level);

      // We tried to parse a declaration and couldn't.
      // So, this is most likely an expression statement or its just not a statement.
      if (result == c_nil_node())
      {
        result = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);

        // If we got a valid expression back, eat a semicolon.
        if (result != c_nil_node())
        {
          if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
          {
            c_parse_error(parser, "Expected semicolon following expression.");
          }
        }
      }

    } break;
  }

  return result;
}

static
C_Node *c_parse_block(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_new_node(arena, C_NODE_BLOCK);

  if (c_parse_eat(parser, C_TOKEN_BEGIN_CURLY_BRACE))
  {
    while (true)
    {
      C_Node *statement = c_parse_statement(arena, parser);
      if (statement == c_nil_node())
      {
        break;
      }

      c_node_add_child(result, statement);
    }

    if (!c_parse_eat(parser, C_TOKEN_CLOSE_CURLY_BRACE))
    {
      c_parse_error(parser, "Expected closing curly brace for block statement.");
    }
  }
  else
  {
    c_parse_error(parser, "Tried to parse block without begin curly brace.");
  }

  return result;
}

static
C_Node *c_parse_enum_or_struct_declaration(Arena *arena, C_Parser *parser, C_Token_Type enum_or_struct)
{
  ASSERT(enum_or_struct == C_TOKEN_KEYWORD_STRUCT || enum_or_struct == C_TOKEN_KEYWORD_ENUM, "Idiot.");

  b32 is_struct   = enum_or_struct == C_TOKEN_KEYWORD_STRUCT;
  char *node_name = is_struct ? "struct" : "enum";

  C_Node_Type type = is_struct ? C_NODE_STRUCT_DECLARATION : C_NODE_ENUM_DECLARATION;

  C_Node *result = c_nil_node();

  if (c_parse_eat(parser, enum_or_struct))
  {
    result = c_new_node(arena, type);

    // Non-anonymous if we get a non-nil back here.
    C_Node *identifier = c_parse_identifier(arena, parser);
    c_node_add_child(result, identifier);

    if (c_parse_eat(parser, C_TOKEN_BEGIN_CURLY_BRACE))
    {
      C_Token_Type separator = is_struct ? C_TOKEN_SEMICOLON : C_TOKEN_COMMA;

      // Consume members
      while (!c_parse_eat(parser, C_TOKEN_CLOSE_CURLY_BRACE))
      {
        if (is_struct)
        {
          // TODO: Error checking and reporting here
          C_Node *member = c_parse_full_declarators(arena, parser);
          c_node_add_child(result, member);
        }
        else
        {
          C_Node *member = c_parse_identifier(arena, parser);
          c_node_add_child(result, member);

          if (c_parse_eat(parser, C_TOKEN_ASSIGN))
          {
            // Don't want commas at this level to act like operators, so pass comma's precedence
            C_Node *member_expression = c_parse_expression(arena, parser, token_to_node_table[C_TOKEN_COMMA].binary_precedence);
            c_node_add_child(member, member_expression);

            if (member_expression == c_nil_node())
            {
              c_parse_error(parser, "Expected expression following enum member initialization.");
            }
          }
        }

        if (!c_parse_eat(parser, separator))
        {
          char *separator_name = is_struct ? "semicolon" : "comma";
          c_parse_error(parser, "Expected %s following %s member declaration.", separator_name, node_name);
          break;
        }
      }
    }
  }
  else
  {
    c_parse_error(parser, "Tried to parse %s declaration without beginning %s token.", node_name);
  }

  return result;
}

static
C_Node *c_parse_declaration(Arena *arena, C_Parser *parser, b32 at_top_level)
{
  C_Node *result = c_nil_node();

  C_Token token = c_parse_peek(*parser, 0);

  if (token.type == C_TOKEN_SEMICOLON)
  {
    c_parse_eat(parser, C_TOKEN_SEMICOLON);
  }
  // Label
  else if (token.type == C_TOKEN_IDENTIFIER &&
           c_parse_peek(*parser, 1).type == C_TOKEN_COLON)
  {
    result = c_new_node(arena, C_NODE_LABEL);
    C_Node *name = c_parse_identifier(arena, parser);
    c_node_add_child(result, name);

    c_parse_eat(parser, C_TOKEN_COLON);
  }
  else if (token.type == C_TOKEN_KEYWORD_TYPEDEF)
  {
    c_parse_eat(parser, C_TOKEN_KEYWORD_TYPEDEF);

    result = c_new_node(arena, C_NODE_TYPEDEF);

    // Identifier in declarator just becomes alias.
    C_Node *declarator = c_parse_full_declarators(arena, parser);
    c_node_add_child(result, declarator);

    if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
    {
      c_parse_error(parser, "Expected semicolon following typedef statement.");
    }
  }
  // More complex stuff. Valid options include a variable, function, or struct/enum declaration.
  else
  {
    C_Node *maybe_declarator = c_parse_full_declarators(arena, parser);

    // If we were able to get something check for struct/enum or variable or function declaration.
    // FIXME: THis can be majorly simplified now.
    if (maybe_declarator != c_nil_node())
    {
      // NOTE: We need to check if this is JUST a struct/enum declaration without declaring a variable with it. that is, no identifier
      b32 is_only_struct_or_enum = (maybe_declarator->type == C_NODE_STRUCT_DECLARATION ||                           maybe_declarator->type == C_NODE_ENUM_DECLARATION) &&
                                   (maybe_declarator->child_count != 2);

      if (is_only_struct_or_enum)
      {
        // TODO: Would be a good spot to print errors about having uncesseary declarator nonsense here,
        // ie if after_postfix != base_type
        result = maybe_declarator;

        // Check for semicolon if not at top level or if we didn't have a body (no member children).
        b32 check_semicolon = !at_top_level || result->child_count == 0;

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON) && check_semicolon)
        {
          c_parse_error(parser, "Expected semicolon following non-top-level %s declaration",
                        token.type == C_TOKEN_KEYWORD_STRUCT ? "struct" : "enum");
        }
      }
      // Else is a variable / function declaration
      else
      {
        // Holy jank
        b32 is_function = maybe_declarator->child_count == 1 &&
                          maybe_declarator->first_child->first_child->next_sibling->type == C_NODE_TYPE_FUNCTION;
        if (is_function)
        {
          // HACK:
          result = maybe_declarator;
          result->type = C_NODE_FUNCTION_DECLARATION;

          if (c_parse_match(parser, C_TOKEN_BEGIN_CURLY_BRACE))
          {
            if (at_top_level)
            {
              C_Node *definition = c_parse_block(arena, parser);
              c_node_add_child(result, definition);
            }
            else
            {
              c_parse_error(parser, "Function definition only allowed at top level scope.");
            }
          }
        }
        // Variable thing
        else
        {
          result = c_new_node(arena, C_NODE_VARIABLE_DECLARATION);
          c_node_add_child(result, maybe_declarator);

          C_Node *init = c_parse_variable_init(arena, parser);
          c_node_add_child(result, init);

          if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
          {
            c_parse_error(parser, "Expected semicolon following variable declaration.");
          }
        }
      }
    }
  }

  return result;
}

// Returns root node
static
C_Node *parse_c_tokens(Arena *arena, C_Tokenize_Result tokenize_result)
{
  C_Node *root = c_new_node(arena, C_NODE_ROOT);

  C_Parser parser =
  {
    .source = tokenize_result.source,
    .tokens = tokenize_result.tokens,
    .at = 0,
  };

  while (c_parse_incomplete(parser))
  {
    b32 at_top_level = true;
    C_Node *declaration = c_parse_declaration(arena, &parser, at_top_level);
    c_node_add_child(root, declaration);

    if (declaration == c_nil_node())
    {
      break;
    }
  }

  return root;
}
#endif // C_PARSE
