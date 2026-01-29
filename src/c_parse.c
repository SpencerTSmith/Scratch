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
//   - Precedence
//   - Compound literals
//   - Array access
//   - Function calls
//   - Assignment operators

#define C_Node_Type(X)           \
  X(C_NODE_NONE)                 \
  X(C_NODE_VARIABLE)             \
  X(C_NODE_TYPE)                 \
  X(C_NODE_LITERAL)              \
  X(C_NODE_ROOT)                 \
  X(C_NODE_VARIABLE_DECLARATION) \
  X(C_NODE_UNARY)                \
  X(C_NODE_BINARY)               \
  X(C_NODE_COUNT)

ENUM_TABLE(C_Node_Type);

#define C_MIN_PRECEDENCE INT32_MIN

typedef enum C_Binary
{
  C_BINARY_NONE,

  C_BINARY_ADD,
  C_BINARY_SUBTRACT,
  C_BINARY_MULTIPLY,
  C_BINARY_DIVIDE,
  C_BINARY_MODULO,
  C_BINARY_XOR,
  C_BINARY_BITWISE_AND,
  C_BINARY_BITWISE_OR,
  C_BINARY_ACCESS,
  C_BINARY_POINTER_ACCESS,
  C_BINARY_COMPARE_EQUAL,
  C_BINARY_COMPARE_NOT_EQUAL,
  C_BINARY_LESS_THAN,
  C_BINARY_LESS_THAN_EQUAL,
  C_BINARY_GREATER_THAN,
  C_BINARY_GREATER_THAN_EQUAL,
  C_BINARY_LOGICAL_AND,
  C_BINARY_LOGICAL_OR,
  C_BINARY_LEFT_SHIFT,
  C_BINARY_RIGHT_SHIFT,

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

// Could easily convert these to lookups
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

b32 c_token_is_unary_operator(C_Token token)
{
  C_Token_Type t = token.type;

  // Apparently add is also a unary?
  b32 result = t == C_TOKEN_MINUS       ||
               t == C_TOKEN_STAR        ||
               t == C_TOKEN_INCREMENT   ||
               t == C_TOKEN_DECREMENT   ||
               t == C_TOKEN_ADD         ||
               t == C_TOKEN_BITWISE_AND ||
               t == C_TOKEN_BITWISE_NOT ||
               t == C_TOKEN_LOGICAL_NOT;

  return result;
}

// TODO: account for comma operator... perhaps a flag if we are within a parenthetical
b32 c_token_is_binary_operator(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = t == C_TOKEN_STAR               ||
               t == C_TOKEN_ADD                ||
               t == C_TOKEN_MINUS              ||
               t == C_TOKEN_DIVIDE             ||
               t == C_TOKEN_MODULO             ||
               t == C_TOKEN_XOR                ||
               t == C_TOKEN_BITWISE_AND        ||
               t == C_TOKEN_BITWISE_OR         ||
               t == C_TOKEN_DOT                ||
               t == C_TOKEN_ARROW              ||
               t == C_TOKEN_COMPARE_EQUAL      ||
               t == C_TOKEN_LESS_THAN          ||
               t == C_TOKEN_LESS_THAN_EQUAL    ||
               t == C_TOKEN_GREATER_THAN       ||
               t == C_TOKEN_GREATER_THAN_EQUAL ||
               t == C_TOKEN_COMPARE_NOT_EQUAL  ||
               t == C_TOKEN_LOGICAL_AND        ||
               t == C_TOKEN_LOGICAL_OR;


  return result;
}

void c_parse_error(usize line, const char *message)
{
  LOG_ERROR("[Line: %llu] %s", line, message);
}

// Feels like there is a way to do this wihout repeating self so much
C_Binary c_token_to_binary(C_Token token)
{
  C_Binary result = C_BINARY_NONE;

  switch (token.type)
  {
    default: { LOG_ERROR("Unable to convert token to binary"); } break;
    case C_TOKEN_STAR:               { result = C_BINARY_MULTIPLY; } break;
    case C_TOKEN_ADD:                { result = C_BINARY_ADD; } break;
    case C_TOKEN_MINUS:              { result = C_BINARY_ADD; } break;
    case C_TOKEN_DIVIDE:             { result = C_BINARY_DIVIDE; } break;
    case C_TOKEN_MODULO:             { result = C_BINARY_MODULO; } break;
    case C_TOKEN_XOR:                { result = C_BINARY_XOR; } break;
    case C_TOKEN_BITWISE_AND:        { result = C_BINARY_BITWISE_AND; } break;
    case C_TOKEN_BITWISE_OR:         { result = C_BINARY_BITWISE_OR; }  break;
    case C_TOKEN_DOT:                { result = C_BINARY_ACCESS; } break;
    case C_TOKEN_ARROW:              { result = C_BINARY_POINTER_ACCESS; } break;
    case C_TOKEN_COMPARE_EQUAL:      { result = C_BINARY_COMPARE_EQUAL; } break;
    case C_TOKEN_LESS_THAN:          { result = C_BINARY_LESS_THAN; } break;
    case C_TOKEN_LESS_THAN_EQUAL:    { result = C_BINARY_LESS_THAN_EQUAL; } break;
    case C_TOKEN_GREATER_THAN:       { result = C_BINARY_GREATER_THAN; } break;
    case C_TOKEN_GREATER_THAN_EQUAL: { result = C_BINARY_GREATER_THAN_EQUAL; } break;
    case C_TOKEN_COMPARE_NOT_EQUAL:  { result = C_BINARY_COMPARE_NOT_EQUAL; } break;
    case C_TOKEN_LOGICAL_AND:        { result = C_BINARY_LOGICAL_OR; } break;
    case C_TOKEN_LOGICAL_OR:         { result = C_BINARY_LOGICAL_AND; } break;
    case C_TOKEN_LEFT_SHIFT:         { result = C_BINARY_LEFT_SHIFT; } break;
    case C_TOKEN_RIGHT_SHIFT:        { result = C_BINARY_RIGHT_SHIFT;  } break;
  }

  return result;
}

C_Unary c_token_to_unary(C_Token token, b32 is_post)
{
  C_Unary result = C_UNARY_NONE;

  switch (token.type)
  {
    default: { LOG_ERROR("Unable to convert token to unary"); } break;
    case C_TOKEN_STAR:               { result = C_UNARY_DEREFERENCE; } break;
    case C_TOKEN_MINUS:              { result = C_UNARY_NEGATE; } break;
    case C_TOKEN_BITWISE_AND:        { result = C_UNARY_REFERENCE; } break;
    case C_TOKEN_BITWISE_NOT:        { result = C_UNARY_BITWISE_NOT; }  break;
    case C_TOKEN_LOGICAL_NOT:        { result = C_UNARY_LOGICAL_NOT; } break;
    case C_TOKEN_INCREMENT:          { result = is_post ? C_UNARY_POST_INCREMENT : C_UNARY_PRE_INCREMENT; } break;
    case C_TOKEN_DECREMENT:          { result = is_post ? C_UNARY_POST_DECREMENT : C_UNARY_PRE_DECREMENT; } break;
  }

  return result;
}

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

C_Token c_parse_peek_token(C_Parser parser, isize offset)
{
  C_Token result = {0};

  if (parser.at + offset < parser.tokens.count && ((isize)parser.at + offset) >= 0)
  {
    result = parser.tokens.v[parser.at + offset];
  }

  return result;
}

b32 c_parse_expect(C_Parser parser, C_Token_Type type)
{
  return c_parse_peek_token(parser, 0).type == type;
}

void c_node_add_child(C_Node *parent, C_Node *child)
{
  child->parent = parent;

  DLL_push_last(parent->first_child, parent->last_child, child, next_sibling, prev_sibling);
  parent->child_count += 1;
}

b32 c_parse_incomplete(C_Parser parser)
{
  return parser.at < parser.tokens.count;
}

C_Node *c_nil_node()
{
  static C_Node nil = {0};

  return &nil;
}

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

C_Node *c_parse_function_call(Arena *arena, C_Parser *parser)
{
  // TODO:
  return 0;
}

C_Node *c_parse_expression(Arena *arena, C_Parser *parser, i32 min_precedence);

// TODO: Better name... basically just capture the part that would act like a leaf of a tree...  but not a real leaf as with parentheses
// this will could potentially be a rather large subtree acting like a leaf
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
    C_Token peek = c_parse_peek_token(*parser, 0);

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

    result = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);

    if (!c_parse_expect(*parser, C_TOKEN_CLOSE_PARENTHESIS))
    {
      LOG_ERROR("Expected close parenthesis");
    }
    else
    {
      parser->at += 1;
    }
  }

  // Check if we have a post unary operator, rebuild result tree if so
  C_Token post_unary_peek = c_parse_peek_token(*parser, 0);
  if (post_unary_peek.type == C_TOKEN_INCREMENT || post_unary_peek.type == C_TOKEN_DECREMENT)
  {
    // Save the old as we need to reattach as a child of the unary
    C_Node *save = result;

    result = arena_new(arena, C_Node);
    result->type = C_NODE_UNARY;

    b32 is_post = true;
    result->unary = c_token_to_unary(post_unary_peek, true);

    parser->at += 1;

    c_node_add_child(result, save);
  }

  return result;
}

