#ifndef C_PARSE
#define C_PARSE

#include "common.h"

#include "c_tokenize.h"

// TODO:
// - Statements
//   - Blocks
//   - for
//   - while
//   - if
//   - switch
//   - do while
// - Expressions
//   - Compound literals
//   - Array access
//   - Assignment operators
//   - Comma operator

#define C_Node_Type(X)           \
  X(C_NODE_NONE)                 \
  X(C_NODE_VARIABLE)             \
  X(C_NODE_TYPE)                 \
  X(C_NODE_LITERAL)              \
  X(C_NODE_ROOT)                 \
  X(C_NODE_VARIABLE_DECLARATION) \
  X(C_NODE_UNARY)                \
  X(C_NODE_BINARY)               \
  X(C_NODE_TERNARY)              \
  X(C_NODE_FUNCTION_CALL)        \
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

  C_UNARY_COUNT,
} C_Unary;

typedef struct C_Node C_Node;
struct C_Node
{
  C_Node_Type type;

  C_Node *parent;

  C_Node *first_child;
  C_Node *last_child;
  usize  child_count;

  C_Node *next_sibling;
  C_Node *prev_sibling;

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
  C_Token_Array tokens;
  usize at;
};

typedef struct C_Token_Node_Mappings C_Token_Node_Mappings;
struct C_Token_Node_Mappings
{
  C_Unary  unary;
  C_Binary binary;
};

static
C_Token_Node_Mappings token_to_node_table[] =
{
  [C_TOKEN_MINUS]              = { C_UNARY_NEGATE,        C_BINARY_SUBTRACT },
  [C_TOKEN_STAR]               = { C_UNARY_DEREFERENCE,   C_BINARY_MULTIPLY },
  [C_TOKEN_INCREMENT]          = { C_UNARY_PRE_INCREMENT, C_BINARY_NONE }, // NOTE: Special-case
  [C_TOKEN_DECREMENT]          = { C_UNARY_PRE_DECREMENT, C_BINARY_NONE }, // NOTE: Special-case
  [C_TOKEN_ADD]                = { C_UNARY_PLUS,          C_BINARY_ADD },
  [C_TOKEN_BITWISE_AND]        = { C_UNARY_REFERENCE,     C_BINARY_BITWISE_AND},
  [C_TOKEN_BITWISE_NOT]        = { C_UNARY_BITWISE_NOT,   C_BINARY_NONE },
  [C_TOKEN_LOGICAL_NOT]        = { C_UNARY_LOGICAL_NOT,   C_BINARY_NONE },
  [C_TOKEN_LEFT_SHIFT]         = { C_UNARY_NONE,          C_BINARY_LEFT_SHIFT },
  [C_TOKEN_RIGHT_SHIFT]        = { C_UNARY_NONE,          C_BINARY_RIGHT_SHIFT },
  [C_TOKEN_BITWISE_OR]         = { C_UNARY_NONE,          C_BINARY_BITWISE_OR},
  [C_TOKEN_XOR]                = { C_UNARY_NONE,          C_BINARY_XOR },
  [C_TOKEN_DOT]                = { C_UNARY_NONE,          C_BINARY_ACCESS },
  [C_TOKEN_ARROW]              = { C_UNARY_NONE,          C_BINARY_POINTER_ACCESS },
  [C_TOKEN_COMPARE_EQUAL]      = { C_UNARY_NONE,          C_BINARY_COMPARE_EQUAL },
  [C_TOKEN_COMPARE_NOT_EQUAL]  = { C_UNARY_NONE,          C_BINARY_COMPARE_NOT_EQUAL },
  [C_TOKEN_LESS_THAN]          = { C_UNARY_NONE,          C_BINARY_LESS_THAN },
  [C_TOKEN_LESS_THAN_EQUAL]    = { C_UNARY_NONE,          C_BINARY_LESS_THAN_EQUAL },
  [C_TOKEN_GREATER_THAN]       = { C_UNARY_NONE,          C_BINARY_GREATER_THAN },
  [C_TOKEN_GREATER_THAN_EQUAL] = { C_UNARY_NONE,          C_BINARY_GREATER_THAN_EQUAL },
  [C_TOKEN_LOGICAL_AND]        = { C_UNARY_NONE,          C_BINARY_LOGICAL_AND },
  [C_TOKEN_LOGICAL_OR]         = { C_UNARY_NONE,          C_BINARY_LOGICAL_OR },
};

