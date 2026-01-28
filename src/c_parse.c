#ifndef C_PARSE
#define C_PARSE

#include "common.h"

#include "c_tokenize.h"

#define C_Node_Type(X)           \
  X(C_NODE_NONE)                 \
  X(C_NODE_VARIABLE)             \
  X(C_NODE_TYPE)                 \
  X(C_NODE_LITERAL)              \
  X(C_NODE_ROOT)                 \
  X(C_NODE_VARIABLE_DECLARATION) \
  X(C_NODE_UNARY)                \
  X(C_NODE_BINARY)               \
  X(C_NODE_PARENTHETICAL)        \
  X(C_NODE_COUNT)

ENUM_TABLE(C_Node_Type);

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
  C_BINARY_DOT,
  C_BINARY_ARROW,
  C_BINARY_COMPARE_EQUAL,
  C_BINARY_COMPARE_NOT_EQUAL,
  C_BINARY_LESS_THAN,
  C_BINARY_LESS_THAN_EQUAL,
  C_BINARY_GREATER_THAN,
  C_BINARY_GREATER_THAN_EQUAL,
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
    String    variable_name;
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
    case C_TOKEN_DOT:                { result = C_BINARY_DOT; } break;
    case C_TOKEN_ARROW:              { result = C_BINARY_ARROW; } break;
    case C_TOKEN_COMPARE_EQUAL:      { result = C_BINARY_COMPARE_EQUAL; } break;
    case C_TOKEN_LESS_THAN:          { result = C_BINARY_LESS_THAN; } break;
    case C_TOKEN_LESS_THAN_EQUAL:    { result = C_BINARY_LESS_THAN_EQUAL; } break;
    case C_TOKEN_GREATER_THAN:       { result = C_BINARY_GREATER_THAN; } break;
    case C_TOKEN_GREATER_THAN_EQUAL: { result = C_BINARY_GREATER_THAN_EQUAL; } break;
    case C_TOKEN_COMPARE_NOT_EQUAL:  { result = C_BINARY_COMPARE_NOT_EQUAL; } break;
    case C_TOKEN_LOGICAL_AND:        { result = C_BINARY_LOGICAL_OR; } break;
    case C_TOKEN_LOGICAL_OR:         { result = C_BINARY_LOGICAL_AND; } break;
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
  i32 result = INT32_MIN;

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

  if (parent)
  {
    DLL_push_last(parent->first_child, parent->last_child, child, next_sibling, prev_sibling);
    parent->child_count += 1;
  }
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

C_Node *c_parse_variable(Arena *arena, C_Parser *parser, C_Node *parent)
{
  C_Node *result = arena_new(arena, C_Node);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (token.type == C_TOKEN_IDENTIFIER)
  {
    result->type = C_NODE_VARIABLE;
    result->variable_name = token.raw;

    c_node_add_child(parent, result);

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

C_Node *c_parse_expression(Arena *arena, C_Parser *parser, C_Node *parent);

// TODO: Better name... basically just capture the part that would act like a leaf of a tree...  but not a real leaf as with parentheses
// this will could potentially be a rather large subtree acting like a leaf
C_Node *c_parse_expression_start(Arena *arena, C_Parser *parser, C_Node *parent)
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
    result->type = C_NODE_UNARY;

    b32 is_post = false;
    result->unary = c_token_to_unary(token, is_post);

    parser->at += 1;

    C_Node *unary_child = c_parse_expression_start(arena, parser, result);
  }
  else if (token.type == C_TOKEN_IDENTIFIER)
  {
    C_Token peek = c_parse_peek_token(*parser, 0);

    if (peek.type != C_TOKEN_BEGIN_PARENTHESIS)
    {
      result = c_parse_variable(arena, parser, parent);
    }
    else
    {
      // TODO: Function call
    }
  }
  else if (token.type == C_TOKEN_BEGIN_PARENTHESIS)
  {
    // New parenthetical group
    result = arena_new(arena, C_Node);
    result->type = C_NODE_PARENTHETICAL;

    parser->at += 1;

    C_Node *inner = c_parse_expression(arena, parser, result);

    if (!c_parse_expect(*parser, C_TOKEN_CLOSE_PARENTHESIS))
    {
      LOG_ERROR("Expected close parenthesis");
    }
    else
    {
      parser->at += 1;
    }
  }

  c_node_add_child(parent, result);

  return result;
}

C_Node *c_parse_expression(Arena *arena, C_Parser *parser, C_Node *parent)
{
  // We don't know the parent yet, so pass 0
  C_Node *left = c_parse_expression_start(arena, parser, 0);

  C_Node *result = left; // Return just the left if we don't meet later checks

  C_Token peek = c_parse_peek_token(*parser, 0);
  if (c_token_is_binary_operator(peek))
  {
    result = arena_new(arena, C_Node);
    result->type = C_NODE_BINARY;
    result->binary = c_token_to_binary(peek);

    parser->at += 1; // now skip the binop token

    // Left should be child of operator
    c_node_add_child(result, left);

    // Grab right
    C_Node *right = c_parse_expression(arena, parser, result);
  }
  // catch post increment, decrement here
  else if (c_token_is_unary_operator(peek))
  {
    result = arena_new(arena, C_Node);
    result->type = C_NODE_UNARY;

    b32 is_post = true;
    result->unary = c_token_to_unary(peek, true);

    parser->at += 1;

    c_node_add_child(result, left);
  }
  else if (peek.type == C_TOKEN_CLOSE_PARENTHESIS)
  {
    // Do nothing and return, checking for close will be done by caller
  }

  c_node_add_child(parent, result);

  return result;
}

C_Node *c_parse_type(Arena *arena, C_Parser *parser, C_Node *parent)
{
  C_Node *result = arena_new(arena, C_Node);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (c_token_is_type_keyword(token))
  {
    result->type = C_NODE_TYPE;
    result->variable_name = token.raw; // Hehehe, nice to just do this

    c_node_add_child(parent, result);
  }
  // TODO: probably should build a data structure keeping track of structs, typedefs, etc
  else if (token.type == C_TOKEN_IDENTIFIER) // Custom type
  {
  }

  // Consume
  parser->at += 1;

  return result;
}

C_Node *c_parse_variable_declaration(Arena *arena, C_Parser *parser, C_Node *parent)
{
  C_Node *result = arena_new(arena, C_Node);
  result->type = C_NODE_VARIABLE_DECLARATION;
  c_node_add_child(parent, result);

  C_Node *type_node = c_parse_type(arena, parser, result);
  C_Node *name_node = c_parse_variable(arena, parser, result);

  C_Token peek = c_parse_peek_token(*parser, 0);

  if (peek.type == C_TOKEN_ASSIGN)
  {
    // Skip euqals sign
    parser->at += 1;

    c_parse_expression(arena, parser, result);
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

C_Node *c_parse_function_declaration(Arena *arena, C_Parser *parser, C_Node *parent)
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
          c_parse_function_declaration(arena, &parser, root);
        }
        // Variable thing
        else
        {
          c_parse_variable_declaration(arena, &parser, root);
        }
      }
    }
  }

  return root;
}
#endif // C_PARSE