C_Node *c_parse_expression(Arena *arena, C_Parser *parser, i32 min_precedence)
{
  C_Node *left = c_parse_expression_start(arena, parser);

  C_Node *result = left; // Return just the left if we don't meet later checks

  while (true)
  {
    C_Token peek = c_parse_peek_token(*parser, 0);

    if (c_token_is_binary_operator(peek))
    {
      C_Binary operator  = c_token_to_binary(peek);
      i32 new_precedence = c_binary_precedence(operator);

      if (new_precedence > min_precedence)
      {
        result = arena_new(arena, C_Node);
        result->type = C_NODE_BINARY;
        result->binary = operator;

        parser->at += 1;

        C_Node *right = c_parse_expression(arena, parser, new_precedence);

        c_node_add_child(result, left);
        c_node_add_child(result, right);
      }
      else
      {
        // We need to head back up the stack to parse the next operator,
        // need a lower precedence operator
        break;
      }
    }
    else
    {
      // No more operators, we are done.
      break;
    }

    // Now left hand side becomes this new tree
    left = result;
  }
#endif

  return result;
}

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

    C_Node *expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
    c_node_add_child(result, expression); // Third child is initializing expression, if present
  }

  C_Token wish_semicolon = c_parse_peek_token(*parser, 0);
  if (wish_semicolon.type == C_TOKEN_SEMICOLON)
  {
    parser->at += 1;
  }
  else
  {
    c_parse_error(c_parse_peek_token(*parser, -1).line,
                  "Declaration without subsequent semicolon");
  }

  return result;
}

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
