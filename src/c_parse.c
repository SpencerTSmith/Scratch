#ifndef C_PARSE
#define C_PARSE

#include "common.h"

#include "c_tokenize.h"

#define C_Node_Type(X)       \
  X(C_NODE_NONE)             \
  X(C_NODE_VARIABLE)         \
  X(C_NODE_TYPE)             \
  X(C_NODE_CONSTANT)         \
  X(C_NODE_ROOT)             \
  X(C_NODE_DECLARATION)      \
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

  String name;
};

typedef struct C_Parser C_Parser;
struct C_Parser
{
  C_Token_Array tokens;
  usize at;
};

b32 c_token_is_type_keyword(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = token.type == C_TOKEN_KEYWORD_VOID  ||
               token.type == C_TOKEN_KEYWORD_CHAR  ||
               token.type == C_TOKEN_KEYWORD_SHORT ||
               token.type == C_TOKEN_KEYWORD_INT   ||
               token.type == C_TOKEN_KEYWORD_LONG  ||
               token.type == C_TOKEN_KEYWORD_FLOAT ||
               token.type == C_TOKEN_KEYWORD_DOUBLE;

  return result;
}

b32 c_token_is_literal(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = token.type == C_TOKEN_LITERAL_CHAR   ||
               token.type == C_TOKEN_LITERAL_INT    ||
               token.type == C_TOKEN_LITERAL_DOUBLE ||
               token.type == C_TOKEN_LITERAL_STRING;

  return result;
}

C_Token c_parse_peek_token(C_Parser parser, usize offset)
{
  C_Token result = {0};

  if (parser.at + offset < parser.tokens.count)
  {
    result = parser.tokens.v[parser.at + offset];
  }

  return result;
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

C_Node *c_parse_expression(C_Parser *parser)
{

}

C_Node *c_parse_type(Arena *arena, C_Parser *parser, C_Node *parent)
{
  C_Node *result = arena_new(arena, C_Node);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (c_token_is_type_keyword(token))
  {
    result->type = C_NODE_TYPE;
    result->name = token.raw; // Hehehe, nice to just do this

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
    result->name = token.raw;
    // TODO: probably need to do other stuff?
    c_node_add_child(parent, result);
  }

  // Consume
  parser->at += 1;

  return result;
}

C_Node *c_parse_declaration(Arena *arena, C_Parser *parser, C_Node *parent)
{
  C_Node *result = arena_new(arena, C_Node);
  result->type = C_NODE_DECLARATION;
  c_node_add_child(parent, result);

  C_Node *type_node = c_parse_type(arena, parser, result);
  C_Node *name_node = c_parse_variable(arena, parser, result);

  C_Token peek = c_parse_peek_token(*parser, 0);

  if (peek.type == C_TOKEN_ASSIGN)
  {
    // Skip euqals sign
    parser->at += 1;
    // TODO: Parse expression
  }

  C_Token wish_semicolon = c_parse_peek_token(*parser, 0);
  if (wish_semicolon.type == C_TOKEN_SEMICOLON)
  {
    parser->at += 1;
  }
  else
  {
    LOG_ERROR("Declaration without following semicolon");
  }

  return result;
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
          // TODO: Parse function
        }
        // Variable variable thing
        else
        {
          c_parse_declaration(arena, &parser, root);
        }
      }
    }
  }

  return root;
}

#endif // C_PARSE