static
C_Token_Node_Mappings c_token_to_node_mapping(C_Token_Type type)
{
  C_Token_Node_Mappings result = {0};

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

  b32 result = t == C_TOKEN_KEYWORD_VOID  ||
               t == C_TOKEN_KEYWORD_CHAR  ||
               t == C_TOKEN_KEYWORD_SHORT ||
               t == C_TOKEN_KEYWORD_INT   ||
               t == C_TOKEN_KEYWORD_LONG  ||
               t == C_TOKEN_KEYWORD_FLOAT ||
               t == C_TOKEN_KEYWORD_DOUBLE;

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

// Feels like there is a way to do this wihout repeating self so much
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
    if (result == C_UNARY_PRE_DECREMENT)
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
i32 c_binary_precedence(C_Binary binary)
{
  i32 result = C_MIN_PRECEDENCE;

  switch (binary)
  {
    default: { LOG_ERROR("Tried to attain precedence for invalid binary operator"); } break;
    case C_BINARY_ACCESS:             { result = 14; } break;
    case C_BINARY_POINTER_ACCESS:     { result = 14; } break;
    case C_BINARY_MULTIPLY:           { result = 12; } break;
    case C_BINARY_DIVIDE:             { result = 12; } break;
    case C_BINARY_MODULO:             { result = 12; } break;
    case C_BINARY_ADD:                { result = 11; } break;
    case C_BINARY_SUBTRACT:           { result = 11; } break;
    case C_BINARY_LEFT_SHIFT:         { result = 10; } break;
    case C_BINARY_RIGHT_SHIFT:        { result = 10; } break;
    case C_BINARY_LESS_THAN:          { result = 9; } break;
    case C_BINARY_LESS_THAN_EQUAL:    { result = 9; } break;
    case C_BINARY_GREATER_THAN:       { result = 9; } break;
    case C_BINARY_GREATER_THAN_EQUAL: { result = 9; } break;
    case C_BINARY_COMPARE_EQUAL:      { result = 8; } break;
    case C_BINARY_COMPARE_NOT_EQUAL:  { result = 8; } break;
    case C_BINARY_BITWISE_AND:        { result = 7; } break;
    case C_BINARY_XOR:                { result = 6; } break;
    case C_BINARY_BITWISE_OR:         { result = 5; } break;
    case C_BINARY_LOGICAL_AND:        { result = 4; } break;
    case C_BINARY_LOGICAL_OR:         { result = 3; } break;
  }

  return result;
}

static
C_Token c_parse_peek_token(C_Parser parser, isize offset)
{
  C_Token result = {0};

  if (parser.at + offset < parser.tokens.count && ((isize)parser.at + offset) >= 0)
  {
    result = parser.tokens.v[parser.at + offset];
  }

  return result;
}

static
b32 c_parse_current_is(C_Parser parser, C_Token_Type type)
{
  return c_parse_peek_token(parser, 0).type == type;
}

static
void c_node_add_child(C_Node *parent, C_Node *child)
{
  child->parent = parent;

  DLL_push_last(parent->first_child, parent->last_child, child, next_sibling, prev_sibling);
  parent->child_count += 1;
}

static
b32 c_parse_incomplete(C_Parser parser)
{
  return parser.at < parser.tokens.count;
}

static
C_Node *c_nil_node()
{
  static C_Node nil = {0};

  return &nil;
}

static
C_Node *c_parse_variable(Arena *arena, C_Parser *parser)
{
  C_Node *result = arena_new(arena, C_Node);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (token.type == C_TOKEN_IDENTIFIER)
  {
    result->type = C_NODE_VARIABLE;
    result->name = token.raw;

    // TODO: the rest
  }
  else
  {
    LOG_ERROR("Tried to parse non-identifier token as variable");
  }

  // Consume
  parser->at += 1;

  return result;
}

static
C_Node *c_parse_expression(Arena *arena, C_Parser *parser);

static
C_Node *c_parse_function_call(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Token identifier = c_parse_peek_token(*parser, 0);

  if (identifier.type == C_TOKEN_IDENTIFIER)
  {
    result = arena_new(arena, C_Node);

    result->type = C_NODE_FUNCTION_CALL;
    result->name = identifier.raw;

    parser->at += 1;

    if (c_parse_current_is(*parser, C_TOKEN_BEGIN_PARENTHESIS))
    {
      parser->at += 1;

      while (!c_parse_current_is(*parser, C_TOKEN_CLOSE_PARENTHESIS))
      {
        C_Node *argument = c_parse_expression(arena, parser);
        c_node_add_child(result, argument);

        // Skip comma
        if (c_parse_current_is(*parser, C_TOKEN_COMMA))
        {
          parser->at += 1;
        }
      }

      parser->at += 1; // skip the final parenthesis
    }
    else
    {
      LOG_ERROR("Expected function call to have open parenthesis");
    }
  }
  else
  {
    LOG_ERROR("Expected function call to begin with identifier");
  }

  return result;
}

// TODO: Better name... basically just capture the part that would act like a leaf of a tree...  but not a real leaf as with parentheses
// this will could potentially be a rather large subtree acting like a leaf
static
C_Node *c_parse_expression_start(Arena *arena, C_Parser *parser)
{
  C_Token token = c_parse_peek_token(*parser, 0);

  C_Node *result = c_nil_node();

  if (token.type == C_TOKEN_LITERAL)
  {
    result = arena_new(arena, C_Node);
    result->type = C_NODE_LITERAL;

    result->literal = token.literal; // Copy over

    parser->at += 1;
  }
  else if (c_token_is_unary_operator(token))
  {
    result = arena_new(arena, C_Node);
    result->type = C_NODE_UNARY;

    b32 is_post = false;
    result->unary = c_token_to_unary(token, is_post);

    parser->at += 1;

    // Only parse a single 'thing', i.e. recurse so we only parse a larger expression if parenthesis
    C_Node *child = c_parse_expression_start(arena, parser);
    c_node_add_child(result, child);
  }
  else if (token.type == C_TOKEN_IDENTIFIER)
  {
    C_Token peek = c_parse_peek_token(*parser, 1);

    if (peek.type != C_TOKEN_BEGIN_PARENTHESIS)
    {
      result = c_parse_variable(arena, parser);
    }
    else
    {
      result = c_parse_function_call(arena, parser);
    }
  }
  else if (token.type == C_TOKEN_BEGIN_PARENTHESIS)
  {
    parser->at += 1;

    // Reset as if this expression is all by itself by passing the min precedence
    result = c_parse_expression(arena, parser);

    if (!c_parse_current_is(*parser, C_TOKEN_CLOSE_PARENTHESIS))
    {
      LOG_ERROR("Expected close parenthesis");
    }
    else
    {
      parser->at += 1;
    }
  }

  C_Token post_peek = c_parse_peek_token(*parser, 0);

  // Check if we have a post unary operator, rebuild result tree if so
  if (post_peek.type == C_TOKEN_INCREMENT || post_peek.type == C_TOKEN_DECREMENT)
  {
    // Save the old as we need to reattach as a child of the unary
    C_Node *save = result;

    result = arena_new(arena, C_Node);
    result->type = C_NODE_UNARY;

    b32 is_post = true;
    result->unary = c_token_to_unary(post_peek, true);

    parser->at += 1;

    c_node_add_child(result, save);
  }

  return result;
}

static
C_Node *c_parse_binary_expression(Arena *arena, C_Parser *parser, i32 min_precedence)
{
  C_Node *left = c_parse_expression_start(arena, parser);

  C_Node *result = left; // Return just the left if we don't meet later checks

  // Idea from https://www.youtube.com/watch?v=fIPO4G42wYE&t=4260s
  // Currently does not allow for right associative operator of same precedence
  while (true)
  {
    C_Token peek = c_parse_peek_token(*parser, 0);

    if (c_token_is_binary_operator(peek))
    {
      C_Binary operator  = c_token_to_binary(peek);
      i32 new_precedence = c_binary_precedence(operator);

      if (new_precedence > min_precedence)
      {
        // We can recurse down and build a deeper tree

        result = arena_new(arena, C_Node);
        result->type = C_NODE_BINARY;
        result->binary = operator;

        parser->at += 1;

        C_Node *right = c_parse_binary_expression(arena, parser, new_precedence);

        c_node_add_child(result, left);
        c_node_add_child(result, right);
      }
      else
      {
        // We need to head back up the tree to parse the next operator,
        // need a lower precedence operator
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

  return result;
}

static
C_Node *c_parse_expression(Arena *arena, C_Parser *parser)
{
  // By default just a binary expression.
  C_Node *result = c_parse_binary_expression(arena, parser, C_MIN_PRECEDENCE);

  C_Token post_peek = c_parse_peek_token(*parser, 0);

  // Check for ternary if we are the root of an expression, i.e. we were called with the absolute minimum precedence,
  // if so grab it, rebuild result tree
  if (post_peek.type == C_TOKEN_QUESTION)
  {
    parser->at += 1;

    // We know what we have so far is the condition for the ternary.
    C_Node *condition = result;

    result = arena_new(arena, C_Node);
    result->type = C_NODE_TERNARY;

    c_node_add_child(result, condition);

    C_Node *true_expression = c_parse_expression(arena, parser);
    c_node_add_child(result, true_expression);

    if (!c_parse_current_is(*parser, C_TOKEN_COLON))
    {
      LOG_ERROR("Expected colon after 2nd ternary argument.");
    }
    else
    {
      parser->at += 1;
    }

    C_Node *false_expression = c_parse_expression(arena, parser);
    c_node_add_child(result, false_expression);
  }

  return result;
}

static
C_Node *c_parse_type(Arena *arena, C_Parser *parser)
{
  C_Node *result = arena_new(arena, C_Node);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (c_token_is_type_keyword(token))
  {
    result->type = C_NODE_TYPE;
    result->name = token.raw; // Hehehe, nice to just do this
  }
  // TODO: probably should build a data structure keeping track of structs, typedefs, etc
  else if (token.type == C_TOKEN_IDENTIFIER) // Custom type
  {
  }

  // Consume
  parser->at += 1;

  return result;
}

static
C_Node *c_parse_variable_declaration(Arena *arena, C_Parser *parser)
{
  C_Node *result = arena_new(arena, C_Node);
  result->type = C_NODE_VARIABLE_DECLARATION;

  C_Node *type_node = c_parse_type(arena, parser);
  c_node_add_child(result, type_node); // First child is type of variable

  C_Node *name_node = c_parse_variable(arena, parser);
  c_node_add_child(result, name_node); // Second child is name of variable

  C_Token peek = c_parse_peek_token(*parser, 0);

  if (peek.type == C_TOKEN_ASSIGN)
  {
    // Skip euqals sign
    parser->at += 1;

    C_Node *expression = c_parse_expression(arena, parser);
    c_node_add_child(result, expression); // Third child is initializing expression, if present
  }

  C_Token wish_semicolon = c_parse_peek_token(*parser, 0);
  if (wish_semicolon.type == C_TOKEN_SEMICOLON)
  {
    parser->at += 1;
  }
  else
  {
    LOG_ERROR("Declaration without subsequent semicolon");
  }

  return result;
}

static
C_Node *c_parse_function_declaration(Arena *arena, C_Parser *parser)
{
  return 0;
}

// Returns root node
static
C_Node *parse_c_tokens(Arena *arena, C_Token_Array tokens)
{

  C_Node *root = arena_new(arena, C_Node);
  root->type = C_NODE_ROOT;

  C_Parser parser =
  {
    .tokens = tokens,
    .at = 0,
  };

  while (c_parse_incomplete(parser))
  {
    usize to_consume = 0;

    C_Token token = c_parse_peek_token(parser, 0);

    // TODO: Should also check for declaration if it is an identifier that is a type too
    if (c_token_is_type_keyword(token))
    {
      C_Token peek0 = c_parse_peek_token(parser, 1);
      C_Token peek1 = c_parse_peek_token(parser, 2);

      // Declaration, probably
      if (peek0.type == C_TOKEN_IDENTIFIER)
      {
        // Function thing
        if (peek1.type == C_TOKEN_BEGIN_PARENTHESIS)
        {
          C_Node *function_declaration = c_parse_function_declaration(arena, &parser);
          c_node_add_child(root, function_declaration);
        }
        // Variable thing
        else
        {
          C_Node *variable_declaration = c_parse_variable_declaration(arena, &parser);
          c_node_add_child(root, variable_declaration);
        }
      }
    }
  }

  return root;
}
#endif // C_PARSE
