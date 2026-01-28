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
    String variable_name;
    struct
    {


    };
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
               t == C_TOKEN_LOGICAL_AND ||
               t == C_TOKEN_BITWISE_NOT ||
               t == C_TOKEN_LOGICAL_NOT;

  return result;
}

// TODO: account for comma operator... perhaps a flag if we are within a parenthetical
b32 c_token_is_binary_operator(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = t == C_TOKEN_STAR ||
               t == C_TOKEN_ADD  ||
               t == C_TOKEN_MINUS ||
               t == C_TOKEN_DIVIDE ||
               t == C_TOKEN_MODULO ||
               t == C_TOKEN_XOR ||
               t == C_TOKEN_BITWISE_AND ||
               t == C_TOKEN_BITWISE_OR ||
               t == C_TOKEN_DOT ||
               t == C_TOKEN_ARROW ||
               t == C_TOKEN_BITWISE_NOT ||
               t == C_TOKEN_COMPARE_EQUAL ||
               t == C_TOKEN_LESS_THAN ||
               t == C_TOKEN_LESS_THAN_EQUAL ||
               t == C_TOKEN_GREATER_THAN ||
               t == C_TOKEN_GREATER_THAN_EQUAL ||
               t == C_TOKEN_COMPARE_NOT_EQUAL ||
               t == C_TOKEN_LOGICAL_AND ||
               t == C_TOKEN_LOGICAL_OR;


  return result;
}

void c_parse_error(usize line, const char *message)
{
  LOG_ERROR("[Line: %llu] %s", line, message);
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

C_Node *c_parse_expression(Arena *arena, C_Parser *parser, C_Node *parent);

// TODO: Better name... basically just capture the part that would act like a leaf of a tree...  but not a real leaf as with parentheses
// this will could potentially be a rather large subtree acting like a leaf
C_Node *c_parse_expression_start(Arena *arena, C_Parser *parser, C_Node *parent)
{
  C_Token token = c_parse_peek_token(*parser, 0);

  C_Node *result = arena_new(arena, C_Node);

  if (token.type == C_TOKEN_LITERAL)
  {
    result->type = C_NODE_LITERAL;
    parser->at += 1;
  }
  else if (c_token_is_unary_operator(token))
  {
    result->type = C_NODE_UNARY;
    parser->at += 1;

    C_Node *unary_child = c_parse_expression_start(arena, parser, result);
  }
  else if (token.type == C_TOKEN_IDENTIFIER)
  {
    C_Token peek = c_parse_peek_token(*parser, 0);

    if (peek.type != C_TOKEN_BEGIN_PARENTHESIS)
    {
      result->type = C_NODE_VARIABLE;
      parser->at += 1;
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

    parser->at += 1;

    c_node_add_child(result, left);
  }
  else if (peek.type == C_TOKEN_CLOSE_PARENTHESIS)
  {
    // FIXME: Should just skip this here? Or have caller expect a closed parenthesis?
    parser->at += 1;
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

C_Node *c_parse_variable(Arena *arena, C_Parser *parser, C_Node *parent)
{
  C_Node *result = arena_new(arena, C_Node);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (token.type == C_TOKEN_IDENTIFIER)
  {
    result->type = C_NODE_VARIABLE;
    result->variable_name = token.raw;

    // TODO: probably need to do other stuff?
    c_node_add_child(parent, result);
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
